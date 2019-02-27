/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac - TP/154/MAC/FRAME-VALIDATION-05, FFD1

Check that the MAC sublayer will discard a received frame that implements MAC 2003 security.

*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac.h"

#define USE_ZB_MLME_ASSOCIATE_CONFIRM
#define USE_ZB_MLME_SET_CONFIRM
#include "zb_mac_only_stubs.h"

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

#define ASSOCIATE_TO 0x1122
#define ASSOCIATE_TO_PAN 0x1aaa
#define CHANNEL 0x14
#define CAP_INFO 0x80           /* 80 - "allocate address" */
static zb_ieee_addr_t g_ffd1_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};
static zb_ieee_addr_t g_ffd0_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};

static zb_short_t state = 0;


MAIN()
{
  ARGV_UNUSED;

  ZB_INIT("frame_validation_05_ffd1", argv[1], argv[2]);

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ffd1_addr);  
  MAC_PIB().mac_pan_id = 0x1aaa;

  {
    zb_buf_t *buf = zb_get_out_buf();
    zb_mlme_set_request_t *set_req;

    /* set rx_on_when_idle to true */
    ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_mlme_set_request_t) + sizeof(zb_uint8_t), set_req);	  	  
	  set_req->pib_attr = ZB_PIB_ATTRIBUTE_RX_ON_WHEN_IDLE;
    set_req->pib_length = sizeof(zb_uint8_t);
    *((zb_uint8_t *)(set_req + 1)) = 0x1;
    
    ZB_SCHEDULE_CALLBACK(zb_mlme_set_request, ZB_REF_FROM_BUF(buf));
  }

  while(1)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


void zb_mlme_set_confirm(zb_uint8_t param) ZB_CALLBACK
{  
  zb_uint16_t addr = ASSOCIATE_TO;

  TRACE_MSG(TRACE_NWK2, "zb_mlme_set_confirm", (FMT__0));

  if ( state == 0 )
  {
    /* connect to the coord */
    ZB_MLME_BUILD_ASSOCIATE_REQUEST(ZB_BUF_FROM_REF(param), CHANNEL,
                                    ASSOCIATE_TO_PAN,
                                    ZB_ADDR_16BIT_DEV_OR_BROADCAST, &addr,
                                    CAP_INFO);

    ZB_SCHEDULE_CALLBACK(zb_mlme_associate_request, param);    
  }
  else
  {
    zb_free_buf(ZB_BUF_FROM_REF(param));
  }

  state++;
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
  data_req->dst_pan_id = ASSOCIATE_TO_PAN;
  data_req->msdu_handle = 0xFA; 
  data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT; 

  TRACE_MSG(TRACE_MAC1, "i. D.U.T to Tester: Short Address to Short Address, with ACK", (FMT__0));
  ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, ZB_REF_FROM_BUF(buf));
}


void zb_mlme_associate_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mlme_associate_confirm_t *request = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_associate_confirm_t);

  TRACE_MSG(TRACE_NWK2, "zb_mlme_associate_confirm param %hd status %hd short_address %hd",
            (FMT__H_H_H, param, request->status, request->assoc_short_address));

  ZB_SCHEDULE_CALLBACK(send_request, 0);    
  zb_free_buf(ZB_BUF_FROM_REF(param));
}

/*! @} */
