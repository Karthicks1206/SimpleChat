#include "UiWindow.h"

UiWindow::UiWindow(quint16 chatPort, quint16 discPort, QWidget* parent)
  : QMainWindow(parent), node_(chatPort, discPort, this) {
  auto *central = new QWidget; auto *v = new QVBoxLayout(central);
  log_ = new QTextEdit; log_->setReadOnly(true);
  input_ = new QLineEdit; input_->setPlaceholderText("Type message...");
  dest_ = new QLineEdit; dest_->setPlaceholderText("dest port or -1 for broadcast"); dest_->setText("-1");
  auto *btn = new QPushButton("Send");
  peersView_ = new QListWidget;

  auto *hl = new QHBoxLayout; hl->addWidget(input_, 1); hl->addWidget(new QLabel("Dest:")); hl->addWidget(dest_); hl->addWidget(btn);
  v->addWidget(new QLabel("Log:")); v->addWidget(log_, 1);
  v->addLayout(hl);
  v->addWidget(new QLabel("Peers:"));
  v->addWidget(peersView_);
  setCentralWidget(central);

  connect(btn, &QPushButton::clicked, this, &UiWindow::onSend);
  connect(&node_, &ChatNode::receivedChat, this, &UiWindow::onIncoming);
  connect(&node_, &ChatNode::peerListChanged, this, &UiWindow::onPeersChanged);
}

void UiWindow::onSend() {
  bool ok=false; qint64 d = dest_->text().toLongLong(&ok); if (!ok) d = -1;
  if (!input_->text().isEmpty()) node_.sendChat(input_->text(), d);
  input_->clear();
}
void UiWindow::onIncoming(const QString& origin, qint64 seq, const QString& text) {
  log_->append(QString("[%1 #%2] %3").arg(origin, QString::number(seq), text));
}
void UiWindow::onPeersChanged(const QList<QPair<QHostAddress,quint16>>& ps) {
  peersView_->clear();
  for (auto& p : ps) peersView_->addItem(QString("%1:%2").arg(p.first.toString()).arg(p.second));
}
