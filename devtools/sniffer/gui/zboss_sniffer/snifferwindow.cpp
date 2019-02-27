/***************************************************************************
*                      ZBOSS ZigBee Pro 2007 stack                         *
*                                                                          *
*          Copyright (c) 2013 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*          Copyright (c) 2012 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*          Copyright (c) 2011 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*                            All rights reserved.                          *
*                                                                          *
*                                                                          *
* ZigBee Pro 2007 stack, also known as ZBOSS (R) ZB stack is available     *
* under either the terms of the Commercial License or the GNU General      *
* Public License version 2.0.  As a recipient of ZigBee Pro 2007 stack,    *
* you may choose which license to receive this code under (except as noted *
* in per-module LICENSE files).                                            *
*                                                                          *
* ZBOSS is a registered trademark of DSR Corporation AKA Data Storage      *
* Research LLC.                                                            *
*                                                                          *
* GNU General Public License Usage                                         *
* This file may be used under the terms of the GNU General Public License  *
* version 2.0 as published by the Free Software Foundation and appearing   *
* in the file LICENSE.GPL included in the packaging of this file.  Please  *
* review the following information to ensure the GNU General Public        *
* License version 2.0 requirements will be met:                            *
* http://www.gnu.org/licenses/old-licenses/gpl-2.0.html.                   *
*                                                                          *
* Commercial Usage                                                         *
* Licensees holding valid DSR Commercial licenses may use this file        *
* in accordance with the DSR Commercial License Agreement provided with    *
* the Software or, alternatively, in accordance with the terms contained   *
* in a written agreement between you and DSR.                              *
****************************************************************************
PURPOSE:
*/

#include <QtSerialPort/QSerialPortInfo>
#include <QProcess>
#include <QFileDialog>
#include <QMessageBox>
#include "snifferwindow.h"
#include "ui_snifferwindow.h"

const char SnifferWindow::UNDEFINED_PREFERENCE = -1;
const QString SnifferWindow::PREFERENCES_NAME = "pref";
const qint32 SnifferWindow::MAX_LOGS_VISIBLE = 100;
const SnifferWindow::SnifferChannelConfigT
SnifferWindow::CHANNEL_CONFIGS[SnifferWindow::ZB_END_RADIO_CONFIG] =
{
    {QString("Zigbee Pro"), 11, 26},
    {QString("SubGHz EU1"),  0, 34},
    {QString("SubGHz EU2"),  0, 17},
    {QString("SubGHz US"),   0, 10},
    {QString("SubGHz JP"),   0, 10},
    {QString("SubGHz CN"),   0, 10}
};

SnifferWindow::SnifferWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SnifferWindow)
{
    ui->setupUi(this);
    ui->hlRadio->setAlignment(ui->bttnRadioConfig, Qt::AlignBottom);

    initSniffingMode();
    initCmbRadioConfig();
    /*initCmbChannel(ZB_START_RADIO_CONFIG);*/
    initCmbCom();

    setSnifferState(SNIFFER_START);
    loadPreferences();
}

SnifferWindow::~SnifferWindow()
{
    delete ui;
}

void SnifferWindow::initSniffingMode()
{
    connect(ui->rbPcap, SIGNAL(toggled(bool)), this, SLOT(reWriteLPath()));
    connect(ui->rbWireshark, SIGNAL(toggled(bool)), this, SLOT(reWriteLPath()));
}

void SnifferWindow::initCmbCom()
{
    ui->cmbCom->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->cmbCom->addItem(QString("%1 (%2)").arg(info.description(), info.portName()),
                            QVariant(info.portName()));
    }
}

void SnifferWindow::initCmbRadioConfig()
{
    ui->cmbRadioConfig->clear();
    for (int i = ZB_START_RADIO_CONFIG; i < ZB_END_RADIO_CONFIG; i++)
    {
        ui->cmbRadioConfig->addItem(CHANNEL_CONFIGS[i].title, QVariant(i));
    }

    connect(ui->cmbRadioConfig, SIGNAL(currentIndexChanged(int)),
                this, SLOT(initCmbChannel(int)));
    connect(ui->cmbRadioConfig, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRadioConfigAbility()));
}

void SnifferWindow::initCmbChannel(int region)
{
    ui->cmbChannel->clear();

    /* from min zb channel to max */
    for (uint i = CHANNEL_CONFIGS[region].minChannel;
         i <= CHANNEL_CONFIGS[region].maxChannel; i++)
    {
        QString ch = QString::number(i, 16);

        ch = ch.toUpper();
        /* Add zero to the one-number hexademical (0xA -> 0x0A) */
        if (i < 16)
        {
            ch = QString("0%1").arg(ch);
        }
        ui->cmbChannel->addItem(QString("0x%1 (%2)").arg(ch, QString::number(i, 10)), QVariant(i));
    }

    ui->cmbChannel->setCurrentIndex(prefferedChannels[region]);

    connect(ui->cmbChannel, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRadioConfigAbility()));
}

void SnifferWindow::loadPreferences()
{
    QFile *pref = new QFile(PREFERENCES_NAME);
    SnifferRadioConfigE prefferedRegion = ZB_START_RADIO_CONFIG;

    prefferedWs = "";
    prefferedPcap = "";

    for (int i = ZB_START_RADIO_CONFIG; i < ZB_END_RADIO_CONFIG; i++)
    {
        prefferedChannels[i] = 0;
    }

    if (pref->exists())
    {
        if (pref->open(QIODevice::ReadOnly))
        {
            QTextStream in(pref);
            in.setCodec("UTF-8");

            QString tmp = in.readLine();
            if (!tmp.isNull())
            {
                prefferedWs = tmp;
                tmp = in.readLine();
            }

            if (!tmp.isNull())
            {
                prefferedPcap = tmp;
                tmp = in.readLine();
            }

            if (!tmp.isNull())
            {
                int index = ui->cmbCom->findData(tmp);

                if (index != -1)
                {
                    ui->cmbCom->setCurrentIndex(index);
                }
                tmp = in.readLine();
            }

            if (!tmp.isNull())
            {
                prefferedRegion = (SnifferRadioConfigE)tmp.toInt();
                tmp = in.readLine();
            }

            for (int i = ZB_START_RADIO_CONFIG; i < ZB_END_RADIO_CONFIG; i++)
            {
                if (!tmp.isNull())
                {
                    prefferedChannels[i] = tmp.toInt();
                    tmp = in.readLine();
                }
                else
                {
                    break;
                }
            }

            pref->close();
        }
    }

    /* on start Wireshark rb is checked */
    ui->lePath->setText(prefferedWs);
    ui->cmbRadioConfig->setCurrentIndex(prefferedRegion);
    initCmbChannel(prefferedRegion);
}

void SnifferWindow::savePreferences()
{
    QFile *pref = new QFile(PREFERENCES_NAME);

    if (ui->rbWireshark->isChecked())
    {
        prefferedWs = ui->lePath->text();
    }
    else
    {
        prefferedPcap = ui->lePath->text();
    }

    prefferedChannels[ui->cmbRadioConfig->itemData(ui->cmbRadioConfig->currentIndex()).toInt()] =
            ui->cmbChannel->currentIndex();

    if (pref->open(QIODevice::WriteOnly))
    {
        QTextStream out(pref);
        out.setCodec("UTF-8");

        out << prefferedWs;
        out << "\n";
        out << prefferedPcap;
        out << "\n";
        /*
         * Save the whole text for COM port: there is no guarantee
         * that next time it will appear under the same index
        */
        out << ui->cmbCom->itemData(ui->cmbCom->currentIndex()).toString();
        out << "\n";
        out << (uchar)ui->cmbRadioConfig->currentIndex();
        out << "\n";

        for (int i = ZB_START_RADIO_CONFIG; i < ZB_END_RADIO_CONFIG; i++)
        {
            out << prefferedChannels[i];
            out << "\n";
        }

        pref->close();
    }
}

void SnifferWindow::setSnifferState(SnifferStateE state)
{
    bttnStartState = state;

    switch (state)
    {
    case SNIFFER_START:

        ui->bttnStart->setText("Start");
        break;

    case SNIFFER_PAUSE:

        ui->bttnStart->setText("Pause");
        break;

    case SNIFFER_RESUME:
    case SNIFFER_RESUME_WAIT_SHARK:

        ui->bttnStart->setText("Resume");
        break;
    }
}

void SnifferWindow::on_bttnBrowsePath_clicked()
{
    QFileDialog dialog(this);
    QString invitation = ui->rbWireshark->isChecked() ?
                "Specify the path to Wireshark" : "Specify the path to .pcap file";

    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setWindowTitle(invitation);
    if (dialog.exec())
    {
        QStringList pathes = dialog.selectedFiles();
        ui->lePath->setText(pathes[0]);
    }
}

void SnifferWindow::on_bttnUp_clicked()
{
    initCmbCom();
}

void SnifferWindow::on_bttnStart_clicked()
{
    QString portName = (ui->cmbCom->count() > 0 ?
                            ui->cmbCom->itemData(ui->cmbCom->currentIndex()).toString() : "");

    uchar radioConfig = (uchar)ui->cmbRadioConfig->itemData(ui->cmbRadioConfig->currentIndex()).toUInt();
    uchar channel = (uchar)ui->cmbChannel->itemData(ui->cmbChannel->currentIndex()).toUInt();
    bool needWireshark = ui->rbWireshark->isChecked();
    QString path = ui->lePath->text();

    switch (bttnStartState)
    {

    case SNIFFER_START:

        if (!ui->lePath->text().isEmpty())
        {
            ui->teLogBrowser->clear();
            setSnifferState(SNIFFER_PAUSE);
            refreshToStop();

            savePreferences();

            controller = new SnifferController(portName, needWireshark, path, radioConfig, channel);

            connect(controller, SIGNAL(logMessage(QString)), this, SLOT(printLogMessage(QString)));
            connect(controller, SIGNAL(errorOccured(QString)), this, SLOT(refreshToStart()));
            connect(controller, SIGNAL(syncProcessStart()), this, SLOT(freeze()));
            connect(controller, SIGNAL(syncProcessFinished()), this, SLOT(defrost()));
            connect(controller, SIGNAL(nowSniffing()), this, SLOT(refreshToStop()));
            connect(controller, SIGNAL(waitingWireshark()), this, SLOT(refreshToPause()));

            controller->startSniffer();
        }
        else
        {
            ui->teLogBrowser->append(QString("Specify path to %1").arg(
                        ui->rbWireshark->isChecked() ? "Wireshark" : "Pcap file"));
        }
        break;

    case SNIFFER_PAUSE:

        setSnifferState(SNIFFER_RESUME);

        controller->pauseSniffer();
        break;

    case SNIFFER_RESUME:

        setSnifferState(SNIFFER_PAUSE);

        controller->resumeSniffer();
        break;

    case SNIFFER_RESUME_WAIT_SHARK:
        /*
         * Device is ready, but Wireshark connection is
         * lost or not established yet
         */

        /*
         * Create dialog for opening new shark
         */

        QMessageBox::StandardButton reply;
        QString text =
                QString("Sniffer has lost connection with Wireshark.\nIf Wireshark is still opened click ") +
                QString("Start capture to resume. Also you can start new copy of Wireshark.\n\n") +
                QString("Would you like to start new Wireshark?");

        reply = QMessageBox::question(this,
                                      "Wireshark connection is lost",
                                      text,
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            controller->startWireshark();
        }

        break;
    }
}

void SnifferWindow::on_bttnStop_clicked()
{
    controller->stopSniffer();
    refreshToStart();
}

void SnifferWindow::on_bttnCancel_clicked()
{
    QApplication::quit();
}

void SnifferWindow::reWriteLPath()
{
    ui->lPath->setText(QString("<b>Specify path to %1</b>").arg(
                             ui->rbWireshark->isChecked() ? "Wireshark" : "Pcap file"));
    ui->lePath->setText(ui->rbWireshark->isChecked() ? prefferedWs : prefferedPcap);
}

void SnifferWindow::printLogMessage(QString msg)
{
    if (ui->teLogBrowser->document()->blockCount() == MAX_LOGS_VISIBLE)
    {
        ui->teLogBrowser->clear();
    }
    ui->teLogBrowser->append(msg);
}

void SnifferWindow::refreshToStart()
{
    controller->deleteLater();
    setSnifferState(SNIFFER_START);
    ui->bttnStop->setEnabled(false);
    ui->bttnBrowsePath->setEnabled(true);
    ui->bttnUp->setEnabled(true);
    ui->bttnRadioConfig->setEnabled(false);
    ui->rbWireshark->setEnabled(true);
    ui->rbPcap->setEnabled(true);
    ui->lePath->setEnabled(true);
    ui->cmbCom->setEnabled(true);
    /*
    ui->cmbRadioConfig->setEnabled(true);
    ui->cmbChannel->setEnabled(true);
    */
    ui->bttnRadioConfig->setEnabled(false);
}

void SnifferWindow::refreshToPause()
{
    setSnifferState(SNIFFER_RESUME_WAIT_SHARK);
}

void SnifferWindow::refreshToStop()
{
    setSnifferState(SNIFFER_PAUSE);
    ui->bttnStop->setEnabled(true);
    ui->bttnUp->setEnabled(false);
    ui->bttnBrowsePath->setEnabled(false);
    ui->rbWireshark->setEnabled(false);
    ui->rbPcap->setEnabled(false);
    ui->lePath->setEnabled(false);
    ui->cmbCom->setEnabled(false);
    /*
    ui->cmbRadioConfig->setEnabled(false);
    ui->cmbChannel->setEnabled(false);
    */
}

void SnifferWindow::freeze()
{
    ui->bttnRadioConfig->setEnabled(false);
    ui->bttnStart->setEnabled(false);
    ui->bttnStop->setEnabled(false);
}

void SnifferWindow::defrost()
{
    ui->bttnStart->setEnabled(true);
    ui->bttnStop->setEnabled(true);
}

void SnifferWindow::closeHandler()
{
    if (bttnStartState != SNIFFER_START)
    {
        controller->stopSniffer();
    }
}

void SnifferWindow::on_bttnRadioConfig_clicked()
{
    uchar radioConfig = ui->cmbRadioConfig->itemData(ui->cmbRadioConfig->currentIndex()).toUInt();
    uchar channel = ui->cmbChannel->itemData(ui->cmbChannel->currentIndex()).toUInt();

    controller->reconfigureRadio(radioConfig, channel);

    ui->bttnRadioConfig->setEnabled(false);
}

void SnifferWindow::updateRadioConfigAbility()
{
    /* Radio settings can be updated only if we're sniffing */
    if (bttnStartState != SNIFFER_START)
    {
        ui->bttnRadioConfig->setEnabled(true);
    }
}
