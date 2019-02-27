/***************************************************************************
*                      ZBOSS ZigBee Pro 2007 stack                         *
*                                                                          *
*          Copyright (c) 2012 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*                            All rights reserved.                          *
*          Copyright (c) 2011 ClarIDy Solutions, Inc., Taipei, Taiwan.     *
*                       http://www.claridy.com/                            *
*                                                                          *
*          Copyright (c) 2011 Uniband Electronic Corporation (UBEC),       *
*                             Hsinchu, Taiwan.                             *
*                       http://www.ubec.com.tw/                            *
*                                                                          *
*          Copyright (c) 2011 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*                            All rights reserved.                          *
*                                                                          *
*                                                                          *
* ZigBee Pro 2007 stack, also known as ZBOSS (R) ZB stack is available     *
* under either the terms of the Commercial License or the GNU General      *
* Public License version 2.0.  As a recipient of ZigBee Pro 2007 stack, you*
* may choose which license to receive this code under (except as noted in  *
* per-module LICENSE files).                                               *
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
* Licensees holding valid ClarIDy/UBEC/DSR Commercial licenses may use     *
* this file in accordance with the ClarIDy/UBEC/DSR Commercial License     *
* Agreement provided with the Software or, alternatively, in accordance    *
* with the terms contained in a written agreement between you and          *
* ClarIDy/UBEC/DSR.                                                        *
*                                                                          *
****************************************************************************
PURPOSE: Logger implementation for cortex osif.
*/



#if defined cortexm4 && defined ZB_TRACE_LEVEL

#include <stdarg.h>
#include "zb_common.h"
#include "zb_osif.h"
#include "zb_ringbuffer.h"
#include "zb_mac_transport.h"
#include "zb_cortexm4_usart.h"
#include "zb_trace.h"

#ifdef ZB_TRAFFIC_DUMP_ON
void zb_dump_put_buf(zb_uint8_t *buf, zb_uint_t len, zb_bool_t is_w)
{
  static zb_uint16_t pkt_counter = 0;
  struct trace_hdr_s
  {
    zb_char_t sig[2];
    zb_mac_transport_hdr_t h;
  } ZB_PACKED_STRUCT;

  pkt_counter++;

  TRACE_MSG(TRACE_MAC1, ">>zb_dump_put_buf is_w %hd #%u buf %p [%hd] %hx %hx %hx %hx",
            (FMT__H_D_P_H_H_H_H_H,
             (zb_uint8_t)is_w,
             (zb_uint16_t)pkt_counter,
             buf,
             (zb_uint8_t)len,
             (zb_uint8_t)(buf[0]),
             (zb_uint8_t)(buf[1]),
             (zb_uint8_t)(buf[2]),
             (zb_uint8_t)(buf[3])));

  /* If UART trasport is used for zigbee data transfer, put zb_mac_transport_hdr_t in it.
     This header is used by pipe_data_router to find out type of received packet, it can be
     zb data, trace and dump */
  {
    struct trace_hdr_s hdr;

    hdr.sig[0] = 0xde;
    hdr.sig[1] = 0xad;
    hdr.h.len = len + sizeof(zb_mac_transport_hdr_t);
    hdr.h.type = ZB_MAC_TRANSPORT_TYPE_DUMP;
    if (is_w)
    {
      hdr.h.type |= 0x80;
    }
    ZB_HTOLE16(&hdr.h.time, &ZB_TIMER_GET());
    zb_osif_serial_put_bytes((zb_uint8_t *)&hdr, sizeof(hdr));
  }
  zb_osif_serial_put_bytes(buf, len);

  TRACE_MSG(TRACE_MAC1, "<<zb_dump_put_buf", (FMT__0));
}

void zb_mac_traffic_dump(zb_buf_t *buf, zb_bool_t is_w) ZB_SDCC_REENTRANT
{
	(void)buf;
	(void)is_w;
}
#endif  /* ZB_TRAFFIC_DUMP_ON */

#ifdef ZB_TRACE_LEVEL

/*! \addtogroup ZB_TRACE */
/*! @{ */

/* trace function which does not use printf() */

#define PUTCHAR_BUF_SIZE 127
#define SIGNATURE_SIZE 2

#define INITIAL_OFFSET (sizeof(zb_mac_transport_hdr_t) + SIGNATURE_SIZE)
static zb_uint8_t s_putchar_s[PUTCHAR_BUF_SIZE] = {0xde, 0xad, 0, ZB_MAC_TRANSPORT_TYPE_TRACE};
static zb_ushort_t s_putchar_len = INITIAL_OFFSET;    /* for hdr+signature */


static void zb_ser_flush_putchar()
{
  if (s_putchar_len > INITIAL_OFFSET)
  {
    /* Possible optimization to exclude double buffering: use osif-level serial ring
     * buffer here directly. */
    {
      zb_mac_transport_hdr_t *hdr = (zb_mac_transport_hdr_t *)(s_putchar_s + SIGNATURE_SIZE);
      hdr->time = ZB_TIMER_GET();
      hdr->len = s_putchar_len - SIGNATURE_SIZE;
    }
    zb_osif_serial_put_bytes(s_putchar_s, s_putchar_len);
    s_putchar_len = INITIAL_OFFSET;
  }
}

// Put symbols into special buffer and flush buffer when it is near to overflow.
void zb_ser_putchar(char c)
{
  s_putchar_s[s_putchar_len++] = c;
  if (s_putchar_len == PUTCHAR_BUF_SIZE)
  {
    zb_ser_flush_putchar();
  }
}

// Print numbers and digits to serial.
#define PRINTU_MACRO(v)                         \
{                                               \
  static char s[6];                             \
  zb_ushort_t i = 6;                            \
  do                                            \
  {                                             \
    s[--i] = '0' + (v) % 10;                    \
    (v) /= 10;                                  \
  }                                             \
  while (v);                                    \
  while (i < 6)                                 \
  {                                             \
    zb_ser_putchar(s[i]);                       \
    i++;                                        \
  }                                             \
}


static void printusp(zb_uint_t v)
{
  PRINTU_MACRO(v);
  zb_ser_putchar(' ');
}

// print hex numbers.
static void printxv(zb_minimal_vararg_t v)
{
  static char const s_x_tbl[] = "0123456789abcdef";
  zb_uint32_t i;

  for (i = 0 ; i < sizeof(v) * 2 ; i += 2)
  {
    zb_ser_putchar(s_x_tbl[((v) >> ((i+1) * 4)) & 0xf]);
    zb_ser_putchar(s_x_tbl[((v) >> (i * 4)) & 0xf]);
  }
}


/**
   Output trace message.

   @param file_name - source file name
   @param line_number - source file line
   @param args_size - total size of the trace arguments
   @param ... - trace arguments
 */
void zb_trace_msg_cortex(zb_char_t*file_name, zb_int_t line_number, zb_uint8_t args_size, ...)  
{
  static zb_uint16_t global_counter = 0;

  /* Possible optimization: switch to binary protocol. 2-times traffic economy. */

  /* Has no always running timer - print counter */
  /* %d %d %s:%d */
  printusp(global_counter);
  global_counter++;

  /* Put timer (in beacon intervals). Not normal timer (not always run, not
   * too precesious), but helpful for timeouts discover */
  printusp(ZB_TIMER_GET());

  /* Possible optimization: calculate 4-bytes (?) hash and put it instead of name. It gives us
   * traffic economy.
   * Need to modify win_com_dump as well: at start load all source file names and calculate
   * its hash, then search by hash.
   * More complex optimization is to pass here some 2-bytes id instead of name.
   */

  while (*file_name)
  {
    zb_ser_putchar(*file_name);
    file_name++;
  }

  zb_ser_putchar(':');
  printusp(line_number);

  {
    va_list arglist;
    zb_int_t size = args_size;
    va_start(arglist, args_size);
    while (size > 0)
    {
      zb_minimal_vararg_t v = va_arg(arglist, zb_minimal_vararg_t);
      printxv(v);
      size -= sizeof(v);
    }
    va_end(arglist);
  }

  zb_ser_putchar('\n');

  zb_ser_flush_putchar();
}

#endif /* trace on */

#endif 

/*! @} */
