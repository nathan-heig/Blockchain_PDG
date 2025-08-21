#include <QtTest/QtTest>

class BasicTest : public QObject {
    Q_OBJECT
private slots:
    void sanity() {
        QVERIFY2(1 + 1 == 2, "Math still works");
    }
};

QTEST_APPLESS_MAIN(BasicTest)
#include "test_basic.moc"
