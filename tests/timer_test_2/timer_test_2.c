/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: simple timer test
*/

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac_transport.h"

#include "zb_bank_1.h"

MAIN()
{
  zb_int_t timeout = 10;

  ZG_DECLARE;
  ARGV_UNUSED;

  ZB_STOP_WATCHDOG(); 
  ZB_SET_TRACE_DISABLED();

  TRACE_INIT("timer_test_2");

  TRACE_MSG(TRACE_INFO2, "started", (FMT__0));

  ZB_START_DEVICE();

  ZB_START_SYNC_OP_TIMER(timeout);

  while(!ZB_CHECK_SYNC_TIMER_EXPIRED())
  {
    ZB_GO_IDLE();
  }

  TRACE_MSG(TRACE_INFO2, "finished", (FMT__0));
  MAIN_RETURN(0);
}

/*
  That rotines are here until we will implement AF.
 */

void zb_apsde_data_confirm(void *p)
{
  ZGUNUSED;
  (void)p;
}

void zb_apsde_data_indication(void *p)
{
  ZGUNUSED;
  (void)p;
}

/*void zb_nlme_network_discovery_confirm(void *p)
{
  (void)p;
}

void zb_nlme_network_formation_confirm(void *p)
{
  (void)p;
}

void zb_nlme_join_indication(void *p)
{
  (void)p;
}
*/

/*! @} */
