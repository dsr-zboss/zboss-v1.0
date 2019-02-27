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
#include "zb_secur.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
/* For NS build first ieee addr byte should be unique */
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#endif


void req_key(zb_uint8_t param) ZB_CALLBACK
{
  zb_apsme_request_key_req_t *req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_apsme_request_key_req_t);

  TRACE_MSG(TRACE_SECUR1, "request key", (FMT__0));

  ZB_IEEE_ADDR_COPY(req->dest_address, g_zc_addr);
  req->key_type = ZB_STANDARD_NETWORK_KEY;
  ZB_SCHEDULE_CALLBACK(zb_secur_apsme_request_key, param);
}


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
#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
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
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
   zb_schedule_alarm(req_key, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(10000));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}


/*! @} */
