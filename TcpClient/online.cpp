﻿#include "online.h"
#include "ui_online.h"
#include <QDebug>
#include "tcpclient.h"

Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    qDebug() << "Entry showUsr function";
    uint uiSize = pdu->uiMsgLen / 32;
    char caTmp[32];
    for (int i = 0; i <uiSize; i++)
    {
       memcpy(caTmp, (char*)(pdu->caMsg) + i * 32, 32);
       ui->online_lw->addItem(caTmp);
    }
}

void Online::on_addFriend_pb_clicked()
{
    QListWidgetItem *pItem = ui->online_lw->currentItem();
    QString strPerUsrName = pItem->text();
    QString loginName = TcpClient::getInstance().loginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData, strPerUsrName.toStdString().c_str(), strPerUsrName.size());
    memcpy(pdu->caData + 32, loginName.toStdString().c_str(), loginName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}
