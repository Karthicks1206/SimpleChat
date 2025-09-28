#include "RingNode.h"
#include <QDataStream>
#include <QTimer>

RingNode::RingNode(QString id, QString host, quint16 myPort, quint16 rightPort, QObject* parent)
    : QObject(parent), id_(id), host_(host), myPort_(myPort), rightPort_(rightPort) {
    connect(&server_, &QTcpServer::newConnection, this, &RingNode::handleNewConnection);
}

void RingNode::start() {
    if (!server_.listen(QHostAddress::Any, myPort_)) {
        emit logUpdate("Failed to listen on port " + QString::number(myPort_));
    } else {
        emit logUpdate("Listening on port " + QString::number(myPort_));
    }
    connectToRight();
}

void RingNode::connectToRight() {
    if (rightSock_) {
        rightSock_->deleteLater();
    }
    rightSock_ = new QTcpSocket(this);
    connect(rightSock_, &QTcpSocket::connected, this, &RingNode::handleRightConnected);
    connect(rightSock_, &QTcpSocket::readyRead, this, &RingNode::handleReadyRead);
    connect(rightSock_, &QTcpSocket::disconnected, this, &RingNode::handleDisconnected);
    connect(rightSock_, &QTcpSocket::errorOccurred, this, &RingNode::handleRightError);
    rightSock_->connectToHost(host_, rightPort_);
}

void RingNode::handleRightConnected() {
    emit logUpdate("Connected to right neighbor on port " + QString::number(rightPort_));
}

void RingNode::handleRightError(QAbstractSocket::SocketError) {
    emit logUpdate("Error with right neighbor. Retrying...");
    QTimer::singleShot(1000, this, &RingNode::connectToRight);
}

void RingNode::handleNewConnection() {
    while (server_.hasPendingConnections()) {
        QTcpSocket* s = server_.nextPendingConnection();
        incoming_ << s;
        connect(s, &QTcpSocket::readyRead, this, &RingNode::handleReadyRead);
        connect(s, &QTcpSocket::disconnected, this, &RingNode::handleDisconnected);
        emit logUpdate("New connection accepted");
    }
}

void RingNode::handleDisconnected() {
    auto* s = qobject_cast<QTcpSocket*>(sender());
    if (!s) return;
    frames_.remove(s);
    incoming_.removeAll(s);
    if (s == rightSock_) {
        emit logUpdate("Right neighbor disconnected, will reconnect");
        rightSock_ = nullptr;
        connectToRight();
    }
    s->deleteLater();
}

void RingNode::writeJson(QTcpSocket* sock, const QJsonObject& obj) {
    if (!sock || sock->state() != QAbstractSocket::ConnectedState) return;
    QJsonDocument doc(obj);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);
    QByteArray frame;
    QDataStream out(&frame, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << static_cast<quint32>(payload.size());
    frame.append(payload);
    sock->write(frame);
    sock->flush();
}

void RingNode::handleReadyRead() {
    auto* s = qobject_cast<QTcpSocket*>(sender());
    if (!s) return;
    auto& f = frames_[s];
    QDataStream in(s);
    in.setByteOrder(QDataStream::BigEndian);

    while (true) {
        if (f.expected == 0) {
            if (s->bytesAvailable() < 4) break;
            in >> f.expected;
            f.buffer.clear();
        }
        if (s->bytesAvailable() < f.expected) break;
        f.buffer = s->read(f.expected);
        f.expected = 0;

        QJsonDocument doc = QJsonDocument::fromJson(f.buffer);
        if (doc.isObject()) processJson(doc.object());
    }
}

void RingNode::processJson(const QJsonObject& obj) {
    if (obj.value("type") != "chat") return;

    QString origin = obj["origin"].toString();
    QString dest = obj["destination"].toString();
    QString text = obj["text"].toString();
    quint32 seq = obj["seq"].toInt();

    QString key = origin + "#" + QString::number(seq);
    if (seen_.contains(key)) return;
    seen_.insert(key);

    if (dest == id_) {
        deliverOrQueue(origin, seq, text);
    } else {
        forward(obj);
    }
}

void RingNode::deliverOrQueue(const QString& origin, quint32 seq, const QString& text) {
    quint32 expected = nextSeq_.value(origin, 1u);
    if (seq == expected) {
        emit receivedMessage(origin, seq, text);
        nextSeq_[origin] = expected + 1;
        tryDeliver(origin);
    } else if (seq > expected) {
        buffers_[origin].insert(seq, text);
    }
}

void RingNode::tryDeliver(const QString& origin) {
    quint32 expected = nextSeq_.value(origin, 1u);
    auto& buf = buffers_[origin];
    while (buf.contains(expected)) {
        QString t = buf.take(expected);
        emit receivedMessage(origin, expected, t);
        expected++;
    }
    nextSeq_[origin] = expected;
}

void RingNode::forward(const QJsonObject& obj) {
    if (rightSock_) writeJson(rightSock_, obj);
}

void RingNode::sendMessage(const QString& dest, const QString& text) {
    quint32 seq = nextSeq_.value(id_, 1u);
    QJsonObject obj {
        {"type", "chat"},
        {"origin", id_},
        {"destination", dest},
        {"seq", static_cast<int>(seq)},
        {"text", text}
    };
    forward(obj);
    nextSeq_[id_] = seq + 1;
}
