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
#include "pcapdumper.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>

const int PcapDumper::PCAP_FCS_LEN = 2;

PcapDumper::PcapDumper(QObject *parent) :
    QObject(parent)
{
}

PcapDumper::PcapDumper(QIODevice *dumpDest, QObject *parent):
    QObject(parent)
{
    dumpDestination = dumpDest;
}

bool PcapDumper::isOpen()
{
    bool ret = (dumpDestination != NULL);

    if (ret)
    {
        ret = dumpDestination->isOpen();
    }

    return ret;
}

bool PcapDumper::initDump()
{
    bool ret = false;
    if (dumpDestination != NULL)
    {
        qDebug() << QString(dumpDestination->metaObject()->className());

        if (!dumpDestination->isOpen())
        {
            if (dumpDestination->open(QIODevice::WriteOnly))
            {
                ret = true;
            }
        }

        /* Check for QLocalSocket. H8 reflection(( */
        if (QString(dumpDestination->metaObject()->className()).contains("QLocalSocket"))
        {
            ret = true;
        }

        if (ret)
        {
            writeGlobalHeader();
        }
    }

    return ret;
}

void PcapDumper::writePacket(char *buf, int len)
{
    len -= PCAP_FCS_LEN;
    writePacketHeader(len);
    dumpDestination->write((char *)buf, len);
    if (!QString(dumpDestination->metaObject()->className()).contains("QLocalSocket"))
    {
        ((QFile *)dumpDestination)->flush();
    }
}

void PcapDumper::deinitDump()
{
    /* Socket is handled by WiresharkBridge */
    if (!QString(dumpDestination->metaObject()->className()).contains("QLocalSocket"))
    {
        dumpDestination->close();
    }
}

void PcapDumper::writeGlobalHeader()
{
    quint32 magicNumber = 0xa1b2c3d4;
    quint16 majorVer = 2;
    quint16 minorVer = 4;
    qint32 zone = 0;
    quint32 sigfigs = 0;
    quint32 snaplen = 65535;
    quint32 macProtocol = 195; /* MAC 802.15.4 proto */

    dumpDestination->write((char *)&magicNumber, sizeof(magicNumber));
    dumpDestination->write((char *)&majorVer, sizeof(majorVer));
    dumpDestination->write((char *)&minorVer, sizeof(minorVer));
    dumpDestination->write((char *)&zone, sizeof(zone));
    dumpDestination->write((char *)&sigfigs, sizeof(sigfigs));
    dumpDestination->write((char *)&snaplen, sizeof(snaplen));
    dumpDestination->write((char *)&macProtocol, sizeof(macProtocol));
}

void PcapDumper::writePacketHeader(int len)
{
    quint64 unixTime = QDateTime::currentMSecsSinceEpoch();
    quint32 sec = unixTime / 1000;
    quint32 msec = (quint32)(unixTime - sec * 1000);
    quint32 fileLen = len;
    quint32 packetLen = len + PCAP_FCS_LEN;

    dumpDestination->write((char *)&sec, sizeof(sec));
    dumpDestination->write((char *)&msec, sizeof(msec));
    dumpDestination->write((char *)&fileLen, sizeof(fileLen));
    dumpDestination->write((char *)&packetLen, sizeof(packetLen));
}
