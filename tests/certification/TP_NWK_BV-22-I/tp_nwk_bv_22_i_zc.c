/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: TP/NWK/BV-22-I Buffering for Sleeping Children
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

zb_ieee_addr_t g_ieee_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_zr_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_ed_ieee_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#ifndef ZB_NS_BUILD
#define HW_SLEEP_ADDITIONAL 60*ZB_TIME_ONE_SECOND
#else
#define HW_SLEEP_ADDITIONAL 0
#endif


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
  ZB_INIT("zdo_zc", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zc", "1", "1");
#endif

  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;

  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
  MAC_PIB().mac_pan_id = 0x1aaa;
#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
#endif

#ifdef ZB_SECURITY
  /* turn off security */
  ZB_NIB_SECURITY_LEVEL() = 0;
#endif

  MAC_ADD_VISIBLE_LONG(g_zr_addr);

  if ( zdo_dev_start() != RET_OK )
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


void packets_sent_cb(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  TRACE_MSG(TRACE_APS3, "packets_sent_cb", (FMT__0));
}

void start_packet_send(zb_uint8_t param) ZB_CALLBACK
{
  zb_tp_transmit_counted_packets_param_t *params;
  zb_buf_t *asdu;

/*  ZVUNUSED(param);*/

  TRACE_MSG(TRACE_APS3, "start_packet_send", (FMT__0));

  asdu = zb_get_out_buf();

  params = ZB_GET_BUF_PARAM(asdu, zb_tp_transmit_counted_packets_param_t);
  params->len = param;
  params->packets_number = 1;
  params->idle_time = 10;
  params->dst_addr = zb_address_short_by_ieee(g_ed_ieee_addr);

  TRACE_MSG(TRACE_APS3, "dst addr %d", (FMT__D, params->dst_addr));
  zb_tp_transmit_counted_packets_req(ZB_REF_FROM_BUF(asdu), packets_sent_cb);
}


void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_ERROR, ">>zb_zdo_startup_complete status %d", (FMT__D, (int)buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));

    ZB_SCHEDULE_ALARM(start_packet_send, 0x0A, 20*ZB_TIME_ONE_SECOND + HW_SLEEP_ADDITIONAL);

    ZB_SCHEDULE_ALARM(start_packet_send, 0x20, 20*ZB_TIME_ONE_SECOND + HW_SLEEP_ADDITIONAL + ZB_ZDO_INDIRECT_POLL_TIMER + ZB_TIME_ONE_SECOND);
    ZB_SCHEDULE_ALARM(start_packet_send, 0x20, 20*ZB_TIME_ONE_SECOND + HW_SLEEP_ADDITIONAL + ZB_ZDO_INDIRECT_POLL_TIMER + ZB_TIME_ONE_SECOND);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device START FAILED", (FMT__0));
  }

  zb_free_buf(buf);
}
