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
#define SEND_TMOUT1 ZB_MILLISECONDS_TO_BEACON_INTERVAL(8000)
#define SEND_TMOUT2 ZB_MILLISECONDS_TO_BEACON_INTERVAL(2000)
#define SEND_TMOUT3 ZB_MILLISECONDS_TO_BEACON_INTERVAL(5000)
#else 
#define SEND_TMOUT1 ZB_MILLISECONDS_TO_BEACON_INTERVAL(38000)
#define SEND_TMOUT2 ZB_MILLISECONDS_TO_BEACON_INTERVAL(44000)
#define SEND_TMOUT3 ZB_MILLISECONDS_TO_BEACON_INTERVAL(51000)

#endif

#ifndef ZB_ROUTER_ROLE
#error Router role is not compiled!
#endif

/* For NS build first ieee addr byte should be unique */
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
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
  ZB_INIT("zdo_zr1", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zr1", "2", "2");
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


static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr);
void send_buffertest1(zb_uint8_t param) ZB_CALLBACK;
void send_buffertest2(zb_uint8_t param) ZB_CALLBACK;
void zc_alarm2(zb_uint8_t param) ZB_CALLBACK;
void zc_alarm3(zb_uint8_t param) ZB_CALLBACK;
void send_buffertest3(zb_uint8_t param) ZB_CALLBACK;

void send_buffertest1(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_SECUR1, "buffer test 1", (FMT__0));
  zc_send_data(ZB_BUF_FROM_REF(param), ZB_NWK_BROADCAST_ALL_DEVICES);
  zb_schedule_alarm(zc_alarm2, 0, SEND_TMOUT1);
}


void zc_alarm2(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  zb_get_out_buf_delayed(send_buffertest2);
}


void send_buffertest2(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_SECUR1, "buffer test 2", (FMT__0));
  zc_send_data(ZB_BUF_FROM_REF(param), ZB_NWK_BROADCAST_ALL_DEVICES);
  zb_schedule_alarm(zc_alarm3, 0, SEND_TMOUT2);
}


void zc_alarm3(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  zb_get_out_buf_delayed(send_buffertest3);
}


void send_buffertest3(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_SECUR1, "buffer test 3", (FMT__0));
  zc_send_data(ZB_BUF_FROM_REF(param), ZB_NWK_BROADCAST_ALL_DEVICES);
}



void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
    zb_schedule_alarm(send_buffertest1, param, SEND_TMOUT1);
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


static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr)
{
  zb_buffer_test_req_param_t *req_param;

  TRACE_MSG(TRACE_APS1, "send_test_request to %d", (FMT__D, addr));
  req_param = ZB_GET_BUF_PARAM(buf, zb_buffer_test_req_param_t);
  req_param->len = TEST_BUFFER_LEN;
  req_param->dst_addr = addr;

  zb_tp_buffer_test_request(ZB_REF_FROM_BUF(buf), buffer_test_cb);
}



/*! @} */
