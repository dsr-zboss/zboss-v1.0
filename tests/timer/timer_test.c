/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Trivial timer test
*/

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac_transport.h"

#include "zb_bank_1.h"

static int n_func1_call = 0;
static int n_func2_call = 0;
static int n_func3_call = 0;


void func2(zb_uint8_t param) ZB_CALLBACK;
void func3(zb_uint8_t param) ZB_CALLBACK;


/*
  func1 schedules func2 after 2s timeout and func3 after 1s timeout.
  func2 schedules itself after 3s timeout
  func3 schedules itself after 4s timeout
 */

void func1(zb_uint8_t param) ZB_CALLBACK
{
  static int cnt = 0;
  zb_ret_t ret = RET_OK;
  cnt++;
  n_func1_call++;
  TRACE_MSG(TRACE_INFO2, "func1 call # %d : %d", (FMT__D_D, cnt, param));
  if (cnt < 10 &&
      (ret = ZB_SCHEDULE_ALARM(func2,
                               (param + 1),
                               ZB_MILLISECONDS_TO_BEACON_INTERVAL(2 * 1000))) != RET_OK)
  {
    TRACE_MSG(TRACE_INFO2, "ZB_SCHEDULE_CALLBACK failed %d", (FMT__D, ret));
  }
  if (cnt < 10 &&
      (ret = ZB_SCHEDULE_ALARM(func3,
                               (param + 1),
                               ZB_MILLISECONDS_TO_BEACON_INTERVAL(1 * 1000))) != RET_OK)
  {
    TRACE_MSG(TRACE_INFO2, "ZB_SCHEDULE_CALLBACK failed %d", (FMT__D, ret));
  }
}

void func2(zb_uint8_t param) ZB_CALLBACK
{
  zb_ret_t ret = RET_OK;
  TRACE_MSG(TRACE_INFO2, "func2 called : %hd", (FMT__H, param));
  n_func2_call++;
  if ((ret = ZB_SCHEDULE_ALARM(func2,
                               (param + 1), ZB_MILLISECONDS_TO_BEACON_INTERVAL(3 * 1000))) != RET_OK)
  {
    TRACE_MSG(TRACE_INFO2, "ZB_SCHEDULE_CALLBACK failed %d", (FMT__D, ret));
  }
}

void func3(zb_uint8_t param) ZB_CALLBACK
{
  zb_ret_t ret = RET_OK;
  TRACE_MSG(TRACE_INFO2, "func3 called : %hd", (FMT__H, param));
  n_func3_call++;
  if ((ret = ZB_SCHEDULE_ALARM(func3,
                               (param + 1), ZB_MILLISECONDS_TO_BEACON_INTERVAL(4 * 1000))) != RET_OK)
  {
    TRACE_MSG(TRACE_INFO2, "ZB_SCHEDULE_CALLBACK failed %d", (FMT__D, ret));
  }
}


MAIN()
{
  zb_int_t i;
  ARGV_UNUSED;

  ZB_INIT("timer_test", "/tmp/zudp0.write", "/tmp/zudp0.read");

  TRACE_MSG(TRACE_INFO2, "started", (FMT__0));

  func1(1);

  for (i = 0 ; i < 10 ; ++i)
  {
    zb_sched_loop_iteration();
  }

  if (n_func1_call == 1
      && n_func2_call == 4
      && n_func3_call == 4)
  {
    TRACE_MSG(TRACE_INFO2, "SUCCESS", (FMT__0));
    MAIN_RETURN(0);
  }
  else
  {
    TRACE_MSG(TRACE_INFO2, "ERROR: n_func1_call %d, n_func2_call %d, n_func3_call %d", (FMT__D_D_D,
              n_func1_call, n_func2_call, n_func3_call));
    MAIN_RETURN(-1);
  }
}

/* just to compile... */

void zb_zdo_startup_complete(zb_uint8_t param)
{
  (void)param;
}


#if 0
/*
  That rotines are here until we will implement AF.
 */

void zb_apsde_data_confirm(zb_uint8_t param)
{
  (void)param;
}

void zb_apsde_data_indication(zb_uint8_t param)
{
  (void)param;
}

void zb_nlme_network_discovery_confirm(zb_uint8_t param)
{
  (void)param;
}

void zb_nlme_network_formation_confirm(zb_uint8_t param)
{
  (void)param;
}

void zb_nlme_join_indication(zb_uint8_t param)
{
  (void)param;
}

void zb_nlme_join_confirm(zb_uint8_t param)
{
  (void)param;
}
#endif


/*! @} */
