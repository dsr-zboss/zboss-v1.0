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
#ifndef SNIFFERCONTROLLER_H
#define SNIFFERCONTROLLER_H

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include "snifferserialdevice.h"
#include "pcapdumper.h"
#include "wiresharkbridge.h"

class SnifferController : public QObject
{
    Q_OBJECT
public:

    static const qint32 SNIFFER_HEADER_SIZE;
    static const qint32 SNIFFER_MAX_PAYLOAD_SIZE;

    explicit SnifferController(QObject *parent = 0);
    explicit SnifferController(QString portName, bool needWireshark, QString path,
                               quint8 radioConfig, quint8 channel, QObject *parent = 0);
    ~SnifferController();

    void startSniffer();
    void pauseSniffer();
    void resumeSniffer();
    void stopSniffer();

    void reconfigureRadio(quint8 radioConfig, quint8 channel);
    bool startWireshark();


private:

    enum SnifferHeaderTypeE
    {
      ZB_PACKET_TYPE_COMPLETE  = 0,
      ZB_PACKET_TYPE_ERROR_BIG_LEN  = 1,
      ZB_PACKET_TYPE_ERROR_OVERFLOW = 2,
      ZB_PACKET_TYPE_ERROR
    };

    typedef struct SnifferHeaderS
    {
        unsigned char len;
        unsigned char type;
        unsigned short tail;
    } SnifferHeaderT;

    static const qint32 SNIFFER_WAIT_DEVICE_CLEAR_TIME;
    static const qint32 WIRESHARK_WAIT_FOR_CONNECT_TIME;
    static const qint32 WIRESHARK_WAIT_FOR_RECONNECT_TIME;

    /* Initial context */
    QString portName;
    bool needWireshark;
    QString path;
    quint8 radioConfig;
    quint8 channel;
    /* ---------------- */

    /* Sniffing services */
    WiresharkBridge *bridge;
    SnifferSerialDevice *device;
    PcapDumper *dumper;
    /* ------------------------- */

    /* Sniffing context */
    bool isSniffing;
    bool isPayload;
    quint8 bytesLeft;
    quint8 packetLen;
    QByteArray currentPacket;
    /* -------------------- */


    bool openDevice();
    void doSniff();
    QString printPacketHex(QByteArray arr);


signals:
    void logMessage(QString msg);
    void errorOccured(QString desc);
    void syncProcessStart();
    void syncProcessFinished();
    void nowSniffing();
    void waitingWireshark();

private slots:
    void deviceReadAvailable();
    void wiresharkHasConnected(QLocalSocket *sock);
    void wiresharkHasDisconnected();
    void deviceError(QSerialPort::SerialPortError err);

    void continueStartSniffer();

public slots:

};

#endif // SNIFFERCONTROLLER_H
