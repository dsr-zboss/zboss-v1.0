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
#include "snifferserialdevice.h"
#include <QDebug>

const char SnifferSerialDevice::SNIFFER_STOP_CHAR = 255;

SnifferSerialDevice::SnifferSerialDevice(QObject *parent) :
    QSerialPort(parent)
{
}

SnifferSerialDevice::SnifferSerialDevice(QString name) :
    QSerialPort(name)
{
}

bool SnifferSerialDevice::open(OpenMode mode)
{
    bool ret = QSerialPort::open(mode);

    if (ret)
    {
        ret = (setBaudRate(QSerialPort::Baud115200) &&
               setDataBits(QSerialPort::Data8) &&
               setParity(QSerialPort::NoParity) &&
               setFlowControl(QSerialPort::NoFlowControl) &&
               setStopBits(QSerialPort::OneStop));
    }

    return ret;
}

bool SnifferSerialDevice::startSnifferDevice(char channel)
{
    qDebug() << QString("Start device at channel %1").arg(channel);
    return (writeData(&channel, 1) == 1);
}

bool SnifferSerialDevice::stopSnifferDevice()
{
    /* send any symbol that's not in range [ZB_MIN_CHANNEL; ZB_MAX_CHANNEL] */
    char stop = SNIFFER_STOP_CHAR;

    qDebug() << QString("Stop device by sym %1").arg(stop);
    return (writeData(&stop, 1) == 1);
}

QString SnifferSerialDevice::errorMsg()
{
    QSerialPort::SerialPortError err = QSerialPort::error();

    return strSerialError(err);
}

QString SnifferSerialDevice::strSerialError(QSerialPort::SerialPortError err)
{
    QString res;

    switch (err)
    {
    case QSerialPort::NoError: res = "No error"; break;
    case QSerialPort::DeviceNotFoundError: res = "Device not found"; break;
    case QSerialPort::PermissionError: res = "Permission denied"; break;
    case QSerialPort::OpenError: res = "Already open"; break;
    case QSerialPort::ParityError: res = "Device paritty error"; break;
    case QSerialPort::FramingError: res = "Device framing error"; break;
    case QSerialPort::ReadError: res = "Device read error"; break;
    case QSerialPort::WriteError: res = "Device write error"; break;
    case QSerialPort::ResourceError: res = "Device unplugged"; break;
    default: res = "Device error";
    };

    return res;
}
