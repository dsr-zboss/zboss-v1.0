/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: TP/NWK/BV-03 gZED1
*/


#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "zb_secur_api.h"

/* For NS build first ieee addr byte should be unique */
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif
zb_ieee_addr_t g_zed_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static zb_bool_t aps_secure = 0;
static zb_uint16_t addr = 0;


/*! \addtogroup ZB_TESTS */
/*! @{ */


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
  zb_init("zdo_zc", argv[1], argv[2]);
#else
  zb_init("zdo_zc", "1", "1");
#endif
  ZB_AIB().aps_designated_coordinator = 1;
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_COORDINATOR;  
  if (aps_secure)
  {
    ZG->nwk.nib.secure_all_frames = 0;
  }
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
  ZG->nwk.nib.max_children = 4;
  ZG->nwk.nib.ed_child_num = 0;

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
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));    
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
  }

  zb_free_buf(buf);
}


/*! @} */
