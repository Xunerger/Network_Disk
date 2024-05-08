#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);

    resize(500, 250);

    loadConfig();

    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(showConnect()));
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(recvMsg()));

    // 连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
//        qDebug() << strData;

        strData.replace("\r\n", " ");
        qDebug() << strData;

        QStringList strList = strData.split(" ");
//        for (int i = 0; i < strList.size(); i++)
//        {
//            qDebug() << "--->" << strList[i];
//        }
        m_strIP = strList[0];
        m_usPort = strList[1].toUShort();
//        qDebug() << "IP: " << m_strIP << " usPort: " << m_usPort;

        file.close();
    }
    else
    {
        QMessageBox::critical(this, "open config", "open config failed");
    }
}

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::loginName()
{
    return m_strLoginName;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this, "Connect Server", "Successfully connected to the server.");
}

void TcpClient::recvMsg()
{
    qDebug() << "Entry recvMsg function";
    qDebug() << m_tcpSocket.bytesAvailable();
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    qDebug() << pdu->uiMsgType << (char*)(pdu->caData);
    switch (pdu->uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_RESPOND:
    {
        if (0 == strcmp(pdu->caData, REGIST_OK))
        {
            QMessageBox::information(this, "Regist", REGIST_OK);
        }
        else
        {
            QMessageBox::warning(this, "Regist", REGIST_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:
    {
//        qDebug() << "the value of pdu->caData is " << (char*)pdu->caData;
        if (0 == strcmp(pdu->caData, LOGIN_OK))
        {
            QMessageBox::information(this, "login", LOGIN_OK);
            OpeWidget::getInstance().show();
            this->hide();
        }
        else
        {
            QMessageBox::warning(this, "Login", LOGIN_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
    {
        OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);
        break;
    }

    case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:
    {
        if (0 == strcmp(SEARCH_USR_OFFLINE, pdu->caData))
        {
            QMessageBox::information(this, "Search", QString("%1: offline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }
        else if (0 == strcmp(SEARCH_USR_ONLINE, pdu->caData))
        {
            QMessageBox::information(this, "Search", QString("%1: online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }
        else if (0 == strcmp(SEARCH_USR_NO, pdu->caData))
        {
            QMessageBox::information(this, "Search", QString("%1: not exist").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }
        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {

        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
    {
        QMessageBox::information(this, "Add friend", pdu->caData);
        break;
    }

    default:
        break;
    }
    free(pdu);
    pdu = NULL;
}

#if 0
void TcpClient::on_send_pb_clicked()
{
    QString strMsg = ui->lineEdit->text();
    if (!strMsg.isEmpty())
    {
        PDU *pdu = mkPDU(strMsg.size()+1);
        pdu->uiMsgType = 8888;
        memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.size());
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this, "send message", "message is null");
    }
}
#endif

void TcpClient::on_regist_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
//        qDebug() << "strPwd:" << strPwd;
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this, "Register", "Register failed: username or password is empty!");
    }
}


void TcpClient::on_login_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if (!strName.isEmpty() && !strPwd.isEmpty())
    {
        m_strLoginName = strName;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
//        qDebug() << "strPwd:" << strPwd;
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this, "Login", "Login failed: username or password is empty!");
    }
}


void TcpClient::on_cancel_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if (!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_CANCEL_REQUEST;
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
//        qDebug() << "strPwd:" << strPwd;
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this, "Login", "Login failed: username or password is empty!");
    }
}

