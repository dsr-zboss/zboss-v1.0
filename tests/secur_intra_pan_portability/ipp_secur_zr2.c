                         /***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Test for ZC application written using ZDO.
*/


#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

#define TEST_BUFFER_LEN 30

#ifdef ZB_NS_BUILD 
#define SEND_TMOUT ZB_MILLISECONDS_TO_BEACON_INTERVAL(1000)
#else 
#define SEND_TMOUT ZB_MILLISECONDS_TO_BEACON_INTERVAL(4000)
#endif

#ifndef ZB_ROUTER_ROLE
#error Router role is not compiled!
#endif

/* For NS build first ieee addr byte should be unique */
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr = {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
#endif


/*! \addtogroup ZB_TESTS */
/*! @{ */

/*
  ZR joins to ZC, then sends APS packet.
 */


MAIN()
{
  ARGV_UNUSED;

#ifndef KEIL
  if ( argc < 3 )
  {
    printf("%s <read pipe path> <write pipe path>\n", argv[0]);
    return 0;
  }
#endif

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_zr2", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zr2", "3", "3");
#endif
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);

  if (zdo_dev_start() != RET_OK)
  {
    TRACE_MSG(TRACE_ERROR, "zdo_dev_start failed", (FMT__0));
  }
  else
  {
    zdo_main_loop();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}

void send_packets(zb_uint8_t param);


void set_bad_key_seq(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  TRACE_MSG(TRACE_APS1, "Now set bad key seq number (10)", (FMT__0));

  /* change key seq number to be invalid to force rejoin to ZR1 */
  ZG->nwk.nib.active_key_seq_number = 10;
  ZG->nwk.nib.secur_material_set[0].key_seq_number = 10;
}


void set_bad_frame_cnt(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  TRACE_MSG(TRACE_APS1, "Now set bad frame counter (0)", (FMT__0));

  /* change key seq number to be invalid to force rejoin to ZR1 */
  ZG->nwk.nib.outgoing_frame_counter = 0;
}


void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  static zb_ushort_t first_time = 1;
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));

    if (!first_time)
    {
      TRACE_MSG(TRACE_APS1, "Second start ok - set bad outgoing frame cnt and send buffer test", (FMT__0));
#if 0
      zb_schedule_alarm(set_bad_frame_cnt, 0, SEND_TMOUT);
      send_packets(param);
#else
      zb_free_buf(buf);
#endif
    }
    else
    {
      TRACE_MSG(TRACE_APS1, "First start ok", (FMT__0));
      /* Give a time for the net to finish retransmits etc */
      zb_schedule_alarm(set_bad_key_seq, 0, SEND_TMOUT);
      first_time = 0;
      zb_free_buf(buf);
    }

  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}


void buffer_test_cb(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APS1, "buffer_test_cb %hd", (FMT__H, param));
  if (param == ZB_TP_BUFFER_TEST_OK)
  {
    TRACE_MSG(TRACE_APS1, "status OK", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_APS1, "status ERROR", (FMT__0));
  }
}


void send_packets(zb_uint8_t param)
{
  static zb_ushort_t cnt = 0;

  cnt++;
  if (cnt < 10)
  {
    zb_buf_t *buf = ZB_BUF_FROM_REF(param);
    zb_apsde_data_req_t *req;
    zb_uint8_t *ptr = NULL;
    zb_short_t i;

    ZB_BUF_INITIAL_ALLOC(buf, TEST_BUFFER_LEN, ptr);
    req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
    req->dst_addr.addr_short = 1;
    req->addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    req->tx_options = ZB_APSDE_TX_OPT_ACK_TX;
    req->radius = 1;
    req->profileid = 2;
    req->src_endpoint = 10;
    req->dst_endpoint = 10;
    buf->u.hdr.handle = 0x11;
    for (i = 0 ; i < TEST_BUFFER_LEN ; ++i)
    {
      ptr[i] = i % 32 + '0';
    }
    TRACE_MSG(TRACE_APS3, "Sending apsde_data.request", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
    zb_get_out_buf_delayed(send_packets);
  }
}


/*! @} */
