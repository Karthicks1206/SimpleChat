#pragma once
#include <QtCore>

struct WireMessage {
  QString type;        // "chat","ack","hello","hello_ack","summary","missing","bulk"
  QString origin;      // unique UUID per instance
  qint64  seq = 0;     // per-origin sequence
  qint64  dest = -1;   // -1 broadcast; else targeted port/peer-id
  qint64  chatPort = -1; // <-- NEW: sender's chat port for discovery
  QString text;        // chat text or packed lines for bulk
  qint64  ackOf = -1;  // sequence acknowledged (for "ack")
  QMap<QString, qint64> vc; // vector clock summary: origin -> highest seq seen

  QVariantMap toVariant() const;
  static WireMessage fromVariant(const QVariantMap&);
  QByteArray toJson() const;
  static WireMessage fromJson(const QByteArray&);
};
