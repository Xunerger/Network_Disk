#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QFile>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);

    loadConfig();
    // 监听
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP), m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
//        qDebug() << strData;

        strData.replace("\r\n", " ");
//        qDebug() << strData;

        QStringList strList = strData.split(" ");
//        for (int i = 0; i < strList.size(); i++)
//        {
//            qDebug() << "--->" << strList[i];
//        }
        m_strIP = strList[0];
        m_usPort = strList[1].toUShort();
        qDebug() << "IP: " << m_strIP << " usPort: " << m_usPort;

        file.close();
    }
    else
    {
        QMessageBox::critical(this, "open config", "open config failed");
    }
}

