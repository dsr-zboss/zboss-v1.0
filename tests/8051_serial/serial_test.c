/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Trivial 8051 serial test
*/

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_common.h"
#include "zb_osif.h"
#include "zb_8051_serial.h"
#include "zb_scheduler.h"


#if defined ZB_PLATFORM_LINUX_PC32
  #include "zb_mac_transport_linux.h"
#else
  #include "zb_mac_transport.h"
#endif

/* for SDCC: handler prototype MUST be defined in the same file with main() function */
DECLARE_SERIAL_INTER_HANDLER()
#define _testbit_(b) (b) ? (b = 0, 1) : 0;

zb_int_t dbg_send(char *str)
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

  ZB_SCHED_GLOBAL_LOCK();
  str_size = strlen(str);

  mac_transport = (zb_mac_transport_hdr_t*)send_buf->buf;
  mac_transport->type = ZB_MAC_TRANSPORT_TYPE_TRACE;
  mac_transport->len = str_size + hdr_size;
  send_buf->u.hdr.len = str_size + hdr_size;
  ZB_MEMCPY(send_buf->buf + hdr_size, str, str_size);

  if (!ZB_RING_BUFFER_IS_FULL(&ZG->ioctx.send_trace_rb))
  {
    ZB_RING_BUFFER_PUT(&ZG->ioctx.send_trace_rb, send_buf);
    ZB_START_SERIAL_WRITE(&ZG->ioctx);
  }
  else
  {
    /* TODO: add blocking for no space case */
  }
  ZB_SCHED_GLOBAL_UNLOCK();
  return 0;
}


MAIN()
{
  char str[128];
  zb_buf_t **out_buf = NULL;
  zb_buf_t **out_buf2 = NULL;
  zb_buf_t in_buf;
  zb_buf_t *send_buf = NULL;
  int i = 0;
  zb_mac_transport_hdr_t *mac_transport = NULL;
  int hdr_size = sizeof(zb_mac_transport_hdr_t);
  int str_size = 0;
  ZG_DECLARE;
  ARGV_UNUSED;

  ZB_STOP_WATCHDOG();
  ZB_SET_TRACE_DISABLED();

  zb_mac_transport_init( NULL, NULL, 1);
  /*memset(&ZG->ioctx, 0, sizeof(zb_io_ctx_t));
  ZB_RING_BUFFER_INIT(&ZG->ioctx.send_data_rb);
  ZB_RING_BUFFER_INIT(&ZG->ioctx.send_trace_rb);
  ZG->ioctx.recv_buf = NULL;
  ZG->ioctx.bytes_to_recv = 0;
  ZB_CLEAR_RECV_STATUS();
  zb_init_buffers();*/

  zb_init_8051_serial( ZB_SERIAL_BAUD_RATE);

  ZB_ENABLE_ALL_INTER();

  for (i = 0; i < 1; i++)
  {

    zb_mac_start_recv( &in_buf, 40);
    dbg_send( "test-test-test 2\n" );
#if 1
    while (1)
    {
      if (MAC_STATE_FOR_LAYER(ZB_MAC_IO_LAYER_RX) != ZB_MAC_STATE_IDLE)
      {
        break;
      }
      /* ZB_GO_IDLE(); some bytes are lost if go idle is on in simulator */
    }
#endif
    send_buf = zb_get_out_buf();
    ZB_ASSERT(send_buf);
    if (send_buf != NULL)
    {
      ZB_MEMSET(send_buf, 0, sizeof(zb_buf_t));
    }

    ZB_SCHED_GLOBAL_LOCK();
    ZB_MEMSET(str, 0, sizeof(str));
    sprintf(str, "recv, len %i: \n", (int)in_buf.u.hdr.len);
    str_size = strlen(str);
    ZB_MEMCPY(str + str_size, in_buf.buf, in_buf.u.hdr.len);
    str_size += in_buf.u.hdr.len;

    ZB_MEMCPY(str + str_size, "\n", strlen("\n"));
    str_size += strlen("\n");
    ZB_MEMCPY(send_buf->buf + hdr_size, str, str_size);

    mac_transport = (zb_mac_transport_hdr_t*)send_buf->buf;
    mac_transport->type = ZB_MAC_TRANSPORT_TYPE_TRACE;
    mac_transport->len = str_size + hdr_size;
    send_buf->u.hdr.len = str_size + hdr_size;

    if (!ZB_RING_BUFFER_IS_FULL(&ZG->ioctx.send_trace_rb))
    {
      ZB_RING_BUFFER_PUT(&ZG->ioctx.send_trace_rb, send_buf);
      ZB_START_SERIAL_WRITE(&ZG->ioctx);
    }
    else
    {
      /* TODO: add blocking for no space case */
    }
    ZB_SCHED_GLOBAL_UNLOCK();
  }


  for (;;)
  {
    ZB_GO_IDLE();
  }

  MAIN_RETURN(0);
}

/*! @} */
