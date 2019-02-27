/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: TP/NWK/BV-03 gZED1
*/


#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "zb_secur_api.h"

/* For NS build first ieee addr byte should be unique */
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif

#define PACKET_SEND_DELAY 10*ZB_TIME_ONE_SECOND
#define INITIAL_JOINING_PERIOD 60*ZB_TIME_ONE_SECOND
#define REENABLE_JOINING_DELAY 400*ZB_TIME_ONE_SECOND



static zb_bool_t aps_secure = 0;
void send_data(zb_uint8_t param) ZB_CALLBACK;

/*! \addtogroup ZB_TESTS */
/*! @{ */


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
  zb_init("zdo_zed1", argv[1], argv[2]);
#else
  zb_init("zdo_zed1", "4", "4");
#endif
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ED;
  ZB_PIB_RX_ON_WHEN_IDLE() = 1;
  if (aps_secure)
  {
    ZG->nwk.nib.secure_all_frames = 0;
  }
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);

#ifdef ZB_SECURITY
 /* turn off security */
 ZB_NIB_SECURITY_LEVEL() = 0;
#endif

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

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));    
    ZB_SCHEDULE_ALARM(send_data, 0, PACKET_SEND_DELAY);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
  }
  ZB_SCHEDULE_CALLBACK(zb_check_joined, param);
  zb_free_buf(buf);
}

void send_data(zb_uint8_t param) ZB_CALLBACK
{
  zb_apsde_data_req_t *req; 
  zb_uint8_t *ptr = NULL;  
  zb_buf_t *buf;
  
  ZVUNUSED(param);
  buf = zb_get_out_buf();
  req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));

  req->dst_addr.addr_short = ZG->addr.addr_map[ZG->nwk.handle.parent].addr; /* send to parent */
  req->addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  req->tx_options = 0;
  req->radius = 1;
  req->profileid = 2;
  req->src_endpoint = 10;
  req->dst_endpoint = 10;
  buf->u.hdr.handle = 0x11;

  TRACE_MSG(TRACE_APS3, "Sending apsde_data.request", (FMT__0));

  ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
  ZB_SCHEDULE_ALARM(send_data, 0, PACKET_SEND_DELAY);
}
/*! @} */
