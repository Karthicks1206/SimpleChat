#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSet>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>

class RingNode : public QObject {
    Q_OBJECT
public:
    RingNode(QString id, QString host, quint16 myPort, quint16 rightPort, QObject* parent=nullptr);

    void start();
    void sendMessage(const QString& destination, const QString& text);

signals:
    void receivedMessage(QString origin, quint32 seq, QString text);
    void logUpdate(QString msg);

private slots:
    void handleNewConnection();
    void handleReadyRead();
    void handleDisconnected();
    void handleRightConnected();
    void handleRightError(QAbstractSocket::SocketError);

private:
    struct Frame { quint32 expected = 0; QByteArray buffer; };

    void connectToRight();
    void writeJson(QTcpSocket* sock, const QJsonObject& obj);
    void processJson(const QJsonObject& obj);
    void deliverOrQueue(const QString& origin, quint32 seq, const QString& text);
    void tryDeliver(const QString& origin);
    void forward(const QJsonObject& obj);

    QString id_;
    QString host_;
    quint16 myPort_;
    quint16 rightPort_;
    QTcpServer server_;
    QTcpSocket* rightSock_ = nullptr;
    QList<QTcpSocket*> incoming_;
    QHash<QTcpSocket*, Frame> frames_;
    QSet<QString> seen_;
    QHash<QString, quint32> nextSeq_;
    QHash<QString, QMap<quint32, QString>> buffers_;
};
