#include <QtWidgets>
#include "UiWindow.h"

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  quint16 chatPort = 45454, discPort = 45455;
  if (argc >= 2) chatPort = QString::fromUtf8(argv[1]).toUShort();
  if (argc >= 3) discPort = QString::fromUtf8(argv[2]).toUShort();
  UiWindow w(chatPort, discPort);
  w.setWindowTitle(QString("SimpleChat2 : %1").arg(chatPort));
  w.resize(800, 560);
  w.show();
  return app.exec();
}
