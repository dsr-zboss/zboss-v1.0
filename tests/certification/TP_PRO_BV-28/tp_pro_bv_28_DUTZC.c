/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: TP/PRO/BV-28 Rejoin end device (Rx - Off)
Validate end device (with Rx on idle=false/true) does not change its short address
on rejoin, when it leaves from one parent and joins with another parent.
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

zb_ieee_addr_t g_ieee_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};


static void beacon_sent()
{
  static int cnt = 0;
  TRACE_MSG(TRACE_ERROR, "BEACON sent", (FMT__0));

  cnt++;
#if defined ZB_NS_BUILD || defined ZB_EMBER_GOLDEN_UNIT  
  if ( cnt == 4*ZB_ZDO_NWK_SCAN_ATTEMPTS )
#else
  if ( cnt == 2*ZB_ZDO_NWK_SCAN_ATTEMPTS )
#endif
  {
    TRACE_MSG(TRACE_ERROR, "Now accept new child", (FMT__0));

    ZB_NWK().max_children = 3; /* 3 needed when end-device failed to receive rejoin response */
    MAC_PIB().mac_association_permit = 1;
    zb_nwk_update_beacon_payload();
  }
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
  ZB_INIT("zdo_zc", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zc", "1", "1");
#endif

  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;

  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);

#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
  ZB_UPDATE_PAN_ID();
#endif


#ifdef ZB_SECURITY
  /* turn off security */
  ZB_NIB_SECURITY_LEVEL() = 0;
#endif

  /* accept only one child */
  ZB_NWK().max_children = 1;

  MAC_CTX().beacon_sent = &beacon_sent;

  /* set beacon request notification */

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

/*
  MAC_PIB().mac_association_permit = 0;
  zb_nwk_update_beacon_payload();
 */
