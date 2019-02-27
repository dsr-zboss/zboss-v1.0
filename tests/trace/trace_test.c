
/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Trivial logger test
*/

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_common.h"
#include "zb_bufpool.h"
#include "zb_ringbuffer.h"
#include "zb_mac_transport.h"

#include "zb_bank_common.h"

#ifdef SDCC
DECLARE_SERIAL_INTER_HANDLER()
#endif

MAIN()
{
  int i = 0;
  ARGV_UNUSED;
/* disable watchdog */
  ZB_STOP_WATCHDOG();
  
  ZB_INIT("logger", NULL, NULL);


  
  for (i = 0; i < 20; i++)
  {
    TRACE_MSG(TRACE_ERROR, "Trace test started: message", (FMT__0));
    TRACE_MSG(TRACE_INFO2, "INFO message d %d", (FMT__D, -10));
    TRACE_MSG(TRACE_INFO2, "INFO message x 0x%x", (FMT__D, 0xab12));
    TRACE_MSG(TRACE_INFO2, "INFO message c %c", (FMT__C, 'q'));
    TRACE_MSG(TRACE_INFO2, "INFO message u %u", (FMT__D, (unsigned)ZB_UINT_MAX));
    TRACE_MSG(TRACE_INFO2, "INFO message d %i x %x u %u %c", (FMT__D_D_D_C, 10, 0x123, (unsigned)ZB_UINT_MAX, 'q'));
    TRACE_MSG(TRACE_INFO2, "INFO5 message\n", (FMT__0));
    TRACE_MSG(TRACE_INFO2, "INFO6 message\n", (FMT__0));
    TRACE_MSG(TRACE_INFO2, "INFO7 message\n", (FMT__0));
    TRACE_MSG(TRACE_INFO2, "INFO8 message\n", (FMT__0));
    TRACE_MSG(TRACE_INFO2, "INFO9 message\n", (FMT__0));
    TRACE_MSG(TRACE_INFO2, "INFO10 message\n", (FMT__0));
    TRACE_MSG(TRACE_INFO2, "INFO11 message\n", (FMT__0));
    TRACE_MSG(TRACE_INFO2, "INFO12 message\n", (FMT__0));
    TRACE_MSG(TRACE_INFO2, "INFO13 message\n", (FMT__0));
    TRACE_MSG(TRACE_INFO2, "INFO14 message\n", (FMT__0));
    TRACE_MSG(TRACE_ERROR, "SUCCESS: Trace test finished\n", (FMT__0));
  }
#ifdef ZB_TRANSPORT_8051_UART
  {
    int x = 1;
    while( x )
    {
      x = 1;
      ZB_GO_IDLE();
    };
  }
#endif
  TRACE_DEINIT();

  MAIN_RETURN(0);
}


/* just to compile... */

void zb_zdo_startup_complete(zb_uint8_t param)
{
  (void)param;
}



#if 0
void zb_apsde_data_indication(void *p)
{
  (void)p;
}


void zb_apsde_data_confirm(void *p)
{
  (void)p;
}

void zb_nlme_network_discovery_confirm(zb_uint8_t param) ZB_CALLBACK
{
  (void)param;
}


void zb_nlme_network_formation_confirm(zb_uint8_t param) ZB_CALLBACK
{
  (void)param;
}

void zb_nlme_join_confirm(zb_uint8_t param) ZB_CALLBACK
{
  (void)param;
}


void zb_nlme_join_indication(zb_uint8_t param) ZB_CALLBACK
{
  (void)param;
}
#endif

/*! @} */

