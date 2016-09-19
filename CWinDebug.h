#ifndef CWINDEBUG_H
#define CWINDEBUG_H
#include <QWaitCondition>
#include <QTimer>
#include <QDebug>
#include <QString>
#include <QWidget>
#include <QLineEdit>
#include <QSettings>
#include <QStringList>
#include <QCloseEvent>
#include <QDataStream>
#include <QButtonGroup>
#include <QElapsedTimer>
#include <QTableWidgetItem>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#define DEBUG

#define ACWU 0x01
#define IRU  0x02
#define IMPU 0x03

#define ACWI1 0x11
#define ACWI2 0x12
#define ACWI3 0x13
#define ACWI4 0x14
#define ACWI5 0x15
#define ACWI6 0x16

#define IR1 0x21
#define IR2 0x22
#define IR3 0x23
#define IR4 0x24
#define IR5 0x25
#define IR6 0x26
#define IR7 0x27
#define IR8 0x28

#define CHECK 0x00

#define TOOL0 0x01
#define TOOL1 0x02
#define STOP0 0x03
#define STOP1 0x04
#define START 0x05
#define CONFT 0x06

#define RESULT0 0x10
#define RESULT1 0x11
#define RESULT2 0x12

#define LOAD 0x20
#define SAVE 0x21

#define END 0x80

#define Name 0
#define Volt 1
#define Parm 2
#define Real 3
#define KK   4
#define BB   5

namespace Ui {
class CWinDebug;
}

class CWinDebug : public QWidget
{
    Q_OBJECT

public:
    explicit CWinDebug(QWidget *parent = 0);
    ~CWinDebug();

private:
    Ui::CWinDebug *ui;

private slots:
    void WinInit(void);
    void KeyInit(void);
    void KeyJudge(int id);
    void DatInit(void);
    void DatSave(void);
    void ComInit(void);
    void ComQuit(void);
    void ComRead(void);
    void SendMsg(QByteArray msg);

    void closeEvent(QCloseEvent *e);

    void Test(void);
    bool TestWait(void);
    void TestTool(quint8 t);
    void TestStop(void);
    void TestConfig(void);
    void TestBegin(void);
    void TestGetResult(void);
    void TestCalc(void);
    void TestLoad(void);
    void TestSave(void);
private:
    QList<QTableWidgetItem*> tabItem;
    QButtonGroup *btnGroup;
    QSettings *setting;
    QSerialPort *com1;
    QSerialPort *com2;
    QSerialPort *com3;
    QSerialPort *com4;
    QByteArray result;
    QStringList VList;
    QStringList EList;
    QTimer *timer1;
    QTimer *timer2;

    quint8 step;
    quint8 single;
    double r;
};

#endif // CWINDEBUG_H
