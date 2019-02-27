/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: 14.12 TP/ZDO/BV-06 ZC-ZDO-Transmit
Bind/Unbind_req. Coordinator side
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "tp_zdo_bv_06.h"

zb_ieee_addr_t g_ieee_addr = TEST_IEEE_ADDR_C;
zb_ieee_addr_t g_ieee_addr_ed1 = TEST_IEEE_ADDR_ED1;
zb_ieee_addr_t g_ieee_addr_ed2 = TEST_IEEE_ADDR_ED2;

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
  ZB_INIT("tp_zdo_bv_06_DUTZC", argv[1], argv[2]);
#else
  ZB_INIT("tp_zdo_bv_06_DUTZC", "1", "1");
#endif

  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;

  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), g_ieee_addr);
  MAC_PIB().mac_pan_id = 0x1aaa;
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

void unbind_device1_cb(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_zdo_bind_resp_t *bind_resp = (zb_zdo_bind_resp_t*)ZB_BUF_BEGIN(buf);

  TRACE_MSG(TRACE_COMMON1, "unbind_device1_cb resp status %hd", (FMT__H, bind_resp->status));
  if (bind_resp->status != ZB_ZDP_STATUS_SUCCESS)
  {
    TRACE_MSG(TRACE_COMMON1, "Error bind device 1. Test status failed", (FMT__0));
  }
  zb_free_buf(buf);

}

void unbind_device_1(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_zdo_bind_req_param_t *bind_param;

  TRACE_MSG(TRACE_COMMON1, "unbind_device_1", (FMT__0));

  zb_buf_initial_alloc(buf, 0);
  bind_param = ZB_GET_BUF_PARAM(buf, zb_zdo_bind_req_param_t);
  ZB_MEMCPY(bind_param->src_address, g_ieee_addr_ed1, sizeof(zb_ieee_addr_t));
  bind_param->src_endp = TEST_ED1_EP;
  bind_param->cluster_id = TP_BUFFER_TEST_REQUEST_CLID;
  bind_param->dst_addr_mode = ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
  ZB_MEMCPY(bind_param->dst_address.addr_long, g_ieee_addr_ed2, sizeof(zb_ieee_addr_t));
  bind_param->dst_endp = TEST_ED2_EP;
  bind_param->req_dst_addr = zb_address_short_by_ieee(g_ieee_addr_ed1);
  TRACE_MSG(TRACE_COMMON1, "dst addr %d", (FMT__D, bind_param->req_dst_addr));

  zb_zdo_unbind_req(param, unbind_device1_cb);
}

void bind_device2_cb(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_zdo_bind_resp_t *bind_resp = (zb_zdo_bind_resp_t*)ZB_BUF_BEGIN(buf);

  TRACE_MSG(TRACE_COMMON1, "bind_device2_cb resp status %hd", (FMT__H, bind_resp->status));
  if (bind_resp->status != ZB_ZDP_STATUS_SUCCESS)
  {
    TRACE_MSG(TRACE_COMMON1, "Error bind device 2. Test status failed", (FMT__0));
  }
  ZB_SCHEDULE_ALARM(unbind_device_1, param, 40 * ZB_TIME_ONE_SECOND);
}

void bind_device1_cb(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_zdo_bind_req_param_t *bind_param;
  zb_zdo_bind_resp_t *bind_resp = (zb_zdo_bind_resp_t*)ZB_BUF_BEGIN(buf);

  TRACE_MSG(TRACE_COMMON1, "bind_device1_cb resp status %hd", (FMT__H, bind_resp->status));
  if (bind_resp->status != ZB_ZDP_STATUS_SUCCESS)
  {
    TRACE_MSG(TRACE_COMMON1, "Error bind device 1. Test status failed", (FMT__0));
  }
  else
  {
    bind_param = ZB_GET_BUF_PARAM(buf, zb_zdo_bind_req_param_t);
    ZB_MEMCPY(bind_param->src_address, g_ieee_addr_ed2, sizeof(zb_ieee_addr_t));
    bind_param->src_endp = TEST_ED2_EP;
    bind_param->cluster_id = 0x54;
    bind_param->dst_addr_mode = ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    ZB_MEMCPY(bind_param->dst_address.addr_long, g_ieee_addr_ed1, sizeof(zb_ieee_addr_t));
    bind_param->dst_endp = TEST_ED1_EP;
    bind_param->req_dst_addr = zb_address_short_by_ieee(g_ieee_addr_ed2);
    TRACE_MSG(TRACE_COMMON1, "dst addr %d", (FMT__D, bind_param->req_dst_addr));

    zb_zdo_bind_req(param, bind_device2_cb);
  }
}

void bind_device1(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf;
  zb_zdo_bind_req_param_t *bind_param;

  TRACE_MSG(TRACE_COMMON1, "bind_device1", (FMT__0));

  buf = zb_get_out_buf();
  if (!buf)
  {
    TRACE_MSG(TRACE_ERROR, "out buf alloc failed!", (FMT__0));
  }
  else
  {
    param = ZB_REF_FROM_BUF(buf);
    bind_param = ZB_GET_BUF_PARAM(buf, zb_zdo_bind_req_param_t);
    ZB_MEMCPY(bind_param->src_address, g_ieee_addr_ed1, sizeof(zb_ieee_addr_t));
    bind_param->src_endp = TEST_ED1_EP;
    bind_param->cluster_id = TP_BUFFER_TEST_REQUEST_CLID;
    bind_param->dst_addr_mode = ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    ZB_MEMCPY(bind_param->dst_address.addr_long, g_ieee_addr_ed2, sizeof(zb_ieee_addr_t));
    bind_param->dst_endp = TEST_ED2_EP;
    bind_param->req_dst_addr = zb_address_short_by_ieee(g_ieee_addr_ed1);
    TRACE_MSG(TRACE_COMMON1, "dst addr %d", (FMT__D, bind_param->req_dst_addr));

    zb_zdo_bind_req(param, bind_device1_cb);
  }
}

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_ERROR, ">>zb_zdo_startup_complete status %d", (FMT__D, (int)buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
    ZB_SCHEDULE_ALARM(bind_device1, param, 30 * ZB_TIME_ONE_SECOND);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device START FAILED", (FMT__0));
  }

  zb_free_buf(buf);
}
