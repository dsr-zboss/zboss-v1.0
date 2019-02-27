/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Test for ZR application written using ZDO.
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr = {0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#endif
zb_ieee_addr_t g_ext_pan_id = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

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

  /* FIXME: APS secure is off inside stack, lets use NWK secure */
#if 0
  aps_secure = 1;
#endif

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("tp_pro_bv_30_DUTZR1", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zr", "2", "2");
#endif

  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 0;
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
  /* we need this for rejoin */	\
  ZB_IEEE_ADDR_COPY(ZB_AIB().aps_use_extended_pan_id, &g_ext_pan_id);
  MAC_PIB().mac_pan_id = 0x1aaa;
  ZG->nwk.nib.max_children = 2;
#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
  ZB_UPDATE_PAN_ID();
#endif

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
  TRACE_MSG(TRACE_APS3, ">>zb_zdo_startup_complete status %d", (FMT__D, (int)buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTE FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
  }
  zb_free_buf(buf);
}
#if 0
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
#endif

