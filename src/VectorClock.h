#pragma once
#include <QtCore>

class VectorClock {
public:
  // Use QMap so it matches WireMessage::vc (QMap<QString,qint64>)
  QMap<QString, qint64> vc;  // origin -> highest seq seen

  void markSeen(const QString& origin, qint64 seq) {
    vc[origin] = std::max(vc.value(origin, 0LL), seq);
  }

  // For each origin where *this* has more than 'other', return [other+1, this]
  QMap<QString, QPair<qint64,qint64>> diffForCatchup(const VectorClock& other) const {
    QMap<QString, QPair<qint64,qint64>> ranges;
    for (auto it = vc.constBegin(); it != vc.constEnd(); ++it) {
      qint64 mine = it.value();
      qint64 theirs = other.vc.value(it.key(), 0LL);
      if (mine > theirs) ranges[it.key()] = {theirs+1, mine};
    }
    return ranges;
  }

  QVariantMap toVariantMap() const {
    QVariantMap m; for (auto it = vc.constBegin(); it != vc.constEnd(); ++it) m[it.key()] = it.value();
    return m;
  }
  static VectorClock fromVariantMap(const QVariantMap& m) {
    VectorClock v; for (auto it = m.constBegin(); it != m.constEnd(); ++it) v.vc[it.key()] = it.value().toLongLong();
    return v;
  }
};
