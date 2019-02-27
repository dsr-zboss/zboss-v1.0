/***************************************************************************
*                      ZBOSS ZigBee Pro 2007 stack                         *
*                                                                          *
*          Copyright (c) 2012 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*                            All rights reserved.                          *
*          Copyright (c) 2011 ClarIDy Solutions, Inc., Taipei, Taiwan.     *
*                       http://www.claridy.com/                            *
*                                                                          *
*          Copyright (c) 2011 Uniband Electronic Corporation (UBEC),       *
*                             Hsinchu, Taiwan.                             *
*                       http://www.ubec.com.tw/                            *
*                                                                          *
*          Copyright (c) 2011 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*                            All rights reserved.                          *
*                                                                          *
*                                                                          *
* ZigBee Pro 2007 stack, also known as ZBOSS (R) ZB stack is available     *
* under either the terms of the Commercial License or the GNU General      *
* Public License version 2.0.  As a recipient of ZigBee Pro 2007 stack, you*
* may choose which license to receive this code under (except as noted in  *
* per-module LICENSE files).                                               *
*                                                                          *
* ZBOSS is a registered trademark of DSR Corporation AKA Data Storage      *
* Research LLC.                                                            *
*                                                                          *
* GNU General Public License Usage                                         *
* This file may be used under the terms of the GNU General Public License  *
* version 2.0 as published by the Free Software Foundation and appearing   *
* in the file LICENSE.GPL included in the packaging of this file.  Please  *
* review the following information to ensure the GNU General Public        *
* License version 2.0 requirements will be met:                            *
* http://www.gnu.org/licenses/old-licenses/gpl-2.0.html.                   *
*                                                                          *
* Commercial Usage                                                         *
* Licensees holding valid ClarIDy/UBEC/DSR Commercial licenses may use     *
* this file in accordance with the ClarIDy/UBEC/DSR Commercial License     *
* Agreement provided with the Software or, alternatively, in accordance    *
* with the terms contained in a written agreement between you and          *
* ClarIDy/UBEC/DSR.                                                        *
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


/*! \addtogroup ZB_TESTS */
/*! @{ */

//static void send_data(zb_buf_t *buf);
void data_indication(zb_uint8_t param) ZB_CALLBACK;
void node_power_desc_callback(zb_uint8_t param) ZB_CALLBACK;
void simple_desc_callback(zb_uint8_t param) ZB_CALLBACK;
void simple_desc_callback_ep(zb_uint8_t param) ZB_CALLBACK;
void simple_desc_callback_err(zb_uint8_t param) ZB_CALLBACK;

static zb_int_t g_error_counter = 0;

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

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_zr", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zr", "2", "2");
#endif
#ifdef ZB_SECURITY
  ZG->nwk.nib.security_level = 0;
#endif


  zb_set_default_ffd_descriptor_values(ZB_ROUTER);

  /* FIXME: temporary, until neighbor table is not in nvram */
  /* add extended address of potential parent to emulate that we've already
   * been connected to it */
  {
    zb_ieee_addr_t ieee_address;
    zb_address_ieee_ref_t ref;

    ZB_64BIT_ADDR_ZERO(ieee_address);
    ieee_address[7] = 8;

    zb_address_update(ieee_address, 0, ZB_FALSE, &ref);
  }

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

void node_desc_callback(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_uint8_t *zdp_cmd = ZB_BUF_BEGIN(buf);
  zb_zdo_node_desc_resp_t *resp = (zb_zdo_node_desc_resp_t*)(zdp_cmd);
  zb_zdo_power_desc_req_t *req;

  if (resp->hdr.status == ZB_ZDP_STATUS_SUCCESS)
  {
    TRACE_MSG(TRACE_APS1, "node desc received Ok", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "ERROR receiving node desc %x", (FMT__D, resp->hdr.status));
    g_error_counter++;
  }

  ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_zdo_power_desc_req_t), req);
  req->nwk_addr = 0; //send to coordinator
  zb_zdo_power_desc_req(ZB_REF_FROM_BUF(buf), node_power_desc_callback);
}

void node_power_desc_callback(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_uint8_t *zdp_cmd = ZB_BUF_BEGIN(buf);
  zb_zdo_power_desc_resp_t *resp = (zb_zdo_power_desc_resp_t*)(zdp_cmd);
  zb_zdo_simple_desc_req_t *req;

  if (resp->hdr.status == ZB_ZDP_STATUS_SUCCESS)
  {
    TRACE_MSG(TRACE_APS1, "node power desc received Ok", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "ERROR receiving node power desc %x", (FMT__D, resp->hdr.status));
    g_error_counter++;
  }

  ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_zdo_simple_desc_req_t), req);
  req->nwk_addr = 0; //send to coordinator
  req->endpoint = 0; //zdo ep
  zb_zdo_simple_desc_req(ZB_REF_FROM_BUF(buf), simple_desc_callback);
}


void simple_desc_callback(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_uint8_t *zdp_cmd = ZB_BUF_BEGIN(buf);
  zb_zdo_simple_desc_resp_t *resp = (zb_zdo_simple_desc_resp_t*)(zdp_cmd);
  zb_zdo_simple_desc_req_t *req;

  if (resp->hdr.status == ZB_ZDP_STATUS_SUCCESS)
  {
    TRACE_MSG(TRACE_APS1, "simple desc received Ok", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "ERROR receiving simple desc %x", (FMT__D, resp->hdr.status));
    g_error_counter++;
  }

  ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_zdo_simple_desc_req_t), req);
  req->nwk_addr = 0; //send to coordinator
  req->endpoint = 1; // ep 1
  zb_zdo_simple_desc_req(ZB_REF_FROM_BUF(buf), simple_desc_callback_ep);

}

void simple_desc_callback_ep(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_uint8_t *zdp_cmd = ZB_BUF_BEGIN(buf);
  zb_zdo_simple_desc_resp_t *resp = (zb_zdo_simple_desc_resp_t*)(zdp_cmd);
  zb_zdo_simple_desc_req_t *req;

  TRACE_MSG(TRACE_APS1, "ep %hd, app prof %d, dev id %d, dev ver %hd, input count 0x%hx, output count 0x%hx",
            (FMT__H_D_D_H_H_H, resp->simple_desc.endpoint, resp->simple_desc.app_profile_id,
            resp->simple_desc.app_device_id, resp->simple_desc.app_device_version,
           resp->simple_desc.app_input_cluster_count, resp->simple_desc.app_output_cluster_count));


  if (resp->hdr.status == ZB_ZDP_STATUS_SUCCESS)
  {
    TRACE_MSG(TRACE_APS1, "simple desc received Ok", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "ERROR receiving simple desc %x", (FMT__D, resp->hdr.status));
    g_error_counter++;
  }

  ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_zdo_simple_desc_req_t), req);
  req->nwk_addr = 0; //send to coordinator
  req->endpoint = 10; // ep 10
  zb_zdo_simple_desc_req(ZB_REF_FROM_BUF(buf), simple_desc_callback_err);
}

void simple_desc_callback_err(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_uint8_t *zdp_cmd = ZB_BUF_BEGIN(buf);
  zb_zdo_simple_desc_resp_t *resp = (zb_zdo_simple_desc_resp_t*)(zdp_cmd);

  if (resp->hdr.status == ZB_ZDP_STATUS_SUCCESS)
  {
    TRACE_MSG(TRACE_ERROR, "simple desc received status ok, ERROR!", (FMT__0));
    g_error_counter++;
  }
  else
  {
    TRACE_MSG(TRACE_APS1, "Ok to receiving simple desc fail status %x", (FMT__D, resp->hdr.status));
  }

  if (g_error_counter)
  {
    TRACE_MSG(TRACE_APS1, "zdo descriptor test finished, status: FAILED error_counter %i", (FMT__D, g_error_counter));
  }
  else
  {
    TRACE_MSG(TRACE_APS1, "zdo descriptor test finished, status: OK", (FMT__0));
  }
}


void func1(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *asdu;
  zb_zdo_node_desc_req_t *req;
  ZVUNUSED(param);

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
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
    zb_af_set_data_indication(data_indication);
    //send_data((zb_buf_t *)ZB_BUF_FROM_REF(param));
    /* wait for coordinator */
    ZB_SCHEDULE_ALARM(func1, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(30000));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}

#if 0
static void send_data(zb_buf_t *buf)
{
  zb_apsde_data_req_t *req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
  zb_uint8_t *ptr = NULL;
  zb_short_t i;

  req->dst_addr.addr_short = 0; /* send to ZC */
  req->addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  req->tx_options = ZB_APSDE_TX_OPT_ACK_TX;
  req->radius = 1;
  req->profileid = 2;
  req->src_endpoint = 10;
  req->dst_endpoint = 10;

  buf->u.hdr.handle = 0x11;
  ZB_BUF_INITIAL_ALLOC(buf, 80, ptr);

  for (i = 0 ; i < 80 ; ++i)
  {
    ptr[i] = i % 32 + '0';
  }
  TRACE_MSG(TRACE_APS3, "Sending apsde_data.request", (FMT__0));

  ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
}
#endif


void data_indication(zb_uint8_t param)
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
  }

  //send_data(asdu);
}



/*! @} */
