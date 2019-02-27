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
#include "zb_secur_api.h"
#include "zb_secur.h"
#include "tp_pro_bv_23.h"

#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
#else
zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_zc2_addr = {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb};
zb_ieee_addr_t g_zr2_addr = {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_zr3_addr = {0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00};


zb_ieee_addr_t daintree_1 = {0x00, 0x00, 0x6B, 0x00, 0x00, 0x81, 0x22, 0x00};
zb_ieee_addr_t daintree_2 = {0x00, 0x00, 0x6C, 0x00, 0x00, 0x81, 0x22, 0x00};
#endif 

/*! \addtogroup ZB_TESTS */
/*! @{ */

#ifndef ZB_COORDINATOR_ROLE
#error Coordinator role is not compiled!
#endif


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
  ZB_INIT("zdo_zc1", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zc1", "1", "1");
#endif
#ifdef ZB_SECURITY
  /* switch security off */
  ZG->nwk.nib.security_level = 0;
#endif

  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;
  ZB_NWK().max_children = 1;

  /* assign our address */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_zc_addr);
  MAC_PIB().mac_pan_id = 0x1aaa;
  
  MAC_ADD_VISIBLE_LONG(g_zr2_addr);
  MAC_ADD_INVISIBLE_SHORT(0);   /* ignore beacons from ZC */

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

static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr);

void send_buffertest1(zb_uint8_t param) ZB_CALLBACK
{
  /*
    5) DUT ZC sends a broadcast Buffer Test Request (secured, KEY0) to all
    members of the PAN
   */
  TRACE_MSG(TRACE_SECUR1, "buffer test 1", (FMT__0));
  zc_send_data(ZB_BUF_FROM_REF(param), ZB_NWK_BROADCAST_ALL_DEVICES);
}



void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_APS3, ">>zb_zdo_startup_complete status %hd", (FMT__H, buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));

    zb_schedule_alarm(send_buffertest1, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(75000));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %hd", (FMT__H, buf->u.hdr.status));
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


static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr)
{
  zb_buffer_test_req_param_t *req_param;
  TRACE_MSG(TRACE_APS1, "send_test_request to %d", (FMT__D, addr));
  req_param = ZB_GET_BUF_PARAM(buf, zb_buffer_test_req_param_t);
  req_param->len = 0x10;
  req_param->dst_addr = addr;
  req_param->src_ep = TEST_ED2_EP;
  req_param->dst_ep = TEST_ED1_EP;

  zb_tp_buffer_test_request(ZB_REF_FROM_BUF(buf), buffer_test_cb);
}

/*! @} */
