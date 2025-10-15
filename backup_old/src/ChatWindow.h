#pragma once
#include <QMainWindow>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include "RingNode.h"

class ChatWindow : public QMainWindow {
    Q_OBJECT
public:
    ChatWindow(QString id, QString host, quint16 myPort, quint16 rightPort, QWidget* parent=nullptr);

private slots:
    void send();
    void showReceived(QString origin, quint32 seq, QString text);
    void showLog(QString msg);

private:
    QString id_;
    RingNode* node_;
    QTextBrowser* log_;
    QLineEdit* destInput_;
    QPlainTextEdit* textInput_;
    QPushButton* sendBtn_;
};
