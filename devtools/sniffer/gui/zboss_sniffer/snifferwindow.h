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
#ifndef SNIFFERWINDOW_H
#define SNIFFERWINDOW_H

#include <QMainWindow>
#include "sniffercontroller.h"


namespace Ui {
class SnifferWindow;
}

class SnifferWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SnifferWindow(QWidget *parent = 0);
    ~SnifferWindow();

signals:

private slots:

    void on_bttnBrowsePath_clicked();

    void on_bttnUp_clicked();

    void on_bttnStart_clicked();

    void on_bttnStop_clicked();

    void on_bttnCancel_clicked();

    void reWriteLPath();

    void printLogMessage(QString msg);

    void refreshToStart();

    void refreshToPause();

    void refreshToStop();

    void freeze();

    void defrost();

    void on_bttnRadioConfig_clicked();

    void updateRadioConfigAbility();

private:

    enum SnifferRadioConfigE
    {
        ZB_START_RADIO_CONFIG = 0,
        ZB_PRO                = ZB_START_RADIO_CONFIG,
        ZB_SUB_GHZ_EU1        = 1,
        ZB_SUB_GHZ_EU2        = 2,
        ZB_SUB_GHZ_US         = 3,
        ZB_SUB_GHZ_JP         = 4,
        ZB_SUB_GHZ_CN         = 5,
        ZB_END_RADIO_CONFIG   = 6
    };

    typedef struct SnifferChannelConfigS
    {
        QString title;
        uchar minChannel;
        uchar maxChannel;
    }
    SnifferChannelConfigT;

    enum SnifferStateE
    {
        SNIFFER_START = 0,
        SNIFFER_RESUME = 1,
        SNIFFER_RESUME_WAIT_SHARK = 2,
        SNIFFER_PAUSE = 3
    };

    static const char UNDEFINED_PREFERENCE;
    static const QString PREFERENCES_NAME;
    static const qint32 MAX_LOGS_VISIBLE;
    static const SnifferChannelConfigT CHANNEL_CONFIGS[ZB_END_RADIO_CONFIG];

    Ui::SnifferWindow *ui;
    SnifferController *controller;

    SnifferStateE bttnStartState;
    QString prefferedWs;
    QString prefferedPcap;
    uchar prefferedChannels[ZB_END_RADIO_CONFIG];

    void initSniffingMode();
    void initCmbCom();
    void initCmbRadioConfig();

    void loadPreferences();
    void savePreferences();

    void setSnifferState(SnifferStateE state);

public slots:
    void closeHandler();
    void initCmbChannel(int region);
};

#endif // SNIFFERWINDOW_H
