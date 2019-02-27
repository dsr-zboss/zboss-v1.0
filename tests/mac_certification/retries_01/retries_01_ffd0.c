/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac - TP/154/MAC/RETRIES-01, FFD0

Check D.U.T. uses correct number of frame retries when transmission fails

*/

/* FIXME: this test works only on 2410, don't know how to change the numer of retries on 2400 */

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac.h"

#define USE_ZB_MLME_START_CONFIRM
#define USE_ZB_MLME_SET_CONFIRM
#define USE_ZB_MCPS_DATA_CONFIRM
#include "zb_mac_only_stubs.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

#define CHANNEL 0x14
#define MY_PAN 0x1aaa
static zb_ieee_addr_t g_ffd0_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};

static zb_short_t state = 0;


MAIN()
{
  ARGV_UNUSED;

  ZB_INIT("retries_01_ffd0", argv[1], argv[2]);

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ffd0_addr);
  MAC_PIB().mac_pan_id = MY_PAN;
  ZB_PIB_SHORT_ADDRESS() = 0x1122;

  {
    zb_buf_t *buf = zb_get_out_buf();

    /* change number of retries */
 	  zb_mlme_set_request_t *set_req;

    ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_mlme_set_request_t) + sizeof(zb_uint8_t), set_req);
	  set_req->pib_attr = ZB_PIB_ATTRIBUTE_MAX_FRAME_RETRIES;
    set_req->pib_length = sizeof(zb_uint8_t);
    *((zb_uint8_t *)(set_req + 1)) = 0x7;
    
    ZB_SCHEDULE_CALLBACK(zb_mlme_set_request, ZB_REF_FROM_BUF(buf));
  }

  while (1)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


void send_request(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = zb_get_out_buf();
  zb_uint8_t *pl;
  zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(buf, zb_mcps_data_req_params_t);

  param = ZB_REF_FROM_BUF(buf);

  ZB_BUF_INITIAL_ALLOC(buf, 5, pl);
  pl[0] = 0x00;
  pl[1] = 0x01;
  pl[2] = 0x02;
  pl[3] = 0x03;
  pl[4] = 0x04;  

  data_req->dst_addr.addr_short = 0x3344;
  data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
  data_req->src_addr.addr_short = 0x1122;
  data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
  data_req->dst_pan_id = MY_PAN;
  data_req->msdu_handle = 0x0a;
  data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT; 

  TRACE_MSG(TRACE_MAC1, "i. D.U.T to Tester: Short Address to Short Address, with ACK", (FMT__0));
  ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, ZB_REF_FROM_BUF(buf));
}


void zb_mlme_set_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mlme_start_req_t *req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_start_req_t);  

  TRACE_MSG(TRACE_NWK2, "zb_mlme_set_confirm", (FMT__0));

  if ( state == 0 )
  {
    /* start PAN */
    ZB_BZERO(req, sizeof(*req));
    req->pan_id = MAC_PIB().mac_pan_id;
    req->logical_channel = CHANNEL;
    req->pan_coordinator = 1;      /* will be coordinator */
    req->beacon_order = ZB_TURN_OFF_ORDER;
    req->superframe_order = ZB_TURN_OFF_ORDER;
  
    ZB_SCHEDULE_CALLBACK(zb_mlme_start_request, param);
  }
  else if ( state == 1 )
  {
    ZB_SCHEDULE_ALARM(send_request, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(10000));    
    zb_free_buf(ZB_BUF_FROM_REF(param));
  }
  else
  {
    zb_free_buf(ZB_BUF_FROM_REF(param));
  }
}

void zb_mlme_start_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK2, "zb_mlme_start_confirm", (FMT__0));

  ZB_SCHEDULE_ALARM(send_request, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(1000));

  zb_free_buf(ZB_BUF_FROM_REF(param));
}


void zb_mcps_data_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mcps_data_confirm_params_t *confirm_params = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_confirm_params_t);
  
  TRACE_MSG(TRACE_NWK2, "zb_mcps_data_confirm param %hd handle 0x%hx status 0x%hx state %hd",
            (FMT__H_H_H_H, (zb_uint8_t)param, (zb_uint8_t)confirm_params->msdu_handle,
             (zb_uint8_t)ZB_BUF_FROM_REF(param)->u.hdr.status, state));

  if ( state == 0 )
  {
    /* change number of retries */
 	  zb_mlme_set_request_t *set_req;

    ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), sizeof(zb_mlme_set_request_t) + sizeof(zb_uint8_t), set_req);
	  set_req->pib_attr = ZB_PIB_ATTRIBUTE_MAX_FRAME_RETRIES;
    set_req->pib_length = sizeof(zb_uint8_t);
    *((zb_uint8_t *)(set_req + 1)) = 0x3;
    
    ZB_SCHEDULE_CALLBACK(zb_mlme_set_request, param);
  }
  else
  {
    zb_free_buf(ZB_BUF_FROM_REF(param));
  }
  
  state++;  
}


/*! @} */
