/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: TP/PRO/BV-26 Router – Joins the network
Verify that the router device is capable to join a network.
*/
#if !defined ZB_USE_NVRAM && !defined TP_PRO_BV_31
#error please, define ZB_USE_NVRAM and TP_PRO_BV_31 to compile this test
#endif 

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr);


/* For NS build first ieee addr byte should be unique */
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
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
  ZB_INIT("zdo_zr", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zr", "2", "2");
#endif

  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
#endif
  /* join as a router */
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ROUTER;

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
  zb_ieee_addr_t parent_addr;
  zb_uint16_t s_parent_addr;
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
	zb_address_ieee_by_ref(&parent_addr, ZG->nwk.handle.parent);
	zb_address_short_by_ref(&s_parent_addr, ZG->nwk.handle.parent);
	zb_write_formdesc_data(ZB_STACK_PROFILE, 
	parent_addr, ZB_AIB().aps_channel_mask,s_parent_addr, ZB_NIB_DEPTH(), MAC_PIB().mac_pan_id, ZG->nwk.nib.extended_pan_id, MAC_PIB().mac_short_address);    
	/* just for debug purpose */
	/*zb_read_formdesc_data();*/ 
  }
  else if (buf->u.hdr.status == ZB_NWK_STATUS_ALREADY_PRESENT )
  {
  	TRACE_MSG(TRACE_ERROR, "Device resumed %d", (FMT__D, (int)buf->u.hdr.status));
    zb_erase_nvram(0);
	ZB_P4_ON();	
	zc_send_data(buf, ZB_NWK_BROADCAST_ALL_DEVICES);
  } else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));		
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
  req_param->src_ep = ZB_TEST_PROFILE_EP;
  req_param->dst_ep = ZB_TEST_PROFILE_EP;

  zb_tp_buffer_test_request(ZB_REF_FROM_BUF(buf), buffer_test_cb);
}

