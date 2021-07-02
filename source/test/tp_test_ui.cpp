#include <QtTest>
#include <QCoreApplication>

// add necessary includes here

class TP_test : public QObject
{
    Q_OBJECT

public:
    TP_test();
    ~TP_test();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();

};

TP_test::TP_test()
{

}

TP_test::~TP_test()
{

}

void TP_test::initTestCase()
{

}

void TP_test::cleanupTestCase()
{

}

void TP_test::test_case1()
{

}

QTEST_MAIN(TP_test)

#include "tp_test_ui.moc"
