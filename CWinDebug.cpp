/*******************************************************************************
 * Copyright (c) 2016,青岛艾普智能仪器有限公司
 * All rights reserved.
 *
 * version:     1.0
 * author:      link
 * date:        2016.08.23
 * brief:       手主界面
*******************************************************************************/
#include "CWinDebug.h"
#include "ui_CWinDebug.h"
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      构造函数
******************************************************************************/
CWinDebug::CWinDebug(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWinDebug)
{
    ui->setupUi(this);

    WinInit();
    KeyInit();
    DatInit();
    timer1 = new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(ComRead()));
    timer2 = new QTimer(this);
    connect(timer2,SIGNAL(timeout()),this,SLOT(Test()));
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      析构函数
******************************************************************************/
CWinDebug::~CWinDebug()
{
    delete ui;
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      界面初始化
******************************************************************************/
void CWinDebug::WinInit()
{
    QStringList com;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        com << info.portName();
    }
    ui->Box1->addItems(com);
    ui->Box2->addItems(com);
    ui->Box3->addItems(com);
    ui->Box4->addItems(com);

    QFile file(":/css/aip001.css");
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    this->setStyleSheet(qss);

    ui->TabVolt->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->TabVolt->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->TabVolt->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    ui->TabVolt->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    ui->TabVolt->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Stretch);
    ui->TabVolt->horizontalHeader()->setSectionResizeMode(5,QHeaderView::Stretch);
    ui->TabVolt->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      按键初始化
******************************************************************************/
void CWinDebug::KeyInit()
{
    btnGroup = new QButtonGroup;
    btnGroup->addButton(ui->KeyOpen,  Qt::Key_A);
    btnGroup->addButton(ui->KeyStart, Qt::Key_B);
    btnGroup->addButton(ui->KeyCalc,  Qt::Key_C);
    btnGroup->addButton(ui->KeyLoad,  Qt::Key_D);
    btnGroup->addButton(ui->KeyClear, Qt::Key_E);
    connect(btnGroup,SIGNAL(buttonClicked(int)),this,SLOT(KeyJudge(int)));
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      按键功能
******************************************************************************/
void CWinDebug::KeyJudge(int id)
{
    switch (id) {
    case Qt::Key_A:
        if (ui->KeyOpen->text() == "打开串口")
            ComInit();
        else
            ComQuit();
        break;
    case Qt::Key_B:
        if (ui->KeyStart->text() == "开始测试")
            TestInit();
        else
            TestQuit();
        break;
    case Qt::Key_C:
        TestCalc();
        break;
    case Qt::Key_D:
        TestLoad();
        TestDelay(100);
        TestSave();
        break;
    case Qt::Key_E:
        TestClear();
        TestLoad();
        TestDelay(100);
        TestSave();
        break;
    default:
        break;
    }
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      数据显示
******************************************************************************/
void CWinDebug::DatInit()
{
    com1 = NULL;
    com2 = NULL;
    com3 = NULL;
    com4 = NULL;

    setting = new QSettings("default.ini",QSettings::IniFormat);
    setting->setIniCodec("GB2312");

    ui->Box1->setCurrentIndex(setting->value("/Default/COM1").toInt());
    ui->Box2->setCurrentIndex(setting->value("/Default/COM2").toInt());
    ui->Box3->setCurrentIndex(setting->value("/Default/COM3").toInt());
    ui->Box4->setCurrentIndex(setting->value("/Default/COM4").toInt());
    ui->Box5->setCurrentIndex(setting->value("/Default/BOX5").toInt());
    ui->Edit1->setText(setting->value("/Default/EDIT1").toString());
    ui->Edit2->setText(setting->value("/Default/EDIT2").toString());

    QStringList p = (setting->value("/TEST/TESTN").toString()).split(" ");
    QStringList q = (setting->value("/TEST/TESTP").toString()).split(" ");
    QStringList v = (setting->value("/TEST/TESTV").toString()).split(" ");

    ui->TabVolt->setRowCount(p.size());

    for (int i=0; i<ui->TabVolt->rowCount(); i++) {
        for (int j=0; j<ui->TabVolt->columnCount(); j++) {
            tabItem.append(new QTableWidgetItem);
            tabItem.last()->setTextAlignment(Qt::AlignCenter);
            ui->TabVolt->setItem(i,j,tabItem.last());
        }
        if (i<p.size())
            ui->TabVolt->item(i,Name)->setText(p.at(i));
        if (i<v.size())
            ui->TabVolt->item(i,Volt)->setText(v.at(i));
        if (i<q.size())
            ui->TabVolt->item(i,Parm)->setText(q.at(i));
    }
    TestClear();
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      数据保存
******************************************************************************/
void CWinDebug::DatSave()
{
    setting->setValue("/Default/COM1",ui->Box1->currentIndex());
    setting->setValue("/Default/COM2",ui->Box2->currentIndex());
    setting->setValue("/Default/COM3",ui->Box3->currentIndex());
    setting->setValue("/Default/COM4",ui->Box4->currentIndex());
    setting->setValue("/Default/BOX5",ui->Box5->currentIndex());
    setting->setValue("/Default/EDIT1",ui->Edit1->text());
    setting->setValue("/Default/EDIT2",ui->Edit2->text());
    QStringList temp;
    for (int i=0; i<ui->TabVolt->rowCount(); i++)
        temp.append(ui->TabVolt->item(i,Volt)->text());
    setting->setValue("/TEST/TESTV",temp.join(" "));
    temp.clear();
    for (int i=0; i<ui->TabVolt->rowCount(); i++)
        temp.append(ui->TabVolt->item(i,Parm)->text());
    setting->setValue("/TEST/TESTP",temp.join(" "));
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      串口初始化
******************************************************************************/
void CWinDebug::ComInit()
{
    com1 = new QSerialPort(ui->Box1->currentText(),this);
    com2 = new QSerialPort(ui->Box2->currentText(),this);
    com3 = new QSerialPort(ui->Box3->currentText(),this);
    com4 = new QSerialPort(ui->Box4->currentText(),this);
    if (!com1->open(QIODevice::ReadWrite)) {
        SendMsg(QString("串口%1打开失败\n").arg(ui->Box1->currentText()).toUtf8());
        ui->KeyOpen->setText("打开串口");
        return;
    }
    com1->setBaudRate(9600);    //波特率
    com1->setParity(QSerialPort::NoParity);
    com1->setDataBits(QSerialPort::Data8);
    com1->setStopBits(QSerialPort::OneStop);
    com1->setFlowControl(QSerialPort::NoFlowControl);

    if (!com2->open(QIODevice::ReadWrite)) {
        SendMsg(QString("串口%1打开失败\n").arg(ui->Box2->currentText()).toUtf8());
        ui->KeyOpen->setText("打开串口");
        return;
    }
    com2->setBaudRate(19200);    //波特率
    com2->setParity(QSerialPort::NoParity);
    com2->setDataBits(QSerialPort::Data8);
    com2->setStopBits(QSerialPort::OneStop);
    com2->setFlowControl(QSerialPort::NoFlowControl);

    if (!com3->open(QIODevice::ReadWrite)) {
        SendMsg(QString("串口%1打开失败\n").arg(ui->Box3->currentText()).toUtf8());
        ui->KeyOpen->setText("打开串口");
        return;
    }
    com3->setBaudRate(19200);    //波特率
    com3->setParity(QSerialPort::OddParity);
    com3->setDataBits(QSerialPort::Data7);
    com3->setStopBits(QSerialPort::OneStop);
    com3->setFlowControl(QSerialPort::NoFlowControl);
    com3->setDataTerminalReady(true);
    com3->setRequestToSend(false);

    if (!com4->open(QIODevice::ReadWrite)) {
        SendMsg(QString("串口%1打开失败\n").arg(ui->Box4->currentText()).toUtf8());
        ui->KeyOpen->setText("打开串口");
        return;
    }
    com4->setBaudRate(19200);    //波特率
    com4->setParity(QSerialPort::OddParity);
    com4->setDataBits(QSerialPort::Data7);
    com4->setStopBits(QSerialPort::OneStop);
    com4->setFlowControl(QSerialPort::NoFlowControl);
    com4->setDataTerminalReady(true);
    com4->setRequestToSend(false);
    ui->KeyOpen->setText("关闭串口");
    timer1->start(100);
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      串口退出
******************************************************************************/
void CWinDebug::ComQuit()
{
    TestQuit();
    timer1->stop();
    if (com1 != NULL)
        com1->close();
    if (com2 != NULL)
        com2->close();
    if (com3 != NULL)
        com4->close();
    if (com4 != NULL)
        com4->close();
    ui->KeyOpen->setText("打开串口");
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      读取串口数据
******************************************************************************/
void CWinDebug::ComRead()
{
    if (com3 != NULL && com3->bytesAvailable()>=14) {
        QByteArray msg = com3->readAll();
        QString v = msg.mid(1,5);
        if (msg.at(6) == ';') {
            v.insert(msg.at(0)-0x30+1,".");
            if (msg.at(7) == '4')
                v.insert(0,"-");
            ui->NumVolt->display(v.toDouble());
            if (msg.at(10) == ':') {
                ui->LabUnit1->setText("DC");
            }
            if (msg.at(10) == '6') {
                ui->LabUnit1->setText("AC");
            }
        }
    }
    if (com4 != NULL && com4->bytesAvailable()>=14) {
        QByteArray msg = com4->readAll();
        QString v = msg.mid(1,5);
        if (msg.at(6) == '=') {
            ui->LabGear2->setText("uA");
            v.insert(msg.at(0)-0x30,".");
        }
        if (msg.at(6) == '?') {
            ui->LabGear2->setText("mA");
            v.insert(msg.at(0)-0x30+2,".");
        }
        if (msg.at(6) == '0') {
            ui->LabGear2->setText("A");
            v.insert(msg.at(0)-0x30+2,".");
        }
        if (msg.at(7) == '4')
            v.insert(0,"-");
        ui->NumElec->display(v.toDouble());

        if (msg.at(10) == '6') {
            ui->LabUnit2->setText("AC");
        }
        if (msg.at(10) == ':') {
            ui->LabUnit2->setText("DC");
        }
    }
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      测试信息
******************************************************************************/
void CWinDebug::SendMsg(QByteArray msg)
{
    ui->textBrowser->clear();
    ui->textBrowser->insertPlainText(msg);
    ui->textBrowser->moveCursor(QTextCursor::End);
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      退出保存
******************************************************************************/
void CWinDebug::closeEvent(QCloseEvent *e)
{
    DatSave();
    ComQuit();
    e->accept();
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      测试流程
******************************************************************************/
void CWinDebug::Test()
{
    switch (single) {
    case CHECK:
        if (TestWait())
            single = TOOL0;
        break;
    case TOOL0:
        TestTool();
        single = STOP0;
        break;
    case STOP0:
        TestStop();
        single = CONFG;
        break;
    case CONFG:
        TestConfig();
        single = START;
        break;
    case START:
        TestStart();
        single = TOOL1;
        timer2->start(1500);
        break;
    case TOOL1:
        TestTool();
        single = RSLT0;
        timer2->start(3000);
        break;
    case RSLT0:
        if (item.contains("电压")) {
            ui->TabVolt->item(step,Real)->setText(QString::number(ui->NumVolt->value()*ui->Edit1->text().toDouble()));
            single = STOP1;
            timer2->start(100);
        } else if (item.contains("耐压电流")) {
            ui->TabVolt->item(step,Real)->setText(QString::number(ui->NumElec->value()));
            single = RSLT1;
            timer2->start(2000);
        } else if (item.contains("绝缘电阻")) {
            single = RSLT1;
            timer2->start(2000);
        }
        break;
    case RSLT1:
        TestResult();
        single = RSLT2;
        timer2->start(100);
        break;
    case RSLT2:
        result = com1->readAll();
        r = quint8(result.at(13))+quint8(result.at(14))*256+quint8(result.at(15))*256*256;
        if (item.contains("耐压电流"))
            ui->TabVolt->item(step,Parm)->setText(QString::number(r/1000));
        if (item.contains("绝缘电阻"))
            ui->TabVolt->item(step,Real)->setText(QString::number(r/100));
        single = LOAD;
        break;
    case STOP1:
        TestStop();
        single = LOAD;
        break;
    case LOAD:
        if (ui->Box5->currentText() == "单步测试") {
            TestQuit();
            break;
        }
        TestCalc();
        TestLoad();
        single = SAVE;
        break;
    case SAVE:
        TestSave();
        single = OVER;
        break;
    case OVER:
        single = 0;
        step++;
        if (step == ui->TabVolt->rowCount())
            TestQuit();
        break;
    default:
        break;
    }
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      等待切换万用表
******************************************************************************/
bool CWinDebug::TestWait()
{
    qDebug()<<"TestWait";
    ui->textBrowser->clear();
    if (ui->Box5->currentText() == "单步测试")
        step = ui->TabVolt->currentRow();

    item = ui->TabVolt->item(step,Name)->text();
    if (item.contains("耐压电压") && ui->LabUnit1->text() != "AC") {
        SendMsg("请将电压表打到AC档...\n");
        return false;
    } else if ((item.contains("绝缘电压") || item.contains("匝间电压")) && ui->LabUnit1->text() != "DC") {
        SendMsg("请将电压表打到DC档...\n");
        return false;
    } else if (item.contains("耐压电流") && ui->LabUnit2->text() != "AC") {
        SendMsg("请将电流表打到AC档...\n");
        return false;
    } else if ((item.contains("耐压电流1") || item.contains("耐压电流2") ||item.contains("耐压电流3")) && ui->LabGear2->text() != "uA") {
        SendMsg("请将电流表打到uA档...\n");
        return false;
    } else if ((item.contains("耐压电流4") || item.contains("耐压电流5") ||item.contains("耐压电流6")) && ui->LabGear2->text() != "mA") {
        SendMsg("请将电流表打到mA档...\n");
        return false;
    }
    return true;
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      工装切换
******************************************************************************/
void CWinDebug::TestTool()
{
    if (item.contains("耐压电压") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000200000000027D"));
    else if (item.contains("绝缘电压") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000200000000027D"));
    else if (item.contains("匝间电压") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000600000000067D"));
    else if (item.contains("耐压电流1") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000000020000027D"));
    else if (item.contains("耐压电流2") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000002000000027D"));
    else if (item.contains("耐压电流3") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000002200000227D"));
    else if (item.contains("耐压电流3") && single==TOOL1)
        com2->write(QByteArray::fromHex("7B0C0010000002000000027D"));
    else if (item.contains("耐压电流4") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000000010000017D"));
    else if (item.contains("耐压电流5") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000020010000217D"));
    else if (item.contains("耐压电流5") && single==TOOL1)
        com2->write(QByteArray::fromHex("7B0C0010000000010000017D"));
    else if (item.contains("耐压电流6") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000020010000217D"));
    else if (item.contains("绝缘电阻1") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000000100000107D"));
    else if (item.contains("绝缘电阻2") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000000100000107D"));
    else if (item.contains("绝缘电阻2") && single==TOOL1)
        com2->write(QByteArray::fromHex("7B0C0010000001000000017D"));
    else if (item.contains("绝缘电阻3") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000001000000017D"));
    else if (item.contains("绝缘电阻4") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000001000000017D"));
    else if (item.contains("绝缘电阻4") && single==TOOL1)
        com2->write(QByteArray::fromHex("7B0C0010000000080000087D"));
    else if (item.contains("绝缘电阻5") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000000080000087D"));
    else if (item.contains("绝缘电阻6") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000000080000087D"));
    else if (item.contains("绝缘电阻6") && single==TOOL1)
        com2->write(QByteArray::fromHex("7B0C0010000000800000807D"));
    else if (item.contains("绝缘电阻7") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000080000000807D"));
    else if (item.contains("绝缘电阻7") && single==TOOL1)
        com2->write(QByteArray::fromHex("7B0C0010000008000000087D"));
    else if (item.contains("绝缘电阻8") && single==TOOL0)
        com2->write(QByteArray::fromHex("7B0C0010000080000000807D"));
    else
        return ;
    SendMsg("工装切换中...\n");
    qDebug()<<"TestTool";
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      开始测试
******************************************************************************/
void CWinDebug::TestStart()
{
    if (item.contains("耐压"))
        com1->write(QByteArray::fromHex("7B070001030B7D"));
    else if (item.contains("绝缘"))
        com1->write(QByteArray::fromHex("7B070001070F7D"));
    else if (item.contains("匝间"))
        com1->write(QByteArray::fromHex("7B08001318083B7D"));
    SendMsg("测试启动，等待...\n");
    qDebug()<<"TestStart";
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      停止测试
******************************************************************************/
void CWinDebug::TestStop()
{
    QString temp = ui->TabVolt->item(step,Name)->text();
    if (temp.contains("匝间"))
        com1->write(QByteArray::fromHex("7B08001338085B7D"));
    else
        com1->write(QByteArray::fromHex("7B060002087D"));
    SendMsg("测试停止...\n");
    qDebug()<<"TestStop";
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      参数配置
******************************************************************************/
void CWinDebug::TestConfig()
{
    quint8 crc;
    QByteArray cmd;
    QDataStream out(&cmd,QIODevice::ReadWrite);
    quint16 v = ui->TabVolt->item(step,1)->text().toInt();
    if (item.contains("耐压"))
        cmd = QByteArray::fromHex("7B1B0010000003D000F401C409000032000000000000000000F27D");
    else if (item.contains("绝缘"))
        cmd = QByteArray::fromHex("7B1B0010000007D200F40100000000320000000000000000002B7D");
    else if (item.contains("匝间"))
        cmd = QByteArray::fromHex("7B1B00100000081500F40102000A000A000A000A0000000000677D");
    out.device()->seek(9);
    out<<quint8(v%256)<<quint8(v/256);
    out.device()->seek(cmd.size()-2);
    crc = 0;
    for (int i=1; i<cmd.size()-2; i++)
        crc += cmd.at(i);
    out<<quint8(crc);
    com1->write(cmd);
    SendMsg("配置参数...\n");
    qDebug()<<"TestConfig:"<<cmd.toHex().toUpper();
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      获取测试结果
******************************************************************************/
void CWinDebug::TestResult()
{
    com1->readAll();
    if (item.contains("耐压"))
        com1->write(QByteArray::fromHex("7B070004030E7D"));
    if (item.contains("绝缘"))
        com1->write(QByteArray::fromHex("7B07000407127D"));
    SendMsg("获取测试结果...\n");
    qDebug()<<"TestGetResult";
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      计算调试参数
******************************************************************************/
void CWinDebug::TestCalc()
{
    for (int i=0; i<ui->TabVolt->rowCount()/2; i++) {
        if (ui->TabVolt->item(i*2+0,Parm)->text().isEmpty())
            continue;
        if (ui->TabVolt->item(i*2+0,Real)->text().isEmpty())
            continue;
        if (ui->TabVolt->item(i*2+1,Parm)->text().isEmpty())
            continue;
        if (ui->TabVolt->item(i*2+1,Real)->text().isEmpty())
            continue;
        double s0 = ui->TabVolt->item(i*2+0,Parm)->text().toDouble();
        double r0 = ui->TabVolt->item(i*2+0,Real)->text().toDouble();
        double s1 = ui->TabVolt->item(i*2+1,Parm)->text().toDouble();
        double r1 = ui->TabVolt->item(i*2+1,Real)->text().toDouble();
        QString temp = ui->TabVolt->item(i*2,Name)->text();
        int k = 0;
        int b = 0;
        if (temp.contains("耐压电压") || temp.contains("绝缘电压")) {
            k = ((r1-r0)*1024/(s1-s0));
            b = ((r1-r0)*s0/(s1-s0)+1024-r0);
        } else if (temp.contains("匝间电压")) {
            r0 *= ui->Edit2->text().toDouble();
            r1 *= ui->Edit2->text().toDouble();
            k = ((s1-s0)*1024/(r1-r0));
            b = ((s1-s0)*r0/(r1-r0)+1024-s0);
        }else if (temp.contains("耐压电流")) {
            r0 *= 100;
            r1 *= 100;
            s0 *= 100;
            s1 *= 100;
            k = ((r1-r0)*1024/(s1-s0));
            b = ((r1-r0)*s0/(s1-s0)+1024-r0);
        } else if (temp.contains("绝缘电阻1") || temp.contains("绝缘电阻3") || temp.contains("绝缘电阻5")) {
            r0 *= 100;
            r1 *= 100;
            s0 *= 100;
            s1 *= 100;
            k = ((r1-r0)*s0*s1/(s1-s0))*1024/(r0*r1);
            b = k*1000000/s0/1024+1024-1000000/r0;
        }else if (temp.contains("绝缘电阻7")) {
            r0 *= 100;
            r1 *= 100;
            s0 *= 100;
            s1 *= 100;
            k = ((r1-r0)*s0*s1/(s1-s0))*1024/(r0*r1);
            b = k*500000/s0/1024+1024-500000/r0;
        }
        if (abs(s0-r0)/s0 > 0.1)
            ui->TabVolt->item(i*2+0,Real)->setTextColor(QColor(Qt::red));
        else
            ui->TabVolt->item(i*2+0,Real)->setTextColor(QColor(Qt::white));
        if (abs(s1-r1)/s1 > 0.1)
            ui->TabVolt->item(i*2+1,Real)->setTextColor(QColor(Qt::red));
        else
            ui->TabVolt->item(i*2+1,Real)->setTextColor(QColor(Qt::white));

        ui->TabVolt->item(i*2,KK)->setText(QString::number(k));
        ui->TabVolt->item(i*2,BB)->setText(QString::number(b));
    }
    SendMsg("计算调试参数...\n");
    qDebug()<<"TestCalc";
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      下发调试参数
******************************************************************************/
void CWinDebug::TestLoad()
{
    if (ui->KeyOpen->text() == "打开串口") {
        SendMsg("请打开串口\n");
        return;
    }
    quint8 crc;
    QByteArray cmd;
    QDataStream out(&cmd,QIODevice::ReadWrite);

    cmd = QByteArray::fromHex("7b6600A1000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400040004000400000000000000000000000000005C13a67d");
    out.device()->seek(1);
    out<<quint8(cmd.size());
    for (int i=0; i<ui->TabVolt->rowCount()/2; i++) {
        QString temp = ui->TabVolt->item(i*2,Name)->text();
        int k = ui->TabVolt->item(i*2,KK)->text().toInt();
        int b = ui->TabVolt->item(i*2,BB)->text().toInt();
        if (temp.contains("耐压电压1")) {
            out.device()->seek(4);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(8);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("耐压电压3")) {
            out.device()->seek(6);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(10);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("绝缘电压1")) {
            out.device()->seek(12);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(18);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("耐压电流1")) {
            out.device()->seek(28);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(34);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("耐压电流3")) {
            out.device()->seek(26);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(32);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("耐压电流5")) {
            out.device()->seek(24);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(30);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("绝缘电阻1")) {
            out.device()->seek(48);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(56);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("绝缘电阻3")) {
            out.device()->seek(50);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(58);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("绝缘电阻5")) {
            out.device()->seek(52);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(60);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("绝缘电阻7")) {
            out.device()->seek(54);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(62);
            out<<quint8(b%256)<<quint8(b/256);
        } else if (temp.contains("匝间电压1")) {
            out.device()->seek(80);
            out<<quint8(k%256)<<quint8(k/256);
            out.device()->seek(82);
            out<<quint8(b%256)<<quint8(b/256);
        }
    }
    out.device()->seek(cmd.size()-2);
    crc = 0;
    for (int i=1; i<cmd.size()-2; i++)
        crc += cmd.at(i);
    out<<quint8(crc);
    com1->write(cmd);
    SendMsg("下发调试参数...\n");
    qDebug()<<"TestLoad:"<<cmd.toHex().toUpper();
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      保存调试参数
******************************************************************************/
void CWinDebug::TestSave()
{

    if (ui->KeyOpen->text() == "打开串口")
        return;
    com1->write(QByteArray::fromHex("7B0700A300AA7D"));
    SendMsg("保存调试参数...\n");
    qDebug()<<"TestSave";
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      测试开始
******************************************************************************/
void CWinDebug::TestInit()
{
    if (ui->KeyOpen->text() != "关闭串口") {
        SendMsg("请打开串口");
        return;
    }
    step = 0;
    single = 0;
    timer2->start(100);
    SendMsg("测试开始\n");
    ui->KeyStart->setText("中断测试");
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      测试退出
******************************************************************************/
void CWinDebug::TestQuit()
{
    step = 0;
    single = 0;
    timer2->stop();
    SendMsg("测试结束\n");
    ui->KeyStart->setText("开始测试");
}

void CWinDebug::TestClear()
{
    for (int i=0; i<ui->TabVolt->rowCount()/2; i++) {
        ui->TabVolt->setSpan(i*2,KK,2,1);
        ui->TabVolt->setSpan(i*2,BB,2,1);
        ui->TabVolt->item(i*2,KK)->setText("1024");
        ui->TabVolt->item(i*2,BB)->setText("1024");
    }
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.23
  * brief:      延时
******************************************************************************/
void CWinDebug::TestDelay(int ms)
{
    QTime t;
    t.start();
    while(t.elapsed()<ms)
        QCoreApplication::processEvents();
}

