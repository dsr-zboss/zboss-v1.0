/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac - TP/154/MAC/ACK-FRAME-DELIVERY-01, FFD1

Check D.U.T. correctly receives ACK frame

*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac.h"

#define USE_ZB_MLME_ASSOCIATE_CONFIRM
#define USE_ZB_MCPS_DATA_INDICATION
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

  ZB_INIT("ack_frame_delivery_01_ffd1", argv[1], argv[2]);

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ffd1_addr);  
  MAC_PIB().mac_pan_id = 0x1aaa;

  {
    zb_buf_t *buf = zb_get_out_buf();
    zb_mlme_set_request_t *set_req;

    /* set rx_on_when_idle to false */
    ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_mlme_set_request_t) + sizeof(zb_uint8_t), set_req);	  	  
	  set_req->pib_attr = ZB_PIB_ATTRIBUTE_RX_ON_WHEN_IDLE;
    set_req->pib_length = sizeof(zb_uint8_t);
    *((zb_uint8_t *)(set_req + 1)) = 0x0;
    
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


static void change_panid(zb_uint16_t pan_id, zb_uint16_t timeout)
{
  zb_buf_t *buf = zb_get_out_buf();
  zb_mlme_set_request_t *set_req;

  TRACE_MSG(TRACE_NWK2, "change_panid", (FMT__0));

  ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_mlme_set_request_t) + sizeof(zb_uint16_t), set_req);
  set_req->pib_attr = ZB_PIB_ATTRIBUTE_PANID;
  set_req->pib_length = sizeof(zb_uint16_t);
  *((zb_uint16_t *)(set_req + 1)) = pan_id;
  ZB_SCHEDULE_ALARM(zb_mlme_set_request, ZB_REF_FROM_BUF(buf), ZB_MILLISECONDS_TO_BEACON_INTERVAL(timeout));
}


void zb_mlme_associate_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mlme_associate_confirm_t *request = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_associate_confirm_t);

  TRACE_MSG(TRACE_NWK2, "zb_mlme_associate_confirm param %hd status %hd short_address %hd",
            (FMT__H_H_H, param, request->status, request->assoc_short_address));

/* FIXME: idle doesn't work for 2410, change pan id instead */
#if 0
  /* set rx_on_when_idle to true */
  {
    zb_mlme_set_request_t *set_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_set_request_t);
    
    set_req->pib_attr = ZB_PIB_ATTRIBUTE_RX_ON_WHEN_IDLE;
    set_req->pib_length = sizeof(zb_uint8_t);
    *((zb_uint8_t *)(set_req + 1)) = 0x1;
        
    ZB_SCHEDULE_ALARM(zb_mlme_set_request, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(10000));
  }
#else  
  {
    zb_free_buf(ZB_BUF_FROM_REF(param));    
        
    /* change pan id to wrong */
    change_panid(0xDEAD, 0);

    /* change pan id back to be able to receive data */
    change_panid(ASSOCIATE_TO_PAN, 10000);

    /* change pan id to wrong again */
    change_panid(0xDEAD, 20000);
  }
#endif
}

void zb_mcps_data_indication(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_MAC1, ">> zb_mcps_data_indication param %hd", (FMT__H, param));
  
  /* set rx_on_when_idle to true */
  {
    zb_mlme_set_request_t *set_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_set_request_t);
    
    set_req->pib_attr = ZB_PIB_ATTRIBUTE_RX_ON_WHEN_IDLE;
    set_req->pib_length = sizeof(zb_uint8_t);
    *((zb_uint8_t *)(set_req + 1)) = 0x0;
        
    ZB_SCHEDULE_ALARM(zb_mlme_set_request, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(1000));
  }
}

/*! @} */
