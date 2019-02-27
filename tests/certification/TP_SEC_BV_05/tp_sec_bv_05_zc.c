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
#include "zb_secur_api.h"
#include "zb_secur.h"

#define TEST_BUFFER_LEN 70

zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_zr1_addr = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};

zb_uint8_t g_key[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};

#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_zr2_ieee_addr = {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_zed2_ieee_addr = {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
zb_ieee_addr_t g_zed3_ieee_addr = {0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03};
#else
zb_ieee_addr_t g_zr2_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_zed2_ieee_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//zb_ieee_addr_t g_zed2_ieee_addr = {0x00, 0x00, 0x6C, 0x00, 0x00, 0x81, 0x22, 0x00};
zb_ieee_addr_t g_zed3_ieee_addr = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
zb_ieee_addr_t daintree_1 = {0x00, 0x00, 0x6B, 0x00, 0x00, 0x81, 0x22, 0x00};
zb_ieee_addr_t daintree_2 = {0x00, 0x00, 0x6C, 0x00, 0x00, 0x81, 0x22, 0x00};
#endif

zb_uint16_t g_zed2_addr;

/*
  Keys in Wireshark:

  0000:0000:0000:0000:8967:4523:01ef:cdab

 */


/*! \addtogroup ZB_TESTS */
/*! @{ */

#ifndef ZB_COORDINATOR_ROLE
#error Coordinator role is not compiled!
#endif


/*
  The test is: ZC starts PAN, ZR joins to it by association and send APS data packet, when ZC
  received packet, it sends packet to ZR, when ZR received packet, it sends
  packet to ZC etc.
 */

#ifndef APS_RETRANSMIT_TEST
static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr);
#endif

void data_indication(zb_uint8_t param) ZB_CALLBACK;

#define HW_SLEEP_ADDITIONAL 10*ZB_TIME_ONE_SECOND

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
  /* fix key to be able to decrypt by Wireshark */
  zb_secur_setup_preconfigured_key(g_key, 0);

  /* assign our address */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_zc_addr);
  MAC_PIB().mac_pan_id = 0x1aaa;

  MAC_ADD_VISIBLE_LONG(g_zr1_addr);
  MAC_ADD_VISIBLE_LONG(daintree_1);  
  MAC_ADD_INVISIBLE_SHORT(2);
  MAC_ADD_INVISIBLE_SHORT(0x287);
  MAC_ADD_INVISIBLE_SHORT(0x288);
  MAC_ADD_INVISIBLE_SHORT(0x289);


  /* allow only one join, then let's ZR1 join ZR2, then ZR2 join all others */
  ZG->nwk.nib.max_children = 1;

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



/*
  Test procedure:

  ZR1 joins ZC
  ZR2 joins ZR1
  ZED1 joins ZR2
  ZED2 joins ZR2
  ZC sends broadcast buffer test request
  ZC removes ZED2 using APS-REMOVE
  ZC sends unicast buffer test request to ZED2
  ZED3 joins ZR2
  ZC sends unicast buffer test request to ZED3
 */

void zc_alarm1(zb_uint8_t param) ZB_CALLBACK;
void aps_remove_zed2(zb_uint8_t param) ZB_CALLBACK;
void zc_alarm2(zb_uint8_t param) ZB_CALLBACK;
void send_buffertest2(zb_uint8_t param) ZB_CALLBACK;
void zc_alarm3(zb_uint8_t param) ZB_CALLBACK;
void send_buffertest3(zb_uint8_t param) ZB_CALLBACK;
void buffer_test_cb(zb_uint8_t param);
static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr);


void send_buffertest1(zb_uint8_t param) ZB_CALLBACK
{
  /*
    ZC sends broadcast buffer test request
   */
  TRACE_MSG(TRACE_SECUR1, "buffer test 1 (broadcast)", (FMT__0));
  zc_send_data(ZB_BUF_FROM_REF(param), ZB_NWK_BROADCAST_ALL_DEVICES);
  zb_schedule_alarm(zc_alarm1, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(2000)+2*HW_SLEEP_ADDITIONAL);
}


void zc_alarm1(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  zb_get_out_buf_delayed(aps_remove_zed2);
}

void aps_remove_zed2(zb_uint8_t param) ZB_CALLBACK
{
  /*
    ZC removes ZED2 using APS-REMOVE
  */
  TRACE_MSG(TRACE_SECUR1, "APS remove ZED2", (FMT__0));

  /* APS remove here! Get ZED2 address by long addr. Remember it: need to send
   * buffer test request. */

  g_zed2_addr = zb_address_short_by_ieee(g_zed2_ieee_addr);
  TRACE_MSG(TRACE_COMMON2, "ZED2 addr 0x%x", (FMT__D, g_zed2_addr));

  {
    zb_apsme_remove_device_req_t *req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_apsme_remove_device_req_t);

    ZB_IEEE_ADDR_COPY(req->parent_address, g_zr2_ieee_addr); /* ZR2 addr */
    ZB_IEEE_ADDR_COPY(req->child_address, g_zed2_ieee_addr); /* ZED2 addr */

    ZB_SCHEDULE_CALLBACK(zb_secur_apsme_remove_device, param);
  }

  zb_schedule_alarm(zc_alarm2, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(2000) + 2*HW_SLEEP_ADDITIONAL);
}


void zc_alarm2(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  zb_get_out_buf_delayed(send_buffertest2);
}


void send_buffertest2(zb_uint8_t param) ZB_CALLBACK
{
  /*
    ZC sends unicast buffer test request to ZED2
  */
  TRACE_MSG(TRACE_SECUR1, "buffer test 2 - to ZED2 - must fail", (FMT__0));
  zc_send_data(ZB_BUF_FROM_REF(param), g_zed2_addr);
  zb_schedule_alarm(zc_alarm3, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(20000) + HW_SLEEP_ADDITIONAL);
}


void zc_alarm3(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  zb_get_out_buf_delayed(send_buffertest3);
}


void send_buffertest3(zb_uint8_t param) ZB_CALLBACK
{
  /*
    ZC sends unicast buffer test request to ZED3
  */
  TRACE_MSG(TRACE_SECUR1, "buffer test 3 - to ZED3", (FMT__0));
  zc_send_data(ZB_BUF_FROM_REF(param), zb_address_short_by_ieee(g_zed3_ieee_addr));
}


void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_APS3, ">>zb_zdo_startup_complete status %hd", (FMT__H, buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
    zb_schedule_alarm(send_buffertest1, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(60000) + 5*HW_SLEEP_ADDITIONAL);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %hd", (FMT__H, buf->u.hdr.status));
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
