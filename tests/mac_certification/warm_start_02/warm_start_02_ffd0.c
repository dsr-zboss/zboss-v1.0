/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac - TP/154/MAC/WARM-START-02, FFD0

Check D.U.T. (as coordinator) correctly starts up when PIB is restored using 
MLME-SET.request commands and MLME-START.request is issued. Note: that this
test is similar to TP/154/MAC/WARM-START-01 with an additional step to
"start" the device under test as a coordinator.

*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac.h"

#define USE_ZB_MLME_START_CONFIRM
#define USE_ZB_MLME_SET_CONFIRM
#include "zb_mac_only_stubs.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"
                                                                
#define CHANNEL 0x14
#define MY_PAN 0x1aaa
static zb_ieee_addr_t g_ffd0_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};
static zb_ieee_addr_t g_ffd1_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};

static zb_short_t state = 0;


MAIN()
{
  ARGV_UNUSED;

  ZB_INIT("warm_start_02_ffd0", argv[1], argv[2]);

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ffd0_addr);
  MAC_PIB().mac_pan_id = MY_PAN;
  ZB_PIB_SHORT_ADDRESS() = 0x1122;

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

  while (1)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
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
  else
  {
    zb_free_buf(ZB_BUF_FROM_REF(param));
  }

  state++;
}


void zb_mlme_start_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK2, "zb_mlme_start_confirm", (FMT__0));
  zb_free_buf(ZB_BUF_FROM_REF(param));
}

/*! @} */
