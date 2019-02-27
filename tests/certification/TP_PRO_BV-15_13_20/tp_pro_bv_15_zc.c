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
#include "zb_secur.h"
#include "zb_secur_api.h"

zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_uint8_t g_key[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_zr1_ieee_addr = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_zr1_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#endif
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_zr2_ieee_addr = {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_zr2_ieee_addr = {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
#endif
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_zed1_ieee_addr = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_zed1_ieee_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif





static zb_bool_t aps_secure = 0;

/*! \addtogroup ZB_TESTS */
/*! @{ */

#ifndef ZB_COORDINATOR_ROLE
#error Coordinator role is not compiled!
#endif


/*
  The test is: ZC starts PAN, ZR joins to it by association and send APS data packet, when ZC
  received packet, it sends packet to ZR, when ZR received packet, it sends
  packet to ZC etc.
 */
#ifdef ZB_NS_BUILD
#define SEND_BUF_TMOUT 40000
#else
#define SEND_BUF_TMOUT 90000
#endif


#ifndef APS_RETRANSMIT_TEST
static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr);
#endif

void data_indication(zb_uint8_t param) ZB_CALLBACK;

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

  /* FIXME: APS secure is off inside stack, lets use NWK secure */
#if 0
  aps_secure = 1;
#endif

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_zc", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zc", "1", "1");
#endif

  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;
#ifdef ZB_SECURITY
  if (aps_secure)
  {
    ZG->nwk.nib.secure_all_frames = 0;
  }
#endif
  zb_secur_setup_preconfigured_key(g_key, 0);
  /* assign our address */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_zc_addr);
  MAC_PIB().mac_pan_id = 0x1aaa;
  /* allow only one join, then let's ZR1 utilize joins */
  ZG->nwk.nib.max_children = 1;
#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
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

void zc_send_buffer_test3_check(zb_uint8_t param) ZB_CALLBACK
{
	if (param == 0)
  {
     TRACE_MSG(TRACE_ERROR, "sending to zr3 success", (FMT__0));
  } else
  {
     TRACE_MSG(TRACE_ERROR, "sending to zr3 failed", (FMT__0));
  }
}


void zc_send_buffer_test3(zb_uint8_t param) ZB_CALLBACK
{
  zb_buffer_test_req_param_t *req_param;
  zb_buf_t *buf;
  if (param == 0)
  {
    TRACE_MSG(TRACE_ERROR, "sending to zr2 success, now sending zr3", (FMT__0));
    buf = zb_get_out_buf();
    req_param = ZB_GET_BUF_PARAM(buf, zb_buffer_test_req_param_t);
    req_param->len = 0x10;
    req_param->dst_addr = zb_address_short_by_ieee(g_zed1_ieee_addr);
    req_param->src_ep = 0;
    req_param->dst_ep = 0;
    zb_tp_buffer_test_request(ZB_REF_FROM_BUF(buf), 0);
  } else
  {
    TRACE_MSG(TRACE_ERROR, "error sending to zr2", (FMT__0));
  }

}

void zc_send_buffer_test2(zb_uint8_t param) ZB_CALLBACK
{
  zb_buffer_test_req_param_t *req_param;
  zb_buf_t *buf;
  if (param == 0)
  {
    TRACE_MSG(TRACE_ERROR, "sending to zr1 success, now sending zr2", (FMT__0));
    buf = zb_get_out_buf();
    req_param = ZB_GET_BUF_PARAM(buf, zb_buffer_test_req_param_t);
    req_param->len = 0x10;
    req_param->dst_addr = zb_address_short_by_ieee(g_zr2_ieee_addr);
    req_param->src_ep = 0;
    req_param->dst_ep = 0;
    zb_tp_buffer_test_request(ZB_REF_FROM_BUF(buf), zc_send_buffer_test3);
  } else
  {
    TRACE_MSG(TRACE_ERROR, "error sending to zr1", (FMT__0));
  }
}

void zc_send_buffer_test(zb_uint8_t param) ZB_CALLBACK
{
  zb_buffer_test_req_param_t *req_param;
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_ERROR, "sending to zr1", (FMT__0));
  req_param = ZB_GET_BUF_PARAM(buf, zb_buffer_test_req_param_t);
  req_param->len = 0x10;
  req_param->dst_addr = zb_address_short_by_ieee(g_zr1_ieee_addr);
  req_param->src_ep = 0;
  req_param->dst_ep = 0;

  zb_tp_buffer_test_request(ZB_REF_FROM_BUF(buf), zc_send_buffer_test2);
}




void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_APS3, ">>zb_zdo_startup_complete status %d", (FMT__D, (int)buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
    zb_af_set_data_indication(data_indication);
    ZB_SCHEDULE_ALARM(zc_send_buffer_test, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(SEND_BUF_TMOUT));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTE FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
  }
}


/*
   Trivial test: dump all APS data received
 */


void data_indication(zb_uint8_t param) ZB_CALLBACK
{
  zb_ushort_t i;
  zb_uint8_t *ptr;
  zb_buf_t *asdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);
#ifndef APS_RETRANSMIT_TEST
  zb_apsde_data_indication_t *ind = ZB_GET_BUF_PARAM(asdu, zb_apsde_data_indication_t);
#endif

  /* Remove APS header from the packet */
  ZB_APS_HDR_CUT_P(asdu, ptr);

  TRACE_MSG(TRACE_APS3, "apsde_data_indication: packet %p len %d handle 0x%x", (FMT__P_D_D,
                         asdu, (int)ZB_BUF_LEN(asdu), asdu->u.hdr.status));

  for (i = 0 ; i < ZB_BUF_LEN(asdu) ; ++i)
  {
    TRACE_MSG(TRACE_APS3, "%x %c", (FMT__D_C, (int)ptr[i], ptr[i]));
    if (ptr[i] != i % 32 + '0')
    {
      TRACE_MSG(TRACE_ERROR, "Bad data %hx %c wants %x %c", (FMT__H_C_D_C, ptr[i], ptr[i],
                              (int)(i % 32 + '0'), (char)i % 32 + '0'));
    }
  }

  /* send packet back to ZR */
  zc_send_data(asdu, ind->src_addr);
}


static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr)
{
  zb_apsde_data_req_t *req;
  zb_uint8_t *ptr = NULL;
  zb_short_t i;

  return;

  ZB_BUF_INITIAL_ALLOC(buf, 70, ptr);
  req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
  req->dst_addr.addr_short = addr; /* send to ZR */
  req->addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  req->tx_options = ZB_APSDE_TX_OPT_ACK_TX;
  if (aps_secure)
  {
    req->tx_options |= (ZB_APSDE_TX_OPT_SECURITY_ENABLED | ZB_APSDE_TX_OPT_USE_NWK_KEY);
  }
  req->radius = 1;
  req->profileid = 2;
  req->src_endpoint = 10;
  req->dst_endpoint = 10;

  buf->u.hdr.handle = 0x11;

  for (i = 0 ; i < 70 ; ++i)
  {
    ptr[i] = i % 32 + '0';
  }
  TRACE_MSG(TRACE_APS3, "Sending apsde_data.request", (FMT__0));

  ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
}


/*! @} */
