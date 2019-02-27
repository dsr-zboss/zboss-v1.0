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

/* For NS build first ieee addr byte should be unique */
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr = {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
#endif
zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
//zb_ieee_addr_t g_zc_addr = {0x0, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00};

/* daintree macAddress:										  */
/* they are 00:50:C2:37:80:6B:00:00 and 00:50:C2:37:80:6C:00:00*/
zb_ieee_addr_t g_zr1_addr = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_zr3_addr = {0x00, 0x50, 0xC2, 0x37, 0x80, 0x6C, 0x00, 0x00};
zb_ieee_addr_t g_zed1_addr = {0x00, 0x50, 0xC2, 0x37, 0x80, 0x6B, 0x00, 0x00};

zb_uint8_t g_key[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};


static zb_bool_t aps_secure = 0;

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

#ifdef APS_SECUR
  aps_secure = 1;
#endif

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_zr2", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zr2", "2", "2");
#endif
#ifdef ZB_SECURITY
  if (aps_secure)
  {
    ZG->nwk.nib.secure_all_frames = 0;
  }
#endif
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
  /* ZR2 allows joins */
  ZG->nwk.nib.max_children = 1;
/*
  MAC_ADD_VISIBLE_LONG(g_zr1_addr);
  MAC_ADD_VISIBLE_LONG(g_zr3_addr);
  MAC_ADD_VISIBLE_LONG(g_zed1_addr);
  MAC_ADD_INVISIBLE_SHORT(0);
*/
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
