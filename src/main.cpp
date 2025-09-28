#include <QApplication>
#include <QCommandLineParser>
#include "ChatWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("SimpleChat Ring Messenger");
    parser.addHelpOption();

    QCommandLineOption idOpt({"i","id"}, "Unique identifier", "id");
    QCommandLineOption portOpt({"p","port"}, "Listening port", "port");
    QCommandLineOption rightOpt({"r","right"}, "Right neighbor port", "port");
    QCommandLineOption hostOpt({"H","host"}, "Neighbor host (default 127.0.0.1)", "host", "127.0.0.1");

    parser.addOption(idOpt);
    parser.addOption(portOpt);
    parser.addOption(rightOpt);
    parser.addOption(hostOpt);
    parser.process(app);

    if (!parser.isSet(portOpt) || !parser.isSet(rightOpt)) {
        parser.showHelp(1);
    }

    QString id = parser.isSet(idOpt) ? parser.value(idOpt) : parser.value(portOpt);
    quint16 myPort = parser.value(portOpt).toUShort();
    quint16 rightPort = parser.value(rightOpt).toUShort();
    QString host = parser.value(hostOpt);

    ChatWindow window(id, host, myPort, rightPort);
    window.show();

    return app.exec();
}
