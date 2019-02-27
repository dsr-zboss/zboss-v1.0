/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: TP/PRO/BV-38 Frequency Agility - Channel Changeover - ZR,
coordinator side
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

zb_ieee_addr_t g_ieee_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr_r = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr_r = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#endif

zb_int_t g_errors = 0;

#define TEST_BUFFER_LEN 30

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

  zb_set_default_ffd_descriptor_values(ZB_COORDINATOR);

  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;

  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);

#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
#endif


#ifdef ZB_SECURITY
  /* turn off security */
  ZB_NIB_SECURITY_LEVEL() = 0;
#endif

  ZB_NWK().max_children = 2;

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
    TRACE_MSG(TRACE_APS1, "Test finished, status OK", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_APS1, "Test finished, status ERROR", (FMT__0));
  }
}

void send_test_request(zb_uint8_t param)
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_buffer_test_req_param_t *req_param;

  TRACE_MSG(TRACE_APS1, "send_test_request %hd", (FMT__H, param));
  req_param = ZB_GET_BUF_PARAM(buf, zb_buffer_test_req_param_t);
  req_param->len = TEST_BUFFER_LEN;
  req_param->dst_addr = zb_address_short_by_ieee(g_ieee_addr_r);

  TRACE_MSG(TRACE_APS3, "dst addr %d", (FMT__D, req_param->dst_addr));

  zb_tp_buffer_test_request(param, buffer_test_cb);
}

void new_channel_cb(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APS1, "new_channel_cb, wait for %d sec and make send_test_request",
            (FMT__D, ZB_NWK_BROADCAST_DELIVERY_TIME()));
  ZB_SCHEDULE_ALARM(send_test_request, param, ZB_NWK_BROADCAST_DELIVERY_TIME() * ZB_TIME_ONE_SECOND);
}

void change_current_channel(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf;
  zb_zdo_mgmt_nwk_update_req_t *req;

  ZVUNUSED(param);

  TRACE_MSG(TRACE_APS1, "change_current_channel", (FMT__0));
  buf = zb_get_out_buf();
  if (!buf)
  {
    TRACE_MSG(TRACE_ERROR, "out buf alloc failed!", (FMT__0));
  }
  else
  {
    req = ZB_GET_BUF_PARAM(buf, zb_zdo_mgmt_nwk_update_req_t);

    req->hdr.scan_channels = ZB_MAC_GET_CURRENT_LOGICAL_CHANNEL() + 1;
    if (req->hdr.scan_channels > ZB_MAC_MAX_CHANNEL_NUMBER)
    {
      req->hdr.scan_channels = ZB_MAC_START_CHANNEL_NUMBER;
    }
    TRACE_MSG(TRACE_APS3, "new channel %ld %lx", (FMT__L_L, req->hdr.scan_channels, (1 << req->hdr.scan_channels)));
    req->hdr.scan_channels = 1l << req->hdr.scan_channels;
    req->hdr.scan_duration = ZB_ZDO_NEW_ACTIVE_CHANNEL;
    req->manager_addr = ZB_NIB_NWK_MANAGER_ADDR();
    req->update_id = ZB_NIB_UPDATE_ID();
    req->dst_addr = zb_address_short_by_ieee(g_ieee_addr_r);

    TRACE_MSG(TRACE_APS3, "dst addr %d", (FMT__D, req->dst_addr));
    zb_zdo_mgmt_nwk_update_req(ZB_REF_FROM_BUF(buf), new_channel_cb);
  }
}

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_APS1, "zb_zdo_startup_complete status %d", (FMT__D, (int)buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
    ZB_SCHEDULE_ALARM(change_current_channel, 0, ZB_TIME_ONE_SECOND * 40);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device START FAILED", (FMT__0));
  }

  zb_free_buf(buf);
}

