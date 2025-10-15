#include <QtTest>
#include "../src/VectorClock.h"

class TestVC: public QObject { Q_OBJECT
private slots:
  void diff() {
    VectorClock a; a.markSeen("A",5); a.markSeen("B",2);
    VectorClock b; b.markSeen("A",3); b.markSeen("B",2);
    auto d = a.diffForCatchup(b);
    QVERIFY(d.contains("A"));
    QCOMPARE(d["A"].first, 4LL);
    QCOMPARE(d["A"].second, 5LL);
    QVERIFY(!d.contains("B"));
  }
};
QTEST_MAIN(TestVC)
#include "test_vectorclock.moc"
