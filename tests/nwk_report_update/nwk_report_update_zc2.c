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

zb_ieee_addr_t g_ieee_addr = {0x03, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb};


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
  ZB_INIT("zdo_zc2", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zc2", "2", "2");
#endif
#ifdef ZB_SECURITY
  /* switch security off */
  ZG->nwk.nib.security_level = 0;
#endif

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
  MAC_PIB().mac_pan_id = 0x1bbb;
  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;

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


void change_panid(zb_uint8_t param) ZB_CALLBACK;

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    change_panid(param);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}


void change_panid(zb_uint8_t param) ZB_CALLBACK
{
  zb_mlme_start_req_t * req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_start_req_t);

  TRACE_MSG(TRACE_NWK1, "Change PANID", (FMT__0));

  ZB_MEMSET(req, 0, sizeof(*req));
  req->pan_id = 0x1aaa;
  req->logical_channel = MAC_CTX().current_channel;
  req->channel_page = 0;
  req->pan_coordinator = (ZB_NIB_DEVICE_TYPE() == ZB_NWK_DEVICE_TYPE_COORDINATOR);
  req->coord_realignment = 0;
  req->beacon_order = ZB_TURN_OFF_ORDER;
  req->superframe_order = ZB_TURN_OFF_ORDER;

  ZG->nwk.handle.state = ZB_NLME_STATE_PANID_CONFLICT_RESOLUTION;
  ZB_SCHEDULE_CALLBACK(zb_mlme_start_request, param);

  TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
}


/*! @} */
