/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: broadcast blocking test
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

zb_ieee_addr_t g_ieee_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};

static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr);

#ifndef ZB_NS_BUILD
#define HW_SLEEP_ADDITIONAL 30
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
  MAC_PIB().mac_pan_id = 0x1aaa;
  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
  /* accept only one child */
  ZB_NWK().max_children = 1;


#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
  ZB_UPDATE_PAN_ID();
#endif

#ifdef ZB_SECURITY
  /* turn off security */
  ZB_NIB_SECURITY_LEVEL() = 0;
#endif

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


void send_buffertest1(zb_uint8_t param) ZB_CALLBACK
{
  /*
    5) DUT ZC sends a broadcast Buffer Test Request (secured, KEY0) to all
    members of the PAN
   */
  TRACE_MSG(TRACE_SECUR1, "buffer test 1", (FMT__0));
  zc_send_data(ZB_BUF_FROM_REF(param), ZB_NWK_BROADCAST_ALL_DEVICES);   
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

static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr)
{
  zb_buffer_test_req_param_t *req_param;

  TRACE_MSG(TRACE_APS1, "send_test_request to %d", (FMT__D, addr));
  req_param = ZB_GET_BUF_PARAM(buf, zb_buffer_test_req_param_t);
  req_param->len = 70;
  req_param->dst_addr = addr;

  zb_tp_buffer_test_request(ZB_REF_FROM_BUF(buf), buffer_test_cb);
}






void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);

  TRACE_MSG(TRACE_ERROR, ">>zb_zdo_startup_complete status %d", (FMT__D, (int)buf->u.hdr.status));

  if (buf->u.hdr.status == RET_OK)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
	zb_schedule_alarm(send_buffertest1, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(20000));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device START FAILED", (FMT__0));
  }

  zb_free_buf(buf);
}


