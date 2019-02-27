/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: nwk reset test
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

zb_ieee_addr_t g_ieee_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

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
  ZB_INIT("zdo_zed", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zed", "2", "2");
#endif

  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
#endif

#ifdef ZB_SECURITY
  /* turn off security */
  ZB_NIB_SECURITY_LEVEL() = 0;
#endif

  /* become an ED */
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ED;
  ZB_PIB_RX_ON_WHEN_IDLE() = ZB_TRUE;

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

static void reset_confirm_callback(zb_uint8_t param)
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_nlme_reset_confirm_t *confirm = (zb_nlme_reset_confirm_t *)ZB_BUF_BEGIN(buf);

  TRACE_MSG(TRACE_ERROR, "RESET CALLBACK status %hd", (FMT__H, confirm->status));

  zb_free_buf(buf);
}


static void leave_callback(zb_uint8_t param)
{
  zb_uint8_t *ret = (zb_uint8_t *)ZB_BUF_BEGIN(ZB_BUF_FROM_REF(param));

  TRACE_MSG(TRACE_ERROR, "LEAVE CALLBACK status %hd", (FMT__H, *ret));
  zb_zdo_reset(param, ZB_FALSE, &reset_confirm_callback);
}

static void zb_leave_req(zb_uint8_t param)
{
  zb_ret_t ret = RET_OK;
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_zdo_mgmt_leave_param_t *req = NULL;

  ZVUNUSED(param);
  TRACE_MSG(TRACE_ERROR, "zb_leave_req_test", (FMT__0));
  req = ZB_GET_BUF_PARAM(buf, zb_zdo_mgmt_leave_param_t);

  ZB_MEMCPY(req->device_address, g_ieee_addr, sizeof(g_ieee_addr));
  req->remove_children = ZB_FALSE;
  req->rejoin = ZB_FALSE;
  {
    req->dst_addr = 0;
  }

  if (ret == RET_OK)
  {
    zdo_mgmt_leave_req(ZB_REF_FROM_BUF(buf), leave_callback);
  }
}


void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
    zb_leave_req(param);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}
