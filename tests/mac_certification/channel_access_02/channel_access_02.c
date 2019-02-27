/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: 5.1. TP/154/MAC/CHANNEL-ACCESS-02
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac.h"

#define USE_ZB_MLME_RESET_CONFIRM
#define USE_ZB_MLME_SET_CONFIRM
#define USE_ZB_MLME_START_CONFIRM
#define USE_ZB_MCPS_DATA_CONFIRM
#include "zb_mac_only_stubs.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"


MAIN()
{
  ARGV_UNUSED;

  ZB_INIT("channel_access_02", argv[1], argv[2]);
/*
  Security: no for all the devices
  Channel: 0x14 for all the devices
  PAN id: 0x1AAA for all the devices
  MAC: 0xACDE480000000001
*/

  {
    static zb_ieee_addr_t g_zc_addr = {0xac, 0xde, 0x48, 0x00, 0x00, 0x00, 0x00, 0x01};
    ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_zc_addr);
  }
  MAC_PIB().mac_pan_id = 0x1aaa;

  {
    zb_buf_t *buf = zb_get_out_buf();
    zb_mlme_reset_request_t *reset_req = ZB_GET_BUF_PARAM(buf, zb_mlme_reset_request_t);
    reset_req->set_default_pib = 0;

    ZB_SCHEDULE_CALLBACK(zb_mlme_reset_request, ZB_REF_FROM_BUF(buf));
  }

  while(1)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


void zb_mlme_reset_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mlme_set_request_t *req;

  TRACE_MSG(TRACE_NWK2, "zb_mlme_reset_confirm", (FMT__0));

  ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), sizeof(zb_mlme_set_request_t) + sizeof(zb_uint16_t), req);
  req->pib_attr = ZB_PIB_ATTRIBUTE_COORD_SHORT_ADDRESS;
  req->pib_length = sizeof(zb_uint16_t);
  *((zb_uint16_t *)(req + 1)) = 0x0;

  ZB_SCHEDULE_CALLBACK(zb_mlme_set_request, param);
}


void zb_mlme_set_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mlme_start_req_t *req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_start_req_t);

  TRACE_MSG(TRACE_NWK2, "zb_mlme_set_confirm", (FMT__0));

  ZB_BZERO(req, sizeof(*req));
  req->pan_id = MAC_PIB().mac_pan_id;
  req->logical_channel = 0x14;
  req->pan_coordinator = 1;      /* will be coordinator */
  req->beacon_order = ZB_TURN_OFF_ORDER;
  req->superframe_order = ZB_TURN_OFF_ORDER;

  ZB_SCHEDULE_CALLBACK(zb_mlme_start_request, param);
}


void zb_mlme_start_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK2, "zb_mlme_start_confirm", (FMT__0));

  {
    zb_uint8_t *pl;

    ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 10, pl);
    pl[0] = 0x00;
    pl[1] = 0x01;
    pl[2] = 0x02;
    pl[3] = 0x03;
    pl[4] = 0x04;
    pl[5] = 0x05;
    pl[6] = 0x06;
    pl[7] = 0x07;
    pl[8] = 0x08;
    pl[9] = 0x09;
    pl[10] = 0x0a;
  }
  {
    zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

    data_req->dst_addr = 0x1234;
    data_req->src_addr = 0x0;
    data_req->msdu_handle = 0;
    data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
  }

  ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);
}


void zb_mcps_data_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mcps_data_confirm_params_t *confirm_params = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_confirm_params_t);
  TRACE_MSG(TRACE_NWK2, "zb_mcps_data_confirm param %hd handle 0x%hx status 0x%hx",
            (FMT__H_H_H, (zb_uint8_t)param, (zb_uint8_t)confirm_params->msdu_handle, (zb_uint8_t)ZB_BUF_FROM_REF(param)->u.hdr.status));
  zb_free_buf(ZB_BUF_FROM_REF(param));
}



/*! @} */
