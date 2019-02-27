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
zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif
zb_ieee_addr_t g_zed_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static zb_bool_t aps_secure = 0;
static zb_uint16_t addr = 0;
void start_packet_send(zb_uint8_t param) ZB_CALLBACK;

#define TEST_PACKET_LENGTH 36
#define TEST_PACKET_COUNT 1000
#define TEST_PACKET_DELAY 40 /* ms */



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
  zb_init("zdo_zc", argv[1], argv[2]);
#else
  zb_init("zdo_zc", "1", "1");
#endif
  ZB_AIB().aps_designated_coordinator = 1;
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_COORDINATOR;  
  if (aps_secure)
  {
    ZG->nwk.nib.secure_all_frames = 0;
  }
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
  ZG->nwk.nib.max_children = 1;

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

void packets_sent_cb(zb_uint8_t param) ZB_CALLBACK
{
  //zb_buf_t *buf = ZB_BUF_FROM_PARAM(param);
  ZVUNUSED(param);
  TRACE_MSG(TRACE_APS3, "packets_sent_cb", (FMT__0));
}

void start_packet_send(zb_uint8_t param) ZB_CALLBACK
{  
  zb_apsde_data_req_t *req = ZB_GET_BUF_TAIL(ZB_BUF_FROM_REF(param), sizeof(zb_apsde_data_req_t));
  zb_uint8_t *ptr = NULL;
  zb_short_t i;

  req->dst_addr.addr_short = zb_address_short_by_ieee(g_zed_addr);
  req->addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  req->tx_options = ZB_APSDE_TX_OPT_ACK_TX;
  req->radius = 1;
  req->profileid = 2;
  req->src_endpoint = 10;
  req->dst_endpoint = 10;

  ZB_BUF_FROM_REF(param)->u.hdr.handle = 0x11;
  ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 0, ptr);

  for (i = 0 ; i < ZB_TEST_DATA_SIZE ; ++i)
  {
    ptr[i] = i % 32 + '0';
  }
  TRACE_MSG(TRACE_APS3, "Sending apsde_data.request", (FMT__0));

  ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, param);  
  zb_schedule_alarm(start_packet_send, ZB_REF_FROM_BUF(zb_get_out_buf()), ZB_TIME_ONE_SECOND * 5);	
}



void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));    
	zb_schedule_alarm(start_packet_send, param, ZB_TIME_ONE_SECOND * 15);	
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
	zb_free_buf(buf);
  }

  
}


/*! @} */
