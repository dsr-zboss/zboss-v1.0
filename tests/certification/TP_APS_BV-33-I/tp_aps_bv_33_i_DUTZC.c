/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: 14.21 TP/APS/ BV-33-I End Device Binding (HC V1 DUT
ZC). Coordinator side
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "tp_aps_bv_33_i.h"

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
  ZB_INIT("tp_aps_bv_33_i_DUTZC", argv[1], argv[2]);
#else
  ZB_INIT("tp_aps_bv_33_i_DUTZC", "1", "1");
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


void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_ERROR, ">>zb_zdo_startup_complete status %d", (FMT__D, (int)buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device START FAILED", (FMT__0));
  }

  zb_free_buf(buf);
}
