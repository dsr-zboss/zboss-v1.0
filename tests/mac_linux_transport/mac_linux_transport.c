/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Mac transport under linux test
*/

#include "zb_common.h"
#include "zb_bufpool.h"
#include "zb_scheduler.h"
#include "zb_ringbuffer.h"
#include "zb_mac_transport.h"
#include "zb_mac.h"

int main(int argc, char **argv)
{
  int i = 0;
  zb_buf_t out_buf;
  void *ptr = NULL;

  (void)argc;
  (void)argv;

  ZB_MEMSET(&out_buf, 0, sizeof(out_buf));

  ZB_INIT("mac_transport_linux", "/tmp/zudp0.write", "/tmp/zudp0.read");

  /* prepare buffer for output */
#if 1
  {
    int src_addr = 0xff;
    int dst_addr = 0xff;

    ZB_BUF_INITIAL_ALLOC(&out_buf, 0, ptr);

    ZB_MCPS_BUILD_DATA_REQUEST(&out_buf, src_addr, dst_addr, MAC_TX_OPTION_ACKNOWLEDGED_BIT, 0);
  }
#else
  ZB_BUF_INITIAL_ALLOC(&out_buf, 100, ptr);
  ZB_MEMSET(ptr, 0xff, 100);
  *(char *)ptr = 100;
#endif

  zb_mac_transport_put_data(&out_buf);

  for (i = 0; i < 10; i++)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();
  return 0;
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

void zb_apsme_update_device_request(zb_uint8_t param)
{
  (void)param;
}

#endif
