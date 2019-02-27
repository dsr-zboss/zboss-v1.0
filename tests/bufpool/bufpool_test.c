/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for the buffers pool
*/


/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_common.h"
#include "zb_bufpool.h"

#include "zb_bank_1.h"
void zb_callback_test(zb_uint8_t param) ZB_CALLBACK;

void zb_callback_test(zb_uint8_t param) ZB_CALLBACK
{
  if (param) {};
  TRACE_MSG(TRACE_INFO2, "SUCCESS", (FMT__0));
}



MAIN()
{
  zb_int_t i;
  zb_buf_t *in_bufs[ZB_IOBUF_POOL_SIZE];
  zb_buf_t *out_bufs[ZB_IOBUF_POOL_SIZE];
  ARGV_UNUSED;

  ZB_INIT("bufpool_test", NULL, NULL);
  TRACE_MSG(TRACE_INFO2, "started", (FMT__0));

  /* Loop is up to ZB_IOBUF_POOL_SIZE while could allcoate only
   * ZB_IOBUF_POOL_SIZE/2. Will free some buffers.
   */
  for (i = 0 ; i < ZB_IOBUF_POOL_SIZE/2 ; ++i)
  {
    in_bufs[i] = zb_get_in_buf();
    out_bufs[i] = zb_get_out_buf();
  }
  zb_get_out_buf_delayed(zb_callback_test);

  MAIN_RETURN(0);
}

/* just to compile... */

void zb_zdo_startup_complete(zb_uint8_t param)
{
  (void)param;
}

#if 0

/* just to compile... */
void zb_apsde_data_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_apsde_data_indication(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_network_discovery_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_network_formation_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_join_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_join_indication(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
}

#endif

/*! @} */
