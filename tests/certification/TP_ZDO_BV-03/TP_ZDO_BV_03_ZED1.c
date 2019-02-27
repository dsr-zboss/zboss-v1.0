/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: 14.9 TP/ZDO/BV-03: ZC-ZDO-Receive Service Discovery
The DUT as ZigBee coordinator shall respond to mandatory service discovery requests
from a remote node. End device side
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

/* For NS build first ieee addr byte should be unique */
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif

static zb_uint8_t g_error = 0;

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
  ZB_INIT("zdo_zed1_", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zed1_", "2", "2");
#endif

  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);
#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
#endif


#ifdef ZB_SECURITY
  /* turn off security */
  ZB_NIB_SECURITY_LEVEL() = 0;
#endif

  /* become an ED */
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ED;
  ZB_PIB_RX_ON_WHEN_IDLE() = ZB_FALSE;

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

void match_desc_callback(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_uint8_t *zdp_cmd = ZB_BUF_BEGIN(buf);
  zb_zdo_match_desc_resp_t *resp = (zb_zdo_match_desc_resp_t*)zdp_cmd;
  zb_uint8_t *match_list = (zb_uint8_t*)(resp + 1);

  TRACE_MSG(TRACE_APS1, "match_desc_callback status %hd, addr 0x%x",
            (FMT__H, resp->status, resp->nwk_addr));
  if (resp->status != ZB_ZDP_STATUS_SUCCESS || resp->nwk_addr != 0x0)
  {
    TRACE_MSG(TRACE_APS1, "Error incorrect status/addr", (FMT__0));
    g_error++;
  }
  /*
    asdu=Match_Descr_rsp(Status=0x00=Success, NWKAddrOfInterest=0x0000,
    MatchLength=0x01, MatchList=0x01)
  */
  TRACE_MSG(TRACE_APS1, "match_len %hd, list %hd ", (FMT__H_H, resp->match_len, *match_list));
  if (resp->match_len != 1 || *match_list != 1)
  {
    TRACE_MSG(TRACE_APS1, "Error incorrect match result", (FMT__0));
    g_error++;
  }

  TRACE_MSG(TRACE_APS1, "test is finished, error counter %hd", (FMT__H, g_error));
  if (g_error <= 1)
  {
    /* one error is allowed - power descriptor is different */
    TRACE_MSG(TRACE_APS1, "Test status: OK", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_APS1, "Test FAILED", (FMT__0));
  }

  zb_free_buf(buf);
}


void active_ep_callback(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_uint8_t *zdp_cmd = ZB_BUF_BEGIN(buf);
  zb_zdo_ep_resp_t *resp = (zb_zdo_ep_resp_t*)zdp_cmd;
  zb_uint8_t *ep_list = zdp_cmd + sizeof(zb_zdo_ep_resp_t);
  zb_zdo_match_desc_param_t *req;

  TRACE_MSG(TRACE_APS1, "active_ep_callback status %hd, addr 0x%x",
            (FMT__H, resp->status, resp->nwk_addr));

  if (resp->status != ZB_ZDP_STATUS_SUCCESS || resp->nwk_addr != 0x0)
  {
    TRACE_MSG(TRACE_APS1, "Error incorrect status/addr", (FMT__0));
    g_error++;
  }

  TRACE_MSG(TRACE_APS1, " ep count %hd, ep %hd", (FMT__H_H, resp->ep_count, *ep_list));
  if (resp->ep_count != 1 || *ep_list != 1)
  {
    TRACE_MSG(TRACE_APS3, "Error incorrect ep count or ep value", (FMT__0));
    g_error++;
  }

  ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_zdo_match_desc_param_t) + (2 + 3) * sizeof(zb_uint16_t), req);

/*
  NWKAddrOfInterest=0x0000 16-bit DUT ZC NWK address
  ProfileID=Profile of interest to match=0x0103
  NumInClusters=Number of input clusters to match=0x02,
  InClusterList=matching cluster list=0x54 0xe0
  NumOutClusters=return value=0x03
  OutClusterList=return value=0x1c 0x38 0xa8
*/

  req->nwk_addr = 0; //send to coordinator
  req->profile_id = 0x0103;
  req->num_in_clusters = 2;
  req->num_out_clusters = 3;
  req->cluster_list[0] = 0x54;
  req->cluster_list[1] = 0xe0;

  req->cluster_list[2] = 0x1c;
  req->cluster_list[3] = 0x38;
  req->cluster_list[4] = 0xa8;

  zb_zdo_match_desc_req(param, match_desc_callback);
}

void simple_desc_callback(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_uint8_t *zdp_cmd = ZB_BUF_BEGIN(buf);
  zb_zdo_simple_desc_resp_t *resp = (zb_zdo_simple_desc_resp_t*)(zdp_cmd);
  zb_uint_t i;
  zb_zdo_active_ep_req_t *req;

  TRACE_MSG(TRACE_APS1, "simple_desc_callback status %hd, addr 0x%x",
            (FMT__H, resp->hdr.status, resp->hdr.nwk_addr));
  if (resp->hdr.status != ZB_ZDP_STATUS_SUCCESS || resp->hdr.nwk_addr != 0x0)
  {
    TRACE_MSG(TRACE_APS1, "Error incorrect status/addr", (FMT__0));
    g_error++;
  }

/*
simple descriptor for test SimpleDescriptor=
Endpoint=0x01, Application profile identifier=0x0103, Application device
identifier=0x0000, Application device version=0b0000, Application
flags=0b0000, Application input cluster count=0x0A, Application input
cluster list=0x00 0x03 0x04 0x38 0x54 0x70 0x8c 0xc4 0xe0 0xff,
Application output cluster count=0x0A, Application output cluster
list=0x00 0x01 0x02 0x1c 0x38 0x70 0x8c 0xa8 0xc4 0xff
 */

  TRACE_MSG(TRACE_APS1, "ep %hd, app prof %d, dev id %d, dev ver %hd, input count 0x%hx, output count 0x%hx",
            (FMT__H_D_D_H_H_H, resp->simple_desc.endpoint, resp->simple_desc.app_profile_id,
            resp->simple_desc.app_device_id, resp->simple_desc.app_device_version,
           resp->simple_desc.app_input_cluster_count, resp->simple_desc.app_output_cluster_count));

  TRACE_MSG(TRACE_APS1, "clusters:", (FMT__0));
  for(i = 0; i < resp->simple_desc.app_input_cluster_count + resp->simple_desc.app_output_cluster_count; i++)
  {
    TRACE_MSG(TRACE_APS1, " 0x%hx", (FMT__H, *(resp->simple_desc.app_cluster_list + i)));
  }

  ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_zdo_active_ep_req_t), req);
  req->nwk_addr = 0; //coord addr
  zb_zdo_active_ep_req(ZB_REF_FROM_BUF(buf), active_ep_callback);
}

void node_power_desc_callback(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_uint8_t *zdp_cmd = ZB_BUF_BEGIN(buf);
  zb_zdo_power_desc_resp_t *resp = (zb_zdo_power_desc_resp_t*)(zdp_cmd);
  zb_zdo_simple_desc_req_t *req;

  TRACE_MSG(TRACE_APS1, " node_power_desc_callback status %hd, addr 0x%x",
            (FMT__H, resp->hdr.status, resp->hdr.nwk_addr));
  if (resp->hdr.status != ZB_ZDP_STATUS_SUCCESS || resp->hdr.nwk_addr != 0x0)
  {
    TRACE_MSG(TRACE_APS1, "Error incorrect status/addr", (FMT__0));
    g_error++;
  }

  TRACE_MSG(TRACE_APS1, "power mode %hd, avail power src %hd, cur power src %hd, cur power level %hd",
            (FMT__H_H_H_H, ZB_GET_POWER_DESC_CUR_POWER_MODE(&resp->power_desc),
             ZB_GET_POWER_DESC_AVAIL_POWER_SOURCES(&resp->power_desc),
             ZB_GET_POWER_DESC_CUR_POWER_SOURCE(&resp->power_desc),
             ZB_GET_POWER_DESC_CUR_POWER_SOURCE_LEVEL(&resp->power_desc)));
/* PowerDescriptor=Current power mode=0b0000, Available power mode=0b0111, Current
   power source=0b0001, Current power source level=0b1100 */
  if (ZB_GET_POWER_DESC_CUR_POWER_MODE(&resp->power_desc) != 0 ||
      ZB_GET_POWER_DESC_AVAIL_POWER_SOURCES(&resp->power_desc) != 0x7 ||
      ZB_GET_POWER_DESC_CUR_POWER_SOURCE(&resp->power_desc) != 0x1 ||
      ZB_GET_POWER_DESC_CUR_POWER_SOURCE_LEVEL(&resp->power_desc) != 0xC)
  {
    TRACE_MSG(TRACE_APS1, "Error incorrect power desc", (FMT__0));
    g_error++;
  }

  ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_zdo_simple_desc_req_t), req);
  req->nwk_addr = 0; //send to coordinator
  req->endpoint = 1;
  zb_zdo_simple_desc_req(ZB_REF_FROM_BUF(buf), simple_desc_callback);
}


void node_desc_callback(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_uint8_t *zdp_cmd = ZB_BUF_BEGIN(buf);
  zb_zdo_node_desc_resp_t *resp = (zb_zdo_node_desc_resp_t*)(zdp_cmd);
  zb_zdo_power_desc_req_t *req;

  TRACE_MSG(TRACE_APS1, "node_desc_callback: status %hd, addr 0x%x",
            (FMT__H_D, resp->hdr.status, resp->hdr.nwk_addr));
  if (resp->hdr.status != ZB_ZDP_STATUS_SUCCESS || resp->hdr.nwk_addr != 0x0)
  {
    TRACE_MSG(TRACE_APS1, "Error incorrect status/addr", (FMT__0));
    g_error++;
  }
  ZB_LETOH16_XOR(resp->node_desc.node_desc_flags);
  TRACE_MSG(TRACE_APS1, "logic type %hd, aps flag %hd, frequency %hd",
            (FMT__H_H_H, ZB_GET_NODE_DESC_LOGICAL_TYPE(&resp->node_desc), ZB_GET_NODE_DESC_APS_FLAGS(&resp->node_desc),
             ZB_GET_NODE_DESC_FREQ_BAND(&resp->node_desc)));
  if (ZB_GET_NODE_DESC_LOGICAL_TYPE(&resp->node_desc) != 0 || ZB_GET_NODE_DESC_APS_FLAGS(&resp->node_desc) != 0 
  ||     ZB_GET_NODE_DESC_FREQ_BAND(&resp->node_desc) != ZB_FREQ_BAND_2400 )
  {
    TRACE_MSG(TRACE_APS1, "Error incorrect type/flags/freq", (FMT__0));
    g_error++;
  }

  TRACE_MSG(TRACE_APS1, "mac cap 0x%hx, manufact code %hd, max buf %hd, max transfer %hd",
            (FMT__H_H_H_H, resp->node_desc.mac_capability_flags, resp->node_desc.manufacturer_code,
             resp->node_desc.max_buf_size, resp->node_desc.max_incoming_transfer_size));
  if ((resp->node_desc.mac_capability_flags & 0xB) != 0xB /*0b0X001X11*/ || (resp->node_desc.mac_capability_flags & ~0x4f) != 0 ||
      resp->node_desc.manufacturer_code != 0 ||
      resp->node_desc.max_incoming_transfer_size != 0)
  {
    TRACE_MSG(TRACE_APS1, "Error incorrect cap/manuf code/max transfer", (FMT__0));
    g_error++;
  }

  ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_zdo_power_desc_req_t), req);
  req->nwk_addr = 0; //send to coordinator
  zb_zdo_power_desc_req(ZB_REF_FROM_BUF(buf), node_power_desc_callback);
}


void get_node_desc()
{
  zb_buf_t *asdu;
  zb_zdo_node_desc_req_t *req;

  TRACE_MSG(TRACE_APS1, "get_node_desc", (FMT__0));

  asdu = zb_get_out_buf();
  if (!asdu)
  {
    TRACE_MSG(TRACE_ERROR, "out buf alloc failed!", (FMT__0));
  }
  else
  {
    ZB_BUF_INITIAL_ALLOC(asdu, sizeof(zb_zdo_node_desc_req_t), req);
    req->nwk_addr = 0; //send to coordinator
    zb_zdo_node_desc_req(ZB_REF_FROM_BUF(asdu), node_desc_callback);
  }
}


void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
    get_node_desc();
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}
