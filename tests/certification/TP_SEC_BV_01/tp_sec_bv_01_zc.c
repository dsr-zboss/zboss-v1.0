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
//zb_uint8_t g_key2[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};
zb_uint8_t g_key2[16] = { 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0, 0, 0, 0, 0, 0, 0, 0};
zb_uint8_t g_key3[16] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0, 0, 0, 0, 0, 0, 0, 0};
/*
  Keys in Wireshark:

  0000:0000:0000:0000:8967:4523:01ef:cdab

  0000:0000:0000:0000:8070:6050:4030:2010

  0000:0000:0000:0000:8877:6655:4433:2211

 */

#ifndef ZB_NS_BUILD
#define HW_SLEEP_ADDITIONAL 10*ZB_TIME_ONE_SECOND
#else
#define HW_SLEEP_ADDITIONAL 0
#endif




static zb_bool_t aps_secure = 0;


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

//  aps_secure = 1;

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_zc", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zc", "1", "1");
#endif

  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;
#ifdef ZB_SECURITY
  if (aps_secure)
  {
    ZG->nwk.nib.secure_all_frames = 0;
  }
#endif
  zb_secur_setup_preconfigured_key(g_key, 0);
  zb_secur_setup_preconfigured_key(g_key2, 1);
  zb_secur_setup_preconfigured_key(g_key3, 2);
//  secur_nwk_key_switch(1);
//  ZG->nwk.nib.active_key_seq_number = 1;
//  ZG->nwk.nib.active_secur_material_i = 1;

  /* assign our address */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_zc_addr);
  MAC_ADD_VISIBLE_LONG(g_zr1_addr);  
  MAC_ADD_INVISIBLE_SHORT(2);
  MAC_ADD_INVISIBLE_SHORT(3);

  MAC_PIB().mac_pan_id = 0x1aaa;

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


void zc_alarm2(zb_uint8_t param) ZB_CALLBACK;
void zc_alarm3(zb_uint8_t param) ZB_CALLBACK;
void zc_alarm4(zb_uint8_t param) ZB_CALLBACK;
void zc_alarm5(zb_uint8_t param) ZB_CALLBACK;
void zc_alarm6(zb_uint8_t param) ZB_CALLBACK;
void zc_alarm7(zb_uint8_t param) ZB_CALLBACK;
void send_buffertest1(zb_uint8_t param) ZB_CALLBACK;
void transport_key1(zb_uint8_t param) ZB_CALLBACK;
void switch_key1(zb_uint8_t param) ZB_CALLBACK;
void send_buffertest2(zb_uint8_t param) ZB_CALLBACK;
void transport_key2(zb_uint8_t param) ZB_CALLBACK;
void switch_key2(zb_uint8_t param) ZB_CALLBACK;
void send_buffertest4(zb_uint8_t param) ZB_CALLBACK;

void send_buffertest1(zb_uint8_t param) ZB_CALLBACK
{
  /*
    5) DUT ZC sends a broadcast Buffer Test Request (secured, KEY0) to all
    members of the PAN
   */
  TRACE_MSG(TRACE_SECUR1, "buffer test 1", (FMT__0));
  zc_send_data(ZB_BUF_FROM_REF(param), ZB_NWK_BROADCAST_ALL_DEVICES);
  zb_schedule_alarm(zc_alarm2, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(4000) + HW_SLEEP_ADDITIONAL);
}


void zc_alarm2(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  zb_get_out_buf_delayed(transport_key1);
}

void transport_key1(zb_uint8_t param) ZB_CALLBACK
{
  /*
    9) DUT ZC issues a Key Transport with KEY1 (secured, KEY0) command to each
    of DUT ZR1, DUT ZR2, gZED1 and gZR3 via network broadcast.
  */
  TRACE_MSG(TRACE_SECUR1, "transport key1 - broadcast", (FMT__0));
  *ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_uint16_t) = ZB_NWK_BROADCAST_ALL_DEVICES;
  ZB_SCHEDULE_CALLBACK(zb_secur_send_nwk_key_update_br, param);
  zb_schedule_alarm(zc_alarm3, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(6000)+2*HW_SLEEP_ADDITIONAL);
}



void zc_alarm3(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  zb_get_out_buf_delayed(switch_key1);
}

void switch_key1(zb_uint8_t param) ZB_CALLBACK
{
  /*
    11) DUT ZC issues a Key Switch command for KEY1 to DUT ZR1, DUT ZR2,
    and gZR3 via fffd broadcast
  */
  TRACE_MSG(TRACE_SECUR1, "switch to key1 - broadcast rx-on-when-idle", (FMT__0));
  *ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_uint16_t) = ZB_NWK_BROADCAST_RX_ON_WHEN_IDLE;
  ZB_SCHEDULE_CALLBACK(zb_secur_send_nwk_key_switch, param);
  zb_schedule_alarm(zc_alarm4, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(6000) + 2*HW_SLEEP_ADDITIONAL);
}


void zc_alarm4(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  zb_get_out_buf_delayed(send_buffertest2);
}

void send_buffertest2(zb_uint8_t param) ZB_CALLBACK
{
  /*
    12) DUT ZC sends a broadcast Buffer Test Request (secured, KEY1) to all
    members of the PAN.
  */
  TRACE_MSG(TRACE_SECUR1, "buffer test 2", (FMT__0));
  zc_send_data(ZB_BUF_FROM_REF(param), ZB_NWK_BROADCAST_ALL_DEVICES);
  zb_schedule_alarm(zc_alarm5, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(6000)+2*HW_SLEEP_ADDITIONAL);
}


void zc_alarm5(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  zb_get_out_buf_delayed(transport_key2);
}

void transport_key2(zb_uint8_t param) ZB_CALLBACK
{
  /*
    16) DUT ZC issues a Key Transport with KEY2 (secured, KEY1) command to
    each of DUT ZR1, DUT ZR2, and gZR3 visa fffd broadcast.
  */
  TRACE_MSG(TRACE_SECUR1, "transport key2 - broadcast rx-on-when-idle", (FMT__0));
  *ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_uint16_t) = ZB_NWK_BROADCAST_RX_ON_WHEN_IDLE;
  ZB_SCHEDULE_CALLBACK(zb_secur_send_nwk_key_update_br, param);
  zb_schedule_alarm(zc_alarm6, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(6000)+2*HW_SLEEP_ADDITIONAL);
}


void zc_alarm6(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  zb_get_out_buf_delayed(switch_key2);
}

void switch_key2(zb_uint8_t param) ZB_CALLBACK
{
  /*
    17) DUT ZC issues a Key Switch command for KEY2 by unicast to each of DUT
    ZR1, DUT ZR2, and gZR3 using KEY2.
  */
  TRACE_MSG(TRACE_SECUR1, "switch to key2 - broadcast rx-on-when-idle", (FMT__0));
  *ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_uint16_t) = ZB_NWK_BROADCAST_RX_ON_WHEN_IDLE;
  ZB_SCHEDULE_CALLBACK(zb_secur_send_nwk_key_switch, param);
  zb_schedule_alarm(zc_alarm7, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(6000)+2*HW_SLEEP_ADDITIONAL);
}


void zc_alarm7(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  zb_get_out_buf_delayed(send_buffertest4);
}

void send_buffertest4(zb_uint8_t param) ZB_CALLBACK
{
  /*
    18) DUT ZC sends a broadcast Buffer Test Request (secured, KEY2) to all
    members of the PAN
   */
  TRACE_MSG(TRACE_SECUR1, "buffer test 3", (FMT__0));
  zc_send_data(ZB_BUF_FROM_REF(param), ZB_NWK_BROADCAST_ALL_DEVICES);
}



void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_APS3, ">>zb_zdo_startup_complete status %hd", (FMT__H, buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));

    zb_schedule_alarm(send_buffertest1, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(60000) + 15*HW_SLEEP_ADDITIONAL);
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
