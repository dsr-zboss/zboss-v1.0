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
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

#include "zb_bank_1.h"

zb_buf_t *in_bufs[ZB_IOBUF_POOL_SIZE];
zb_buf_t *out_bufs[ZB_IOBUF_POOL_SIZE];
zb_uint8_t counter = 0;
zb_uint8_t free_counter = 0;
  
void zb_callback_test(zb_uint8_t param) ZB_CALLBACK;
void zb_freebuf_test(zb_uint8_t param) ZB_CALLBACK;


void zb_callback_test(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);                                                    
  counter++;
  zb_free_buf(ZB_BUF_FROM_REF(param));
  if ((counter == 3)&&(free_counter == 1)) 
  /* just place breakpoint here, or look for trace message */
  TRACE_MSG(TRACE_INFO2, "SUCCESS!", (FMT__0));
}
void zb_freebuf_test(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  zb_free_buf(ZB_BUF_FROM_REF(param));  
  free_counter++;
}

MAIN()
{
  zb_int_t i;
  zb_int_t j = 0;
  zb_int_t n_free = 0;
  zb_int_t n_allocated = 0;
  ARGV_UNUSED;

  ZB_INIT("zdo_zr", "2", "2");
  TRACE_MSG(TRACE_INFO2, "started", (FMT__0));

  /* Loop is up to ZB_IOBUF_POOL_SIZE while could allcoate only
   * ZB_IOBUF_POOL_SIZE/2. Will free some buffers.
   */

  for (i = 0 ; i < ZB_IOBUF_POOL_SIZE/2 ; ++i)
  {
    in_bufs[i] = zb_get_in_buf();
    out_bufs[i] = zb_get_out_buf();
  }
  zb_free_buf(out_bufs[0]);  
  ZB_SCHEDULE_CALLBACK(zb_freebuf_test, ZB_REF_FROM_BUF(zb_get_out_buf()));
  /* delayed callbacks */
  zb_get_out_buf_delayed(zb_callback_test);  
  zb_get_out_buf_delayed(zb_callback_test);  
  zb_get_out_buf_delayed(zb_callback_test);  
  TRACE_MSG(TRACE_INFO2, "SUCCESS, allocated %d, free %d", (FMT__D_D, n_allocated, n_free));  
  zb_free_buf(out_bufs[0]);
  zdo_main_loop();
  MAIN_RETURN(0);
}

/* just to compile... */

void zb_zdo_startup_complete(zb_uint8_t param)
{
  if (param) {};
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
