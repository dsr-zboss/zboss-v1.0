/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: ZED for 13.18TP/PRO/BV-13 Operation of ZigBee 2006 devices on ZigBee
PRO PAN
*/


#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "zb_secur_api.h"

zb_ieee_addr_t g_ieee_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define TEST_ED1_EP 0x01
#define TEST_ED2_EP 0xF0

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
  ZB_INIT("zdo_zed1", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zed1", "4", "4");
#endif
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ED;
  ZB_PIB_RX_ON_WHEN_IDLE() = 0;

  ZG->nwk.nib.secure_all_frames = 1;
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);

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


void send_test_request(zb_uint8_t param) ZB_CALLBACK;

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
    zb_schedule_alarm(send_test_request, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(1000));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
  }
  zb_free_buf(buf);																																																														                 
}																																																																          
																																																                                                                 

void buffer_test_cb(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_APS1, "buffer_test_cb %hd", (FMT__H, param));
  zb_free_buf(ZB_BUF_FROM_REF(param));																																											                        
}


void send_test_request(zb_uint8_t param) ZB_CALLBACK																																									                                       
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_buffer_test_req_param_EP_t *req_param;			                                                                                                               \
  TRACE_MSG(TRACE_APS1, "send_test_request %hd", (FMT__H, param));

  req_param = ZB_GET_BUF_PARAM(buf, zb_buffer_test_req_param_EP_t);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
                                                                                                                                                                                                                                                                                      req_param->len = 10;

  req_param->src_ep = TEST_ED1_EP;
  req_param->dst_ep = TEST_ED2_EP;
                    }


/*! @} */
