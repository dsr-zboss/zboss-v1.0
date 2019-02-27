/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac - TP/154/MAC/WARM-START-01, RFD1

Check D.U.T. (as device) correctly starts up when PIB is restored using MLME-SET.request commands.

*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac.h"

#define USE_ZB_MLME_SET_CONFIRM
#define USE_ZB_MCPS_DATA_CONFIRM
#include "zb_mac_only_stubs.h"

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

#define COORD_ADDRESS 0x1122
#define MY_PAN 0x1aaa
#define MY_ADDRESS 0x3344
#define CHANNEL 0x14
#define CAP_INFO 0x80           /* 80 - "allocate address" */
static zb_ieee_addr_t g_rfd1_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};
static zb_ieee_addr_t g_ffd0_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};

static zb_short_t state = 0;


static void set_request_8bit_value(zb_uint8_t param, zb_mac_pib_attr_t attr, zb_uint8_t value)
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_mlme_set_request_t *set_req;

  ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_mlme_set_request_t) + sizeof(zb_uint8_t), set_req);	  	  
  set_req->pib_attr = attr;
  set_req->pib_length = sizeof(zb_uint8_t);
  *((zb_uint8_t *)(set_req + 1)) = value;
    
  ZB_SCHEDULE_CALLBACK(zb_mlme_set_request, param);
}


static void set_request_16bit_value(zb_uint8_t param, zb_mac_pib_attr_t attr, zb_uint16_t value)
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  zb_mlme_set_request_t *set_req;
    
  ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_mlme_set_request_t) + sizeof(zb_uint16_t), set_req);	  	  
  set_req->pib_attr = attr;
  set_req->pib_length = sizeof(zb_uint16_t);
  *((zb_uint16_t *)(set_req + 1)) = value;
    
  ZB_SCHEDULE_CALLBACK(zb_mlme_set_request, param);
}


MAIN()
{
  ARGV_UNUSED;

  ZB_INIT("warm_start_01_rfd1", argv[1], argv[2]);

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_rfd1_addr);    

  {
    zb_buf_t *buf = zb_get_out_buf();
    set_request_8bit_value(ZB_REF_FROM_BUF(buf), ZB_PIB_ATTRIBUTE_RX_ON_WHEN_IDLE, 0x1);
  }

  while(1)
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
  TRACE_MSG(TRACE_MAC1, "get out buf %p param %hd", (FMT__P_H, buf, param));

  ZB_BUF_INITIAL_ALLOC(buf, 5, pl);
  pl[0] = 0x00;
  pl[1] = 0x01;
  pl[2] = 0x02;
  pl[3] = 0x03;
  pl[4] = 0x04;  

  data_req->dst_addr.addr_short = 0x1122;
  data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
  data_req->src_addr.addr_short = 0x3344;
  data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
  data_req->dst_pan_id = MY_PAN;
  data_req->msdu_handle = 0x0a; 
  data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT; 

  TRACE_MSG(TRACE_MAC1, "i. D.U.T to Coord: Short Address to Short Address, with ACK", (FMT__0));
  ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, ZB_REF_FROM_BUF(buf));
}


void zb_mlme_set_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK2, "zb_mlme_set_confirm param %hd state %hd", (FMT__H_H, param, state));

  if ( state == 0 )
  {
    /* set current channel */
    set_request_8bit_value(param, ZB_PHY_PIB_CURRENT_CHANNEL, CHANNEL);
  }
  else if ( state == 1 )
  { 
    /* set PANID */
    set_request_16bit_value(param, ZB_PIB_ATTRIBUTE_PANID, MY_PAN);
  }
  else if ( state == 2 )
  {
    /* set MAC short address */
    set_request_16bit_value(param, ZB_PIB_ATTRIBUTE_SHORT_ADDRESS, MY_ADDRESS);
  }
  else if ( state == 3 )
  {
    /* set MAC coord short address */
    set_request_16bit_value(param, ZB_PIB_ATTRIBUTE_COORD_SHORT_ADDRESS, COORD_ADDRESS);
  }
  else if ( state == 4 )
  {
    /* set MAC Coordinator extend address */
    zb_buf_t *buf = ZB_BUF_FROM_REF(param);
    zb_mlme_set_request_t *set_req;
    
    ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_mlme_set_request_t) + sizeof(zb_ieee_addr_t), set_req);	  	  
    set_req->pib_attr = ZB_PIB_ATTRIBUTE_COORD_EXTEND_ADDRESS;
    set_req->pib_length = sizeof(zb_ieee_addr_t);
    ZB_EXTPANID_COPY((zb_uint8_t *)(set_req + 1), g_ffd0_addr);
    
    ZB_SCHEDULE_CALLBACK(zb_mlme_set_request, param);
  }
  else if ( state == 5 )
  {
    ZB_SCHEDULE_CALLBACK(send_request, 0);
  }
  else
  {
    zb_free_buf(ZB_BUF_FROM_REF(param));
  }

  state++;
}

void zb_mcps_data_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mcps_data_confirm_params_t *confirm_params = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_confirm_params_t);
  
  TRACE_MSG(TRACE_NWK2, "zb_mcps_data_confirm param %hd handle 0x%hx status 0x%hx state %hd",
            (FMT__H_H_H_H, (zb_uint8_t)param, (zb_uint8_t)confirm_params->msdu_handle,
             (zb_uint8_t)ZB_BUF_FROM_REF(param)->u.hdr.status, state));

  zb_free_buf(ZB_BUF_FROM_REF(param));  
}

/*! @} */
