#pragma once
#include <QtCore>
#include "Message.h"
#include "VectorClock.h"

struct StoredMsg {
  WireMessage msg;
  QElapsedTimer sinceSent;
  int resendCount = 0;
};

class ProtocolState {
public:
  QString originId;                      // UUID for this instance
  qint64 nextSeq = 1;                    // per-origin sequence counter
  QMap<qint64, StoredMsg> pendingAcks;   // my msgs awaiting ACK
  QMap<QString, QMap<qint64, WireMessage>> logByOrigin; // origin -> seq -> msg
  VectorClock seen;                      // highest seq per origin
};
