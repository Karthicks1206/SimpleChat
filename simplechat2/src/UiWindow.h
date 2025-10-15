#pragma once
#include <QtWidgets>
#include "ChatNode.h"

class UiWindow : public QMainWindow {
  Q_OBJECT
public:
  UiWindow(quint16 chatPort, quint16 discPort, QWidget* parent=nullptr);

private slots:
  void onSend();
  void onIncoming(const QString& origin, qint64 seq, const QString& text);
  void onPeersChanged(const QList<QPair<QHostAddress,quint16>>& ps);

private:
  ChatNode node_;
  QTextEdit* log_;
  QLineEdit* input_;
  QLineEdit* dest_;
  QListWidget* peersView_;
};
