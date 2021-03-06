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
#ifndef SNIFFERSERIALDEVICE_H
#define SNIFFERSERIALDEVICE_H

#include <QSerialPort>

class SnifferSerialDevice : public QSerialPort
{
    Q_OBJECT

private:
    static const char SNIFFER_STOP_CHAR;

public:

    enum SnifferReadStatusE
    {
        SNIFFER_PACKET_AVAILABLE = 0,
        SNIFFER_BAD_HEADER = 1,
        SNIFFER_REPORT_ERROR = 2,
        SNIFFER_READ_FAIL = 3
    };

    explicit SnifferSerialDevice(QObject *parent = 0);
    explicit SnifferSerialDevice(QString name);
    /* Buffer size must be 128 bytes at least */
    bool open(OpenMode mode);
    bool startSnifferDevice(char channel);
    bool stopSnifferDevice();

    QString errorMsg();
    QString strSerialError(QSerialPort::SerialPortError err);

signals:

private slots:
};

#endif // SNIFFERSERIALDEVICE_H
