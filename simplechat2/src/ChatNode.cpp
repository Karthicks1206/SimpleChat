#include "ChatNode.h"

static QByteArray pack(const WireMessage& w){ return w.toJson(); }
static WireMessage unpack(const QByteArray& b){ return WireMessage::fromJson(b); }

ChatNode::ChatNode(quint16 chatPort, quint16 discoveryPort, QObject* parent)
  : QObject(parent), chatPort_(chatPort), discoveryPort_(discoveryPort) {
  st_.originId = QUuid::createUuid().toString(QUuid::WithoutBraces);

  chatSock_.bind(QHostAddress::AnyIPv4, chatPort_, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
  discSock_.bind(QHostAddress::AnyIPv4, discoveryPort_, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

  connect(&chatSock_, &QUdpSocket::readyRead, this, &ChatNode::onReadyReadChat);
  connect(&discSock_, &QUdpSocket::readyRead, this, &ChatNode::onReadyReadDiscovery);

  resendTimer_.setInterval(400);
  connect(&resendTimer_, &QTimer::timeout, this, &ChatNode::onResendTick);
  resendTimer_.start();

  antiEntropyTimer_.setInterval(1500);
  connect(&antiEntropyTimer_, &QTimer::timeout, this, &ChatNode::onAntiEntropyTick);
  antiEntropyTimer_.start();

  broadcastHello();
}

void ChatNode::addPeer(const QHostAddress& addr, quint16 port) {
  if (peers_.add(addr, port)) emit peerListChanged(peers_.peers);
}

void ChatNode::broadcastHello() {
  WireMessage w; w.type="hello"; w.origin=st_.originId;
  QByteArray dat = pack(w);
  chatSock_.writeDatagram(dat, QHostAddress::Broadcast, chatPort_);
  discSock_.writeDatagram(dat, QHostAddress::Broadcast, discoveryPort_);
}

void ChatNode::handleHello(const QHostAddress& from, quint16 port) {
  addPeer(from, port);
  WireMessage w; w.type="hello_ack"; w.origin=st_.originId;
  chatSock_.writeDatagram(pack(w), from, port);
}
void ChatNode::handleHelloAck(const QHostAddress& from, quint16 port) {
  addPeer(from, port);
}

void ChatNode::sendChat(const QString& text, qint64 dest) {
  WireMessage w; w.type="chat"; w.origin=st_.originId; w.seq = st_.nextSeq++; w.dest=dest; w.text=text;
  st_.logByOrigin[w.origin][w.seq] = w;
  st_.seen.markSeen(w.origin, w.seq);
  st_.pendingAcks[w.seq] = {w, QElapsedTimer(), 0};
  st_.pendingAcks[w.seq].sinceSent.start();

  for (auto& p : peers_.peers) {
    if (dest==-1 || p.second==dest) chatSock_.writeDatagram(pack(w), p.first, p.second);
  }
  emit receivedChat(w.origin, w.seq, w.text);
}

void ChatNode::onReadyReadChat() {
  while (chatSock_.hasPendingDatagrams()) {
    QNetworkDatagram d = chatSock_.receiveDatagram();
    WireMessage w = unpack(d.data());
    handleMessage(w, d.senderAddress(), d.senderPort());
  }
}
void ChatNode::onReadyReadDiscovery() {
  while (discSock_.hasPendingDatagrams()) {
    QNetworkDatagram d = discSock_.receiveDatagram();
    WireMessage w = unpack(d.data());
    if (w.type=="hello") handleHello(d.senderAddress(), chatPort_);
  }
}

void ChatNode::handleMessage(const WireMessage& w, const QHostAddress& from, quint16 port) {
  if (w.origin == st_.originId) return;

  if (w.type=="hello")         { handleHello(from, port); return; }
  if (w.type=="hello_ack")     { handleHelloAck(from, port); return; }

  addPeer(from, port);

  if (w.type=="chat") {
    sendAck(w.origin, w.seq, from, port);
    if (w.seq > st_.seen.vc.value(w.origin, 0LL)) {
      storeAndDeliver(w);
      for (auto& p : peers_.peers)
        if (!(p.first==from && p.second==port))
          chatSock_.writeDatagram(pack(w), p.first, p.second);
    }
    return;
  }

  if (w.type=="ack") {
    st_.pendingAcks.remove(w.ackOf);
    return;
  }

  if (w.type=="summary") {
    VectorClock theirs = VectorClock::fromVariantMap(QVariant(w.vc).toMap());
    auto ranges = st_.seen.diffForCatchup(theirs);
    for (auto it = ranges.constBegin(); it != ranges.constEnd(); ++it) {
      sendMissing(it.key(), it.value().first, it.value().second, from, port);
    }
    return;
  }

  if (w.type=="missing") {
    QList<WireMessage> out;
    auto& bucket = st_.logByOrigin[w.origin];
    for (qint64 s = w.seq; s <= w.ackOf; ++s) if (bucket.contains(s)) out << bucket[s];
    if (!out.isEmpty()) sendBulk(out, from, port);
    return;
  }

  if (w.type=="bulk") {
    for (const auto& m : w.text.split('\n')) {
      if (m.isEmpty()) continue;
      WireMessage one = WireMessage::fromJson(m.toUtf8());
      storeAndDeliver(one);
    }
    return;
  }
}

void ChatNode::storeAndDeliver(const WireMessage& w) {
  st_.logByOrigin[w.origin][w.seq] = w;
  st_.seen.markSeen(w.origin, w.seq);
  emit receivedChat(w.origin, w.seq, w.text);
}

void ChatNode::sendAck(const QString&, qint64 seq, const QHostAddress& addr, quint16 port) {
  WireMessage a; a.type="ack"; a.origin=st_.originId; a.ackOf=seq;
  chatSock_.writeDatagram(pack(a), addr, port);
}

void ChatNode::sendSummary(const QHostAddress& addr, quint16 port) {
  WireMessage s; s.type="summary"; s.origin=st_.originId; s.vc = st_.seen.vc;
  chatSock_.writeDatagram(pack(s), addr, port);
}

void ChatNode::sendMissing(const QString& origin, qint64 fromSeq, qint64 toSeq,
                           const QHostAddress& addr, quint16 port) {
  WireMessage m; m.type="missing"; m.origin=st_.originId; m.text=origin; m.seq=fromSeq; m.ackOf=toSeq;
  chatSock_.writeDatagram(pack(m), addr, port);
}

void ChatNode::sendBulk(const QList<WireMessage>& msgs, const QHostAddress& addr, quint16 port) {
  QStringList lines; lines.reserve(msgs.size());
  for (const auto& w : msgs) lines << QString::fromUtf8(w.toJson());
  WireMessage b; b.type="bulk"; b.origin=st_.originId; b.text = lines.join('\n');
  chatSock_.writeDatagram(pack(b), addr, port);
}

void ChatNode::onResendTick() {
  const qint64 RESEND_MS = 1200; const int MAX_RETRY=6;
  for (auto it = st_.pendingAcks.begin(); it != st_.pendingAcks.end();) {
    auto& sm = it.value();
    if (sm.sinceSent.elapsed() > RESEND_MS) {
      for (auto& p : peers_.peers) chatSock_.writeDatagram(pack(sm.msg), p.first, p.second);
      sm.sinceSent.restart();
      if (++sm.resendCount > MAX_RETRY) it = st_.pendingAcks.erase(it);
      else ++it;
    } else ++it;
  }
}

void ChatNode::onAntiEntropyTick() {
  for (auto& p : peers_.peers) sendSummary(p.first, p.second);
}
