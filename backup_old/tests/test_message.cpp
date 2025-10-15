#include <QtTest>
#include "../src/Message.h"

class TestMessage: public QObject { Q_OBJECT
private slots:
  void roundtrip() {
    WireMessage w; w.type="chat"; w.origin="A"; w.seq=5; w.dest=-1; w.text="hello";
    w.vc.insert("A",5); w.vc.insert("B",3);
    auto b = w.toJson();
    auto x = WireMessage::fromJson(b);
    QCOMPARE(x.type, QString("chat"));
    QCOMPARE(x.origin, QString("A"));
    QCOMPARE(x.seq, 5LL);
    QCOMPARE(x.dest, -1LL);
    QCOMPARE(x.text, QString("hello"));
    QCOMPARE(x.vc.value("B"), 3LL);
  }
};
QTEST_MAIN(TestMessage)
#include "test_message.moc"
