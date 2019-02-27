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
zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
#endif
zb_ieee_addr_t g_ext_pan_id = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

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
  ZB_INIT("tp_pro_bv_30_DUTZR2", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zr", "2", "2");
#endif

  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 0;
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
  /* we need this for rejoin */	\
  ZB_IEEE_ADDR_COPY(ZB_AIB().aps_use_extended_pan_id, &g_ext_pan_id);
  MAC_PIB().mac_pan_id = 0x1aab;
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
