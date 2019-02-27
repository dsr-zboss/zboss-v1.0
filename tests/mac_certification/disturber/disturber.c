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

#define DISTURBER_CHANNEL 0x14


/*! \addtogroup ZB_TESTS */
/*! @{ */

MAIN()
{
  ARGV_UNUSED;

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("disturber", argv[1], argv[2]);
#else
  ZB_INIT("disturber", "1", "1");
#endif
  zb_mac_disturber_loop(DISTURBER_CHANNEL);

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


/* Just to compile ok... */
void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
}

void zb_mlme_purge_confirm(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
}


/*! @} */
