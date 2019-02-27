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

#include "zb_common.h"

#include "zb_cc25xx.h"

#ifdef ZB_SNIFFER_USB_TRACE
#include "hal_board.h"
#include "usb_uart.h"
#elif defined ZB_SNIFFER_SERIAL_TRACE
#include "zb_8051_serial.h"
#else
#error Sniffer trace type is not defined: use ZB_SNIFFER_USB_TRACE or ZB_SNIFFER_SERIAL_TRACE
#endif

#include "util_buffer.h"
#include "zb_sniffer_tools.h"

ringBuf_t rbTxBuf;

zb_uint8_t start_channel;
zb_uint8_t start_signal;
zb_uint8_t stop_signal;


void zb_sniffer_init()
{
  ZB_MEMSET((void*)&g_izb, 0, sizeof(zb_intr_globals_t));

  zb_8051_init_timer();
  init_zu2400();

  bufInit(&rbTxBuf);
#ifndef ZB_SNIFFER_USB_TRACE
  zb_init_8051_serial();
#else
  usbUartInit(UART_BAUDRATE_115200);
  ZB_SNIFFER_TURN_ON_LED();
#endif
  ZB_ENABLE_ALL_INTER();
}

void zb_start_sniffer()
{
    ZB_TRANSCEIVER_SET_CHANNEL(start_channel);
    ISFLUSHRX();
    ISRXON();
    
#ifndef ZB_SNIFFER_USB_TRACE
    ZB_SET_SERIAL_TRANSMIT_FLAG();
    ZB_ENABLE_SERIAL_INTER();
#endif  
}

void zb_clear_sniffer()
{
  /* Stop serial transmit, if enabled */
#ifndef ZB_SNIFFER_USB_TRACE
  ZB_CLEAR_SERIAL_TRANSMIT_FLAG();
  ZB_DISABLE_SERIAL_INTER();
#endif
  
  /* Turn off radio */
  ISRFOFF();
  ISFLUSHRX();

  /* Clear output buffer */
  bufFlush(&rbTxBuf);

#ifndef ZB_SNIFFER_USB_TRACE
  ZB_ENABLE_SERIAL_INTER();
#endif
}

zb_bool_t check_fcf(zb_uint16_t fcf)
{
  zb_bool_t ret = ZB_FALSE;
  /* Frame type is bigger than 0x03 (unknown) */
  zb_int16_t curr = !(fcf & (1 << 2));
  if (curr)
  {
    /* Reserved FCF bits 9-7 must be 000 */
    curr = !(fcf & (7 << 7));
    if (curr)
    {
      /* SAM != reserved (0x02) */
      curr = !((!(fcf & (1 << 11))) && (fcf & (1 << 10)));
      if (curr)
      {
        /* DAM != reserved (0x02) */
        curr = !((!(fcf & (zb_uint16_t)(1 << 15))) && (fcf & (1 << 14)));
        if (curr)
        {
          ret = ZB_TRUE;
        }
      }
    }
  }
  return ret;
}

#define ZB_SNIFFER_CHECK_MSB(byte) (byte & (1 << 7))  

void zb_put_out_queue()
{
  zb_uint8_t buf[ZB_SNIFFER_BUF_SIZE];
  zb_uint8_t len;
  zb_bool_t send_to_host = ZB_TRUE;
  zb_uint8_t i;

  len = RFD;
  /* Check the reserved 7th bit of length field */
  if (ZB_SNIFFER_CHECK_MSB(len))
  {
    send_to_host = ZB_FALSE;
  }
  len &= 0x7F;

  for (i = 0; i<len; i++)
  {
    buf[i] = RFD;
  }

  if (send_to_host)
  {
    send_to_host = bufHasEnoughSpace(&rbTxBuf, len + sizeof(zb_sniffer_hdr_t)) ? ZB_TRUE : ZB_FALSE;
  }

  if (send_to_host)
  {
    /* Check crc ok bit */
    send_to_host = (ZB_SNIFFER_CHECK_MSB(buf[len - 1])) ? ZB_TRUE : ZB_FALSE;
  }
  
  if (send_to_host)
  {
    zb_sniffer_hdr_t hdr;

    ZB_MEMSET(&hdr, 0, ZB_SNIFFER_HDR_SIZE);
    hdr.len = len + ZB_SNIFFER_HDR_SIZE;
    hdr.type = ZB_SNIFFER_OK;
    hdr.tail = 0;
    bufPut(&rbTxBuf, (zb_uint8_t *)(&hdr), sizeof(hdr));
    bufPut(&rbTxBuf, buf, len);
  }
#if 0  
  RFST = 0xED; /* flush rxfifo */
  RFST = 0xE3;
#endif
}

void zb_sniffer_logic_iteration()
{
  if (stop_signal)
  {
    zb_clear_sniffer();
    stop_signal = 0;
  }
  
  if (start_signal)
  {
    zb_start_sniffer();
    start_signal = 0;
  }

#ifndef ZB_SNIFFER_USB_TRACE
  zb_uint8_t nbytes = bufNumBytes(&rbTxBuf);
 
  if (bufNumBytes(&rbTxBuf) && !SER_CTX().tx_in_progress)
  {
    ZB_SET_SERIAL_TRANSMIT_FLAG();
  }
  else
  {
    ZB_GO_IDLE();
  }
#else
  usbUartProcess();
#endif
}
