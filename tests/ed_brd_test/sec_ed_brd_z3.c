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


/*! \addtogroup ZB_TESTS */
/*! @{ */

/* For NS build first ieee addr byte should be unique */
static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr);
void send_buffertest1(zb_uint8_t param);


#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr = {0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00};
#else
/* 00:50:C2:37:80:6B:00:00 for deintree compatibility*/ 
zb_ieee_addr_t g_ieee_addr = {0x00, 0x00, 0x6B, 0x80, 0x37, 0xC2, 0x50, 0x00};
#endif

zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
//zb_ieee_addr_t g_zc_addr = {0x0, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_zr1_addr = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_zr2_addr = {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_zr3_addr = {0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_zed1_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};



zb_uint8_t g_key[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};


static zb_bool_t aps_secure = 0;

/*
  ZR joins to ZC, then sends APS packet.
 */


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

#ifdef APS_SECUR
  aps_secure = 1;
#endif

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_zr3", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zr3", "3", "3");
#endif
#ifdef ZB_SECURITY
  if (aps_secure)
  {
    ZG->nwk.nib.secure_all_frames = 0;
  }
#endif
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
  /* no join to ZR3 */
  ZG->nwk.nib.max_children = 0;
  MAC_ADD_VISIBLE_LONG(g_zr2_addr);
  MAC_ADD_INVISIBLE_SHORT(0);
  MAC_ADD_INVISIBLE_SHORT(1);

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

void send_buffertest1(zb_uint8_t param) ZB_CALLBACK
{
  /*
    5) DUT ZC sends a broadcast Buffer Test Request (secured, KEY0) to all
    members of the PAN
   */
  TRACE_MSG(TRACE_SECUR1, "buffer test 1", (FMT__0));
  zc_send_data(ZB_BUF_FROM_REF(param), 0x0000);
}



void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
		zb_schedule_alarm(send_buffertest1, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(30000));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device start FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
  }
  zb_free_buf(buf);
}

static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr)
{
  zb_buffer_test_req_param_t *req_param;

  TRACE_MSG(TRACE_APS1, "send_test_request to %d", (FMT__D, addr));
  req_param = ZB_GET_BUF_PARAM(buf, zb_buffer_test_req_param_t);
  req_param->len = 20;
  req_param->dst_addr = addr;

  zb_tp_buffer_test_request(ZB_REF_FROM_BUF(buf), 0);
}
/*! @} */
