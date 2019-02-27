/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE:  14.21 TP/APS/ BV-33-I End Device Binding (HC V1 DUT
ZC). End device 1
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "tp_aps_bv_33_i.h"

void send_test_request(zb_uint8_t param) ZB_CALLBACK;
void send_end_device_bind_request(zb_uint8_t param) ZB_CALLBACK;

zb_ieee_addr_t g_ieee_addr = TEST_IEEE_ADDR_C;
zb_ieee_addr_t g_ieee_addr_ed1 = TEST_IEEE_ADDR_ED1;
zb_ieee_addr_t g_ieee_addr_ed2 = TEST_IEEE_ADDR_ED2;

static zb_uint8_t g_counter = 0;
static zb_uint8_t g_error = 0;

zb_af_simple_desc_1_1_t test_simple_desc;

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
  ZB_INIT("tp_aps_bv_33_i_gZED1", argv[1], argv[2]);
#else
  ZB_INIT("tp_aps_bv_33_i_gZED1", "2", "2");
#endif

  zb_set_simple_descriptor((zb_af_simple_desc_1_1_t*)&test_simple_desc,
                           0x01 /* endpoint */,               0x7f01 /* app_profile_id */,
                           0x0 /* app_device_id */,           0x0   /* app_device_version*/,
                           0x1 /* app_input_cluster_count */, 0x1   /* app_output_cluster_count */);
  zb_set_input_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 0,  0x1c);
  zb_set_output_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 0,  0x54);
  zb_add_simple_descriptor((zb_af_simple_desc_1_1_t*)&test_simple_desc);

  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr_ed1);
#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
#endif

#ifdef ZB_SECURITY
  /* turn off security */
  ZB_NIB_SECURITY_LEVEL() = 0;
#endif

  /* become an ED */
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ED;
  ZB_PIB_RX_ON_WHEN_IDLE() = ZB_TRUE;

/* ----------------------------------------------------------- */

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


void buffer_test_cb(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_APS1, "buffer_test_cb %hd", (FMT__H, param));
  if (!g_counter)
  {
    if (param == ZB_TP_BUFFER_TEST_OK)
    {
      TRACE_MSG(TRACE_APS1, "Test resp recv OK", (FMT__0));
    }
    else
    {
      TRACE_MSG(TRACE_APS1, "Test resp, ERROR", (FMT__0));
      g_error++;
    }

    TRACE_MSG(TRACE_APS1, "call send_end_device_bind_request() to unbind device", (FMT__0));
    ZB_SCHEDULE_CALLBACK(send_end_device_bind_request, 0);
  }
  else
  {
    if (param == ZB_TP_BUFFER_TEST_OK)
    {
      TRACE_MSG(TRACE_APS1, "Unexpected Ok, test resp error ", (FMT__0));
      g_error++;
    }
    else
    {
      TRACE_MSG(TRACE_APS1, "Expected error - OK", (FMT__0));
    }
  }
  g_counter++;
  if (g_counter == 2)
  {
    if (g_error == 0)
    {
      TRACE_MSG(TRACE_APS1, "Test is finished, status OK", (FMT__0));
    }
    else
    {
      TRACE_MSG(TRACE_APS1, "Test is finished with error, status FAIL", (FMT__0));
    }
  }
}


void send_test_request(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_buffer_test_req_param_EP_t *req_param;

  TRACE_MSG(TRACE_APS1, "send_test_request %hd", (FMT__H, param));

  req_param = ZB_GET_BUF_PARAM(buf, zb_buffer_test_req_param_EP_t);
  req_param->len = TEST_BUFFER_LEN;

  req_param->src_ep = TEST_ED1_EP;
  req_param->dst_ep = TEST_ED2_EP;

  zb_tp_buffer_test_request_EP(param, buffer_test_cb);
}
void end_device_bind_cb(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_zdo_end_device_bind_resp_t *resp = (zb_zdo_end_device_bind_resp_t*)ZB_BUF_BEGIN(buf);

  TRACE_MSG(TRACE_APS1, "end_device_bind resp status %hd", (FMT__H, resp->status));
  if (resp->status != ZB_ZDP_STATUS_SUCCESS)
  {
    TRACE_MSG(TRACE_APS1, "Error binding device", (FMT__0));
  }

  ZB_SCHEDULE_ALARM(send_test_request, param, 55 * ZB_TIME_ONE_SECOND);
}

void send_end_device_bind_request(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf;
  zb_end_device_bind_req_param_t *req_param;

  TRACE_MSG(TRACE_APS1, "send_end_device_bind_request %hd", (FMT__H, param));
  buf = zb_get_out_buf();
  if (!buf)
  {
    TRACE_MSG(TRACE_ERROR, "out buf alloc failed!", (FMT__0));
  }
  else
  {
    param = ZB_REF_FROM_BUF(buf);
    ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_end_device_bind_req_param_t) + 8 * sizeof(zb_uint16_t), req_param);
    req_param->dst_addr = 0;
    req_param->head_param.binding_target = ZB_PIB_SHORT_ADDRESS(); /* this device short addr */
    ZB_MEMCPY(req_param->head_param.src_ieee_addr, g_ieee_addr_ed1, sizeof(zb_ieee_addr_t));
    req_param->head_param.src_endp = TEST_ED1_EP;
    req_param->head_param.profile_id = TEST_PROFILE_ID;
    req_param->head_param.num_in_cluster = 4;
    req_param->tail_param.num_out_cluster = 5;
    /* In cluster list: 0x0054 0xe000 0xe001 0x0004 */
    req_param->cluster_list[0] = 0x0054;
    req_param->cluster_list[1] = 0xe000;
    req_param->cluster_list[2] = 0xe001;
    req_param->cluster_list[3] = 0x0004;
    /* Out cluster list: 0x001c 0xa0a8 0x0001 0x0002 0x0003 */
    req_param->cluster_list[4] = 0x001c;
    req_param->cluster_list[5] = 0xa0a8;
    req_param->cluster_list[6] = 0x0001;
    req_param->cluster_list[7] = 0x0002;
    req_param->cluster_list[8] = 0x0003;

    zb_end_device_bind_req(param, end_device_bind_cb);
  }
}

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
    ZB_SCHEDULE_ALARM(send_end_device_bind_request, 0, 20 * ZB_TIME_ONE_SECOND);
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}
