/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Test for standby mode
*/


#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

#ifndef ZB_ED_ROLE
//#error define ZB_ED_ROLE required for this test
#endif
/*! \addtogroup ZB_TESTS */
/*! @{ */

#define ZB_TEST_CHANNEL 0x0E
void zb_beacon_req_test(zb_uint8_t param) ZB_CALLBACK;
void zb_fall_asleep(zb_uint8_t param) ZB_CALLBACK;

MAIN()
{    
  ARGV_UNUSED;

#if !(defined KEIL || defined SDCC)
  if ( argc < 3 )
  {
    printf("%s <read pipe path> <write pipe path>\n", argv[0]);
    return 0;
  }
#endif

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  zb_init("zdo_ze", argv[1], argv[2]);
#else
  zb_init((char*)"zdo_ze", (char*)"3", (char*)"3");
#endif
#ifdef ZB_SECURITY
  ZG->nwk.nib.security_level = 0;
#endif
  
  

  ZB_TRANSCEIVER_SET_CHANNEL(ZB_TEST_CHANNEL);

  TRACE_MSG(TRACE_ERROR, "1st beacon req", (FMT__0));
  zb_beacon_request_command();
  zb_timed_sleep();
  TRACE_MSG(TRACE_ERROR, "2nd beacon req", (FMT__0));
while (1)
{
  zb_beacon_request_command();
}
#if 0
  zb_timed_sleep();
  zb_timed_sleep();
  zb_beacon_request_command();

  
  zb_beacon_request_command();
  zb_timed_sleep();
  zb_beacon_request_command();
  zb_timed_sleep();
  zb_timed_sleep();
  zb_beacon_request_command();
  zb_timed_sleep();
  zb_timed_sleep();
  zb_timed_sleep();
#endif
  zb_beacon_request_command();
    

  zdo_main_loop();
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
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}

void zb_beacon_req_test(zb_uint8_t param) ZB_CALLBACK
{
	ZVUNUSED(param);
	zb_beacon_request_command();
}

void zb_fall_asleep(zb_uint8_t param) ZB_CALLBACK
{
	ZVUNUSED(param);
	zb_timed_sleep();
}



/*! @} */
