/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Trivial scheduler test
*/

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_mac_transport.h"

#include "zb_bank_1.h"

static int n_func1_call = 0;
static int n_func2_call = 0;
static int n_func3_call = 0;

void func2(zb_uint8_t param) ZB_CALLBACK;
void func3(zb_uint8_t param) ZB_CALLBACK;

void func1(zb_uint8_t param) ZB_CALLBACK
{
  static int cnt = 0;
  zb_ret_t ret = RET_OK;
  cnt++;
  TRACE_MSG(TRACE_INFO2, "func1 call # %d : %d", (FMT__D_D, cnt, param));
  n_func1_call++;
  if (cnt < 10 &&
      (ret = ZB_SCHEDULE_CALLBACK(func2,
                                  (param + 1) )) != RET_OK)
  {
    TRACE_MSG(TRACE_INFO2, "ZB_SCHEDULE_CALLBACK failed %d", (FMT__D, ret));
  }
}

void func2(zb_uint8_t param) ZB_CALLBACK
{
  zb_ret_t ret = RET_OK;
  TRACE_MSG(TRACE_INFO2, "func2 called : %hd", (FMT__H, param));
  n_func2_call++;
  if ((ret = ZB_SCHEDULE_CALLBACK(func3,
                                  param + 1)) != RET_OK)
  {
    TRACE_MSG(TRACE_INFO2, "ZB_SCHEDULE_CALLBACK failed %d", (FMT__D, ret));
  }
}

void func3(zb_uint8_t param) ZB_CALLBACK
{
  zb_ret_t ret = RET_OK;
  TRACE_MSG(TRACE_INFO2, "func3 called : %hd", (FMT__H, param));
  n_func3_call++;
  if ((ret = ZB_SCHEDULE_CALLBACK(func1,
                                  (param + 1))) != RET_OK)
  {
    TRACE_MSG(TRACE_INFO2, "ZB_SCHEDULE_CALLBACK failed %d", (FMT__D, ret));
  }
}


MAIN()
{
  zb_int_t i;
  ARGV_UNUSED;
	


  ZB_INIT("scheduler_test", NULL, NULL);

  TRACE_MSG(TRACE_INFO2, "started", (FMT__0));

  func1(1);
  func2(1+100);

  for (i = 0 ; i < 10 ; ++i)
  {
    zb_sched_loop_iteration();
  }

  if (n_func1_call == 11
      && n_func2_call == 10
      && n_func3_call == 10)
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
  ZVUNUSED(param);
}


#if 0

/* FIXME: Temporary here, move to AF */
void zb_apsde_data_confirm(zb_uint8_t param)
{
  (void)param;
}

/* FIXME: Temporary here, move to AF */
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
