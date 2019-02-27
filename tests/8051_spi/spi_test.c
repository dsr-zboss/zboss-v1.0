/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Trivial 8051 spi test
*/

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_common.h"
#include "zb_osif.h"
#include "zb_scheduler.h"


#if defined ZB_PLATFORM_LINUX_PC32
  #include "zb_mac_transport_linux.h"
#else
  #include "zb_mac_transport.h"
#endif

/* for SDCC: handler prototype MUST be defined in the same file with main() function */
DECLARE_SPI_INTER_HANDLER()

#define _testbit_(b) (b) ? (b = 0, 1) : 0;

zb_int_t dbg_spi_send(ZPAR char *str)
{
  zb_buf_t *send_buf = NULL;
  zb_mac_transport_hdr_t *mac_transport = NULL;
  zb_int_t str_size = 0;
  zb_int_t hdr_size = sizeof(zb_mac_transport_hdr_t);

  send_buf = zb_get_out_buf();
  ZB_ASSERT(send_buf);
  if (send_buf)
  {
    ZB_MEMSET(send_buf, 0, sizeof(zb_buf_t));
  }

  str_size = strlen(str);

  mac_transport = (zb_mac_transport_hdr_t*)send_buf->buf;
  mac_transport->type = ZB_MAC_TRANSPORT_TYPE_DATA;
  mac_transport->len = str_size + hdr_size;
  send_buf->u.hdr.len = str_size + hdr_size;
  ZB_MEMCPY(send_buf->buf + hdr_size, str, str_size);

  zb_mac_put_data( send_buf);

  ZB_SCHED_GLOBAL_UNLOCK();
  return 0;
}

MAIN()
{
  char str[128];
  int i = 0;
  zb_buf_t *buf = NULL;
  int str_offset = 0;
  ZG_DECLARE;
  ARGV_UNUSED;

  ZB_STOP_WATCHDOG();
  ZB_SET_TRACE_DISABLED();

  zb_mac_transport_init( NULL, NULL, 1);

  zb_spi_init();

  ZB_ENABLE_ALL_INTER();

  dbg_spi_send( "Start SPI test");
  while (1)
  {
    if (ZB_GET_SEND_STATUS(&ZG->ioctx ) != ZB_NO_IO)
    {
      break;
    }
  }

  for (i = 0; i < 10; i++)
  {
    buf = zb_get_in_buf();
    zb_mac_start_recv( buf, 50);
    while (1)
    {
      if (MAC_STATE_FOR_LAYER(ZB_MAC_IO_LAYER_RX) != ZB_MAC_STATE_IDLE)
      {
        break;
      }
    }
    memset(str, 0, sizeof(str));
    sprintf(str, "%i) received: [", i);
    str_offset = strlen(str);
    memcpy(str + str_offset, buf->buf, buf->u.hdr.len);
    str_offset += buf->u.hdr.len;
    memcpy(str + str_offset, "]", sizeof("]"));
    zb_free_buf( buf);

    dbg_spi_send( str);
    while (1)
    {
      if (ZB_GET_SEND_STATUS(&ZG->ioctx) != ZB_NO_IO)
      {
        break;
      }
    }
  }

  dbg_spi_send( "SPI test finished");

  for (;;)
  {
    ZB_GO_IDLE();
  }

  MAIN_RETURN(0);
}

/*! @} */
