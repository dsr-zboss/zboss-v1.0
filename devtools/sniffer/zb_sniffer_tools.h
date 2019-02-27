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

#ifndef ZB_SNIFFER_TOOLS_H
#define ZB_SNIFFER_TOOLS_H

#include "zb_common.h"

#ifdef ZB_SNIFFER_USB_TRACE
#define UART_BAUDRATE_115200        0x06
#endif

#include "util_buffer.h"

typedef enum
{
  ZB_SNIFFER_OK = 0,
  ZB_SNIFFER_TOO_BIG_LEN = 1,
  ZB_SNIFFER_OUT_BUF_OVERFLOW = 2
} zb_sniffer_err_e;

typedef struct zb_sniffer_hdr_s
{
  zb_uint8_t len;   /*!< Packet length, including hdr */
  zb_uint8_t type;  /*!< Packet type, see zb_sniffer_err_e */
  zb_uint16_t tail; /* tail with zeroes */
} ZB_PACKED_STRUCT
zb_sniffer_hdr_t;

#define ZB_SNIFFER_HDR_SIZE   sizeof(zb_sniffer_hdr_t)
#define ZB_SNIFFER_BUF_SIZE         128

void zb_sniffer_init();
void zb_start_sniffer();
void zb_clear_sniffer();
void zb_put_out_queue();
void zb_sniffer_logic_iteration();

extern zb_uint8_t start_channel;
extern zb_uint8_t start_signal;
extern zb_uint8_t stop_signal;


/* Light green LED */
#ifdef ZB_SNIFFER_USB_TRACE
#define ZB_SNIFFER_TURN_ON_LED() (P0DIR |= 0x01, P0_0 |= 0x01)
#endif

#endif
