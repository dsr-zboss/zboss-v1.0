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

/*! \addtogroup ZB_TESTS */
/*! @{ */

#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr_c   = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_ieee_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_ieee_addr_r1 = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr_c   = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_ieee_addr_r1 = {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
#endif

void zb_leave_req(zb_uint8_t param) ZB_CALLBACK;
void zb_nwk_leave_req(zb_uint8_t param) ZB_CALLBACK;
void zb_leave_callback(zb_uint8_t param) ZB_CALLBACK;

void zb_start_join(zb_uint8_t param) ZB_CALLBACK;
zb_ret_t initiate_rejoin1(zb_buf_t *buf);

zb_bool_t flag;

MAIN()
{
  ARGV_UNUSED;
  flag = ZB_TRUE;
#ifndef KEIL
  if ( argc < 3 )
  {
    printf("%s <read pipe path> <write pipe path>\n", argv[0]);
    return 0;
  }
#endif

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_start_zr2", argv[1], argv[2]);
#else
  ZB_INIT("zdo_start_zr2", "1", "1");
#endif

  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);

  /* join as a router */
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ROUTER;

#ifdef ZB_SECURITY
  /* turn off security */
  ZB_NIB_SECURITY_LEVEL() = 0;
#endif

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

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
    if (flag)
    {
      zb_nwk_leave_req(param);
      ZB_SCHEDULE_ALARM(zb_start_join, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(20000));
      flag = ZB_FALSE;
    }
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}

void zb_nwk_leave_req(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_nlme_leave_request_t *r = NULL;

  TRACE_MSG(TRACE_ERROR, "zb_nwk_leave_req", (FMT__0));

  r = ZB_GET_BUF_PARAM(buf, zb_nlme_leave_request_t);
  ZB_64BIT_ADDR_ZERO(r->device_address);
  r->remove_children = ZB_FALSE;
  r->rejoin = ZB_FALSE;
  ZB_SCHEDULE_CALLBACK(zb_nlme_leave_request, param);
}

zb_ret_t initiate_rejoin1(zb_buf_t *buf)
{
  zb_nlme_join_request_t *req = ZB_GET_BUF_PARAM(buf, zb_nlme_join_request_t);

  ZB_MEMSET(req, 0, sizeof(*req));

  ZB_EXTPANID_COPY(req->extended_pan_id, ZB_AIB().aps_use_extended_pan_id);
#ifdef ZB_ROUTER_ROLE
  if (ZB_NIB_DEVICE_TYPE() == ZB_NWK_DEVICE_TYPE_NONE)
  {
    ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ROUTER;
    ZB_MAC_CAP_SET_ROUTER_CAPS(req->capability_information);
    TRACE_MSG(TRACE_APS1, "Rejoin to pan " TRACE_FORMAT_64 " as ZR", (FMT__A, TRACE_ARG_64(ZB_AIB().aps_use_extended_pan_id)));
  }
  else
#endif
  {
    TRACE_MSG(TRACE_APS1, "Rejoin to pan " TRACE_FORMAT_64 " as ZE", (FMT__A, TRACE_ARG_64(ZB_AIB().aps_use_extended_pan_id)));
    if (MAC_PIB().mac_rx_on_when_idle)
    {
      ZB_MAC_CAP_SET_RX_ON_WHEN_IDLE(req->capability_information, 1);
    }
  }
  ZB_MAC_CAP_SET_ALLOCATE_ADDRESS(req->capability_information, 1);
  req->rejoin_network = ZB_NLME_REJOIN_METHOD_REJOIN;
  req->scan_channels = ZB_AIB().aps_channel_mask;
  req->scan_duration = ZB_DEFAULT_SCAN_DURATION;
  ZG->zdo.handle.rejoin = 1;
  ZG->nwk.handle.joined = 0;

  return ZB_SCHEDULE_CALLBACK(zb_nlme_join_request, ZB_REF_FROM_BUF(buf));
}

void zb_start_join(zb_uint8_t param) ZB_CALLBACK
{
  zb_ret_t ret = RET_OK;

  ZVUNUSED(param);
  TRACE_MSG(TRACE_ERROR, "rejoin", (FMT__0));

  ZDO_CTX().zdo_ctx.discovery_ctx.disc_count = ZDO_CTX().conf_attr.nwk_scan_attempts;;
  ZG->zdo.handle.started = 0;
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);

  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ROUTER;

#ifdef ZB_SECURITY
  ZB_NIB_SECURITY_LEVEL() = 0;
#endif
  if (!ZB_EXTPANID_IS_ZERO(ZB_AIB().aps_use_extended_pan_id))
  {
    zb_buf_t *buf = zb_get_out_buf();

    ret = initiate_rejoin1(buf);
  }
  else
  {
    zb_buf_t *buf = zb_get_out_buf();
    zb_nlme_network_discovery_request_t *req = ZB_GET_BUF_PARAM(buf, zb_nlme_network_discovery_request_t);
#ifdef ZB_ROUTER_ROLE
    if (ZB_NIB_DEVICE_TYPE() == ZB_NWK_DEVICE_TYPE_NONE)
    {
      ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ROUTER;
    }
#endif
    req->scan_channels = ZB_AIB().aps_channel_mask;
    req->scan_duration = ZB_DEFAULT_SCAN_DURATION;
    TRACE_MSG(TRACE_APS1, "disc, then join by assoc", (FMT__0));
    ret = ZB_SCHEDULE_CALLBACK(zb_nlme_network_discovery_request, ZB_REF_FROM_BUF(buf));
  }
}

/*! @} */

