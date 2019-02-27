/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: TP/ZDO/BV-01: ZC-ZDO-Receive Device Discovery
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

zb_ieee_addr_t g_c_ieee_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_ieee_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void get_peer_short_addr(zb_uint8_t param) ZB_CALLBACK;
void get_ieee_addr(zb_uint8_t param) ZB_CALLBACK;

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


void get_ieee_addr_cb2(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_ERROR, "All requests are sent", (FMT__0));
  zb_free_buf(ZB_BUF_FROM_REF(param));
}

void get_ieee_addr_cb(zb_uint8_t param) ZB_CALLBACK
{
  zb_schedule_alarm(get_ieee_addr, 2, 10*ZB_TIME_ONE_SECOND);
  zb_free_buf(ZB_BUF_FROM_REF(param));
}

void get_ieee_addr(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = zb_get_out_buf();
  zb_zdo_ieee_addr_req_t *req_param;

  ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_zdo_ieee_addr_req_t), req_param);

  req_param->nwk_addr = 0x00;
  req_param->start_index = 0;
  if ( param == 1 )
  {
    req_param->request_type = ZB_ZDO_SINGLE_DEVICE_RESP;
    zb_zdo_ieee_addr_req(ZB_REF_FROM_BUF(buf), get_ieee_addr_cb);
  }
  else
  {
    req_param->request_type = ZB_ZDO_EXTENDED_DEVICE_RESP;
    zb_zdo_ieee_addr_req(ZB_REF_FROM_BUF(buf), get_ieee_addr_cb2);
  }
}

void get_peer_short_addr_cb2(zb_uint8_t param) ZB_CALLBACK
{
  zb_schedule_alarm(get_ieee_addr, 1,  10*ZB_TIME_ONE_SECOND);
  zb_free_buf(ZB_BUF_FROM_REF(param));
}

void get_peer_short_addr_cb(zb_uint8_t param) ZB_CALLBACK
{
  zb_schedule_alarm(get_peer_short_addr, 2, 10*ZB_TIME_ONE_SECOND);
  zb_free_buf(ZB_BUF_FROM_REF(param));
}

void get_peer_short_addr(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = zb_get_out_buf();
  zb_zdo_nwk_addr_req_param_t *req_param;

  req_param = ZB_GET_BUF_PARAM(buf, zb_zdo_nwk_addr_req_param_t);

  req_param->dst_addr = 0xffff;
  ZB_IEEE_ADDR_COPY(req_param->ieee_addr, g_c_ieee_addr);
  req_param->start_index = 0;
  if ( param == 1 )
  {
    req_param->request_type = ZB_ZDO_SINGLE_DEVICE_RESP;
    zb_zdo_nwk_addr_req(ZB_REF_FROM_BUF(buf), get_peer_short_addr_cb);
  }
  else
  {
    req_param->request_type = ZB_ZDO_EXTENDED_DEVICE_RESP;
    zb_zdo_nwk_addr_req(ZB_REF_FROM_BUF(buf), get_peer_short_addr_cb2);
  }
}

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
    zb_schedule_alarm(get_peer_short_addr, 1, 25 * ZB_TIME_ONE_SECOND);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
  }

  zb_free_buf(buf);
}
