/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: TP/APS/BV-27-I Coordinator
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

/*! \addtogroup ZB_TESTS */
/*! @{ */

/* For NS build first ieee addr byte should be unique */
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr   = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_ieee_addr_d = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr   = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_ieee_addr_d = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#endif


void data_indication(zb_uint8_t param) ZB_CALLBACK;
void send_data(zb_uint8_t param) ZB_CALLBACK;

void zb_leave_req(zb_uint8_t param) ZB_CALLBACK;
void zb_nwk_leave_req(zb_uint8_t param) ZB_CALLBACK;

zb_bool_t flag;

MAIN()
{
  ARGV_UNUSED;

  flag = ZB_TRUE;
#ifndef KEIL
  if ( argc < 3 )
  {
    printf("%s <read pipe path> <write pipe path>\n", argv[0]);
    return 0;
  }
#endif

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("tp_nwk_bv_04_gZC", argv[1], argv[2]);
#else
  ZB_INIT("tp_nwk_bv_04_gZC", "1", "1");
#endif

  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;

  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
  MAC_PIB().mac_pan_id = 0x1aaa;

#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
#endif

#ifdef ZB_SECURITY
  /* turn off security */
  ZB_NIB_SECURITY_LEVEL() = 0;
#endif

  if ( zdo_dev_start() != RET_OK )
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
  TRACE_MSG(TRACE_ERROR, ">>zb_zdo_startup_complete status %d", (FMT__D, (int)buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
    zb_af_set_data_indication(data_indication);
    if (flag)
    {
      ZB_SCHEDULE_ALARM(zb_leave_req, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(40000));
      flag = ZB_FALSE;
    }
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device START FAILED", (FMT__0));
    zb_free_buf(buf);
  }
}

void zb_leave_req(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  ZB_GET_OUT_BUF_DELAYED(zb_nwk_leave_req);
}

void zb_nwk_leave_req(zb_uint8_t param) ZB_CALLBACK
{
  zb_nlme_leave_request_t *lr = NULL;

  TRACE_MSG(TRACE_ERROR, "zb_leave_req_test", (FMT__0));
  lr = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_nlme_leave_request_t);
  ZB_64BIT_ADDR_COPY(lr->device_address, g_ieee_addr_d);
  lr->remove_children = 0;
  lr->rejoin = 0;
  ZB_SCHEDULE_CALLBACK(zb_nlme_leave_request, param);
}

void data_indication(zb_uint8_t param) ZB_CALLBACK
{
  zb_ushort_t i;
  zb_uint8_t *ptr;
  zb_buf_t *asdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);

  /* Remove APS header from the packet */
  ZB_APS_HDR_CUT_P(asdu, ptr);

  TRACE_MSG(TRACE_APS3, "data_indication: packet %p len %d handle 0x%x", (FMT__P_D_D,
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
  send_data(param);
}

void send_data(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = (zb_buf_t *)ZB_BUF_FROM_REF(param);
  zb_apsde_data_indication_t ind;
  zb_apsde_data_req_t *req;
  zb_uint8_t *ptr = NULL;
  zb_short_t i;

  ind = *ZB_GET_BUF_PARAM(buf, zb_apsde_data_indication_t);
  ZB_BUF_INITIAL_ALLOC(buf, 30, ptr);
  req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
  req->dst_addr.addr_short = ind.src_addr;
  req->addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  req->tx_options = 0;
  req->radius = 1;
  req->profileid = 2;
  req->src_endpoint = 1;
  req->dst_endpoint = 2;
  buf->u.hdr.handle = 0x11;
  for (i = 0 ; i < 30 ; ++i)
  {
    ptr[i] = i % 32 + '0';
  }
  TRACE_MSG(TRACE_APS3, "Sending apsde_data.request", (FMT__0));
  ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
}

/*! @} */
