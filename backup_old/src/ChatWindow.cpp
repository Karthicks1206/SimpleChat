#include "ChatWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>

ChatWindow::ChatWindow(QString id, QString host, quint16 myPort, quint16 rightPort, QWidget* parent)
    : QMainWindow(parent), id_(id) {

    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    log_ = new QTextBrowser(central);
    layout->addWidget(log_);

    QHBoxLayout* destLayout = new QHBoxLayout();
    destInput_ = new QLineEdit(central);
    destInput_->setPlaceholderText("Destination ID");
    destLayout->addWidget(destInput_);
    layout->addLayout(destLayout);

    textInput_ = new QPlainTextEdit(central);
    textInput_->setPlaceholderText("Type a message...");
    layout->addWidget(textInput_);

    sendBtn_ = new QPushButton("Send", central);
    layout->addWidget(sendBtn_);

    setCentralWidget(central);

    node_ = new RingNode(id, host, myPort, rightPort, this);
    connect(node_, &RingNode::receivedMessage, this, &ChatWindow::showReceived);
    connect(node_, &RingNode::logUpdate, this, &ChatWindow::showLog);
    node_->start();

    connect(sendBtn_, &QPushButton::clicked, this, &ChatWindow::send);
}

void ChatWindow::send() {
    QString dest = destInput_->text().trimmed();
    QString text = textInput_->toPlainText().trimmed();
    if (dest.isEmpty() || text.isEmpty()) return;
    node_->sendMessage(dest, text);
    QString time = QDateTime::currentDateTime().toString("HH:mm:ss");
    log_->append("[" + time + "] You -> " + dest + ": " + text);
    textInput_->clear();
}

void ChatWindow::showReceived(QString origin, quint32 seq, QString text) {
    QString time = QDateTime::currentDateTime().toString("HH:mm:ss");
    log_->append("[" + time + "] " + origin + " (seq " + QString::number(seq) + "): " + text);
}

void ChatWindow::showLog(QString msg) {
    log_->append("[System] " + msg);
}
