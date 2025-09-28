#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDataStream>
#include <QThread>
#include <iostream>

static bool sendFrame(const QString& host, quint16 port, const QJsonObject& obj, QString* err) {
    QTcpSocket sock;
    sock.connectToHost(host, port);
    if (!sock.waitForConnected(2000)) { if (err) *err = sock.errorString(); return false; }

    QJsonDocument doc(obj);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);

    QByteArray frame;
    QDataStream out(&frame, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << static_cast<quint32>(payload.size());
    frame.append(payload);

    if (sock.write(frame) != frame.size()) { if (err) *err = "short write"; return false; }
    if (!sock.waitForBytesWritten(2000))   { if (err) *err = "timeout writing"; return false; }

    sock.disconnectFromHost();
    // Only wait if still connected (prevents the warning you saw)
    if (sock.state() != QAbstractSocket::UnconnectedState) {
        sock.waitForDisconnected(500);
    }
    return true;
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("SimpleInjector");

    QCommandLineParser p;
    p.setApplicationDescription("Headless injector for SimpleChat ring (Qt6).");
    p.addHelpOption();

    QCommandLineOption hostOpt({"H","host"}, "Target host (default 127.0.0.1).", "host", "127.0.0.1");
    QCommandLineOption portOpt({"p","port"}, "Inject into this node's port.", "port");
    QCommandLineOption originOpt({"o","origin"}, "Origin ID.", "origin");
    QCommandLineOption destOpt({"d","dest","destination"}, "Destination ID.", "dest");
    QCommandLineOption seqOpt({"s","seq"}, "Sequence number.", "seq");
    QCommandLineOption textOpt({"t","text"}, "Message text.", "text");
    QCommandLineOption demoOpt("demo", "Run two demos: (1) ordering via 9001->9003 (seq2 then seq1); (2) propagation 9002->9004.");

    p.addOption(hostOpt); p.addOption(portOpt); p.addOption(originOpt);
    p.addOption(destOpt); p.addOption(seqOpt); p.addOption(textOpt); p.addOption(demoOpt);
    p.process(app);

    if (p.isSet(demoOpt)) {
        QString err;
        // Demo 1: ordering (inject to 9001, dest 9003): send seq=2 then seq=1
        if (!sendFrame("127.0.0.1", 9001, QJsonObject{
            {"type","chat"}, {"origin","DEMO9001"}, {"destination","9003"}, {"seq",2}, {"text","(seq 2) should display SECOND"}
        }, &err)) { std::cerr << "Demo1 A failed: " << err.toStdString() << "\n"; return 1; }
        QThread::msleep(120);
        if (!sendFrame("127.0.0.1", 9001, QJsonObject{
            {"type","chat"}, {"origin","DEMO9001"}, {"destination","9003"}, {"seq",1}, {"text","(seq 1) should display FIRST"}
        }, &err)) { std::cerr << "Demo1 B failed: " << err.toStdString() << "\n"; return 1; }

        // Demo 2: propagation (inject to 9002, dest 9004)
        if (!sendFrame("127.0.0.1", 9002, QJsonObject{
            {"type","chat"}, {"origin","DEMO9002"}, {"destination","9004"}, {"seq",1}, {"text","Propagation check 9002→9004"}
        }, &err)) { std::cerr << "Demo2 failed: " << err.toStdString() << "\n"; return 1; }

        return 0;
    }

    // Single-shot mode
    if (!p.isSet(portOpt) || !p.isSet(originOpt) || !p.isSet(destOpt) || !p.isSet(seqOpt) || !p.isSet(textOpt)) {
        p.showHelp(1);
    }
    bool ok=false; quint16 port = p.value(portOpt).toUShort(&ok); if (!ok||!port) { std::cerr<<"Invalid port\n"; return 2; }
    bool okSeq=false; int seq = p.value(seqOpt).toInt(&okSeq);    if (!okSeq || seq<=0)  { std::cerr<<"Invalid seq\n";  return 2; }

    QString err;
    bool okSend = sendFrame(p.value(hostOpt), port, QJsonObject{
        {"type","chat"},
        {"origin", p.value(originOpt)},
        {"destination", p.value(destOpt)},
        {"seq", seq},
        {"text", p.value(textOpt)}
    }, &err);
    if (!okSend) { std::cerr << "Send failed: " << err.toStdString() << "\n"; return 3; }
    return 0;
}
