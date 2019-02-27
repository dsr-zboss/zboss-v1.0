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
#include "zb_secur_api.h"
#include "tp_pro_bv_23.h"

/*! \addtogroup ZB_TESTS */
/*! @{ */

/* For NS build first ieee addr byte should be unique */
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_zr2_addr = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_zc2_addr = {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb};
zb_ieee_addr_t g_zr2_addr = {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_zr3_addr = {0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00};

zb_ieee_addr_t daintree_1 = {0x00, 0x00, 0x6B, 0x00, 0x00, 0x81, 0x22, 0x00};
zb_ieee_addr_t daintree_2 = {0x00, 0x00, 0x6C, 0x00, 0x00, 0x81, 0x22, 0x00};
#endif



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

#if 0
  aps_secure = 1;
#endif

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_zr2", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zr2", "2", "2");
#endif
#ifdef ZB_SECURITY
  /* switch security off */
  ZG->nwk.nib.security_level = 0;
#endif
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_zr2_addr);
  MAC_ADD_VISIBLE_LONG(g_zc_addr);
  MAC_ADD_VISIBLE_LONG(daintree_1);
  MAC_ADD_VISIBLE_LONG(g_zc2_addr);
  MAC_ADD_VISIBLE_LONG(daintree_2);
  ZB_NWK().max_children = 0;
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
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
  }
  zb_free_buf(buf);
}


/*! @} */
