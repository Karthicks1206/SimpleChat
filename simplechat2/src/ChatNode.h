#pragma once
#include <QtCore>
#include <QtNetwork>
#include "Protocol.h"
#include "PeerDirectory.h"

class ChatNode : public QObject {
  Q_OBJECT
public:
  explicit ChatNode(quint16 chatPort, quint16 discoveryPort, QObject* parent=nullptr);
  void sendChat(const QString& text, qint64 dest = -1);
  void addPeer(const QHostAddress& addr, quint16 port);

signals:
  void receivedChat(const QString& fromOrigin, qint64 seq, const QString& text);
  void peerListChanged(const QList<QPair<QHostAddress,quint16>>& peers);

private slots:
  void onReadyReadChat();
  void onReadyReadDiscovery();
  void onResendTick();
  void onAntiEntropyTick();

private:
  void handleMessage(const WireMessage& w, const QHostAddress& from, quint16 port);
  void sendAck(const QString& toOrigin, qint64 seq, const QHostAddress& addr, quint16 port);
  void sendSummary(const QHostAddress& addr, quint16 port);
  void sendMissing(const QString& origin, qint64 fromSeq, qint64 toSeq, const QHostAddress& addr, quint16 port);
  void sendBulk(const QList<WireMessage>& msgs, const QHostAddress& addr, quint16 port);
  void broadcastHello();
  void handleHello(const QHostAddress& from, quint16 port);
  void handleHelloAck(const QHostAddress& from, quint16 port);
  void storeAndDeliver(const WireMessage& w);

  QUdpSocket chatSock_;
  QUdpSocket discSock_;
  quint16 chatPort_;
  quint16 discoveryPort_;
  PeerDirectory peers_;
  QTimer resendTimer_;
  QTimer antiEntropyTimer_;
  ProtocolState st_;
};
