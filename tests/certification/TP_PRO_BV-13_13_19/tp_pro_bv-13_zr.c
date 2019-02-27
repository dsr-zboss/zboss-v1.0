/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Test for ZC application written using ZDO.
*/


#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

#define TEST_ED1_EP 0x01
#define TEST_ED2_EP 0xF0

/*! \addtogroup ZB_TESTS */
/*! @{ */
zb_ieee_addr_t g_ieee_addr = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};

#ifndef ZB_PRO_COMPATIBLE
#error define ZB_PRO_COMPATIBLE to compile this test
#endif

void zc_send_data(zb_buf_t *buf);
void buffer_test_cb(zb_uint8_t param);



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
  ZB_INIT("zdo_zr", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zr", "2", "2");
#endif

#ifdef ZB_SECURITY
  ZG->nwk.nib.security_level = 0;
#endif

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);  
  /*	MAC_PIB().mac_pan_id = 0x1AAA;  */
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ROUTER;
  ZG->nwk.nib.max_children = 0;
  MAC_ADD_INVISIBLE_SHORT(0);

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
    zc_send_data(buf);	
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}

void buffer_test_cb(zb_uint8_t param)
{
  TRACE_MSG(TRACE_APS1, "buffer_test_cb %hd", (FMT__H, param));
  if (param == ZB_TP_BUFFER_TEST_OK)
  {
    TRACE_MSG(TRACE_APS1, "status OK", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_APS1, "status ERROR", (FMT__0));
  }
}


static void zc_send_data(zb_buf_t *buf)
{
  zb_buffer_test_req_param_t *req_param;

  TRACE_MSG(TRACE_APS1, "send_test_request to %d", (FMT__D, addr));
  req_param = ZB_GET_BUF_PARAM(buf, zb_buffer_test_req_param_t);
  req_param->len = 20;
  req_param->dst_addr = 0x00;

  zb_tp_buffer_test_request(ZB_REF_FROM_BUF(buf), buffer_test_cb);
}
/*! @} */
