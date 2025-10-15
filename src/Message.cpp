#include "Message.h"

QVariantMap WireMessage::toVariant() const {
  QVariantMap m;
  m["type"]=type; m["origin"]=origin; m["seq"]=seq; m["dest"]=dest;
  m["chatPort"]=chatPort;
  m["text"]=text; m["ackOf"]=ackOf;
  QVariantMap vcm;
  for (auto it = vc.constBegin(); it != vc.constEnd(); ++it) vcm[it.key()] = it.value();
  m["vc"]=vcm;
  return m;
}

WireMessage WireMessage::fromVariant(const QVariantMap& m) {
  WireMessage w;
  w.type   = m.value("type").toString();
  w.origin = m.value("origin").toString();
  w.seq    = m.value("seq").toLongLong();
  w.dest   = m.value("dest").toLongLong();
  w.chatPort = m.value("chatPort", -1).toLongLong();
  w.text   = m.value("text").toString();
  w.ackOf  = m.value("ackOf").toLongLong();
  QVariantMap vcm = m.value("vc").toMap();
  for (auto it = vcm.constBegin(); it != vcm.constEnd(); ++it) w.vc[it.key()] = it.value().toLongLong();
  return w;
}

QByteArray WireMessage::toJson() const {
  QJsonObject obj = QJsonObject::fromVariantMap(toVariant());
  QJsonDocument doc(obj);
  return doc.toJson(QJsonDocument::Compact);
}

WireMessage WireMessage::fromJson(const QByteArray& bytes) {
  auto doc = QJsonDocument::fromJson(bytes);
  return fromVariant(doc.object().toVariantMap());
}
