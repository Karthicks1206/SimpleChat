#pragma once
#include <QtCore>
#include <QtNetwork>

class PeerDirectory {
public:
  QList<QPair<QHostAddress,quint16>> peers;
  bool add(const QHostAddress& a, quint16 p) {
    QPair<QHostAddress,quint16> key{a,p};
    if (!peers.contains(key)) { peers.append(key); return true; }
    return false;
  }
};
