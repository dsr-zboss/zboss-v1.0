/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: TP/NWK/BV-15-I Network Broadcast to Coordinator
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

 /*00:22:01:00:00:6b:00:00  00:22:01:00:00:6c:00:00*/
zb_ieee_addr_t g_ieee_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_zr_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
zb_ieee_addr_t daintree_1 = {0x00, 0x00, 0x6B, 0x00, 0x00, 0x81, 0x22, 0x00};
zb_ieee_addr_t daintree_2 = {0x00, 0x00, 0x6C, 0x00, 0x00, 0x81, 0x22, 0x00};


#ifndef ZB_NS_BUILD
#define HW_SLEEP_ADDITIONAL 80*ZB_TIME_ONE_SECOND
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
  MAC_ADD_VISIBLE_LONG(daintree_1);  

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

void test_buffer_request(zb_uint8_t param)
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_uint16_t addr = *ZB_GET_BUF_PARAM(buf, zb_uint16_t);
  zb_buffer_test_req_param_t *req_param;

  TRACE_MSG(TRACE_APS1, "send_test_request to %d", (FMT__D, addr));
  req_param = ZB_GET_BUF_PARAM(buf, zb_buffer_test_req_param_t);
  req_param->len = 0x10;
  req_param->dst_addr = addr;

  zb_tp_buffer_test_request(ZB_REF_FROM_BUF(buf), buffer_test_cb);
}

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_ERROR, ">>zb_zdo_startup_complete status %d", (FMT__D, (int)buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    zb_buf_t *buf1;
    zb_buf_t *buf2;
    zb_buf_t *buf3;
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));

    buf1 = zb_get_out_buf();
    ZB_SET_BUF_PARAM(buf1, 0xffff, zb_uint16_t);
    ZB_SCHEDULE_ALARM(test_buffer_request, ZB_REF_FROM_BUF(buf1), 40 * ZB_TIME_ONE_SECOND + HW_SLEEP_ADDITIONAL);

    buf2 = zb_get_out_buf();
    ZB_SET_BUF_PARAM(buf2, 0xfffc, zb_uint16_t);
    ZB_SCHEDULE_ALARM(test_buffer_request, ZB_REF_FROM_BUF(buf2), 50 * ZB_TIME_ONE_SECOND + HW_SLEEP_ADDITIONAL);

    buf3 = zb_get_out_buf();
    ZB_SET_BUF_PARAM(buf3, 0xfffd, zb_uint16_t);
    ZB_SCHEDULE_ALARM(test_buffer_request, ZB_REF_FROM_BUF(buf3), 60 * ZB_TIME_ONE_SECOND + HW_SLEEP_ADDITIONAL);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device START FAILED", (FMT__0));
  }

  zb_free_buf(buf);
}
