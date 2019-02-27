/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac data - FFD
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac.h"

#define USE_ZB_MLME_START_CONFIRM
#define USE_ZB_MCPS_DATA_CONFIRM
#define USE_ZB_MLME_ASSOCIATE_INDICATION
#define USE_ZB_MCPS_DATA_INDICATION
#include "zb_mac_only_stubs.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

#define CHANNEL 0x14
#define PAN_ID 0x1aaa
static zb_ieee_addr_t g_zc_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};
static zb_ieee_addr_t g_rfd_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};

static zb_short_t state = 0;

MAIN()
{
  ARGV_UNUSED;

  ZB_INIT("data_01_ffd", argv[1], argv[2]);
/*
  Security: no for all the devices
  Channel: 0x14 for all the devices
  PAN id: 0x1AAA for all the devices
  MAC: 0xACDE480000000001
*/

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_zc_addr);
  MAC_PIB().mac_pan_id = PAN_ID;
  ZB_PIB_SHORT_ADDRESS() = 0x1122;

  {
    zb_buf_t *buf = zb_get_out_buf();

    zb_mlme_start_req_t *req = ZB_GET_BUF_PARAM(buf, zb_mlme_start_req_t);

    ZB_BZERO(req, sizeof(*req));
    req->pan_id = MAC_PIB().mac_pan_id;
    req->logical_channel = CHANNEL;
    req->pan_coordinator = 1;      /* will be coordinator */
    req->beacon_order = ZB_TURN_OFF_ORDER;
    req->superframe_order = ZB_TURN_OFF_ORDER;

    ZB_SCHEDULE_CALLBACK(zb_mlme_start_request, ZB_REF_FROM_BUF(buf));
  }

  while (1)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


void zb_mlme_start_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK2, "zb_mlme_start_confirm", (FMT__0));

  zb_free_buf(ZB_BUF_FROM_REF(param));
}


void zb_mlme_associate_indication(zb_uint8_t param) ZB_CALLBACK
{
  zb_ieee_addr_t device_address;
  zb_mlme_associate_indication_t *request = ZB_GET_BUF_PARAM((zb_buf_t *)ZB_BUF_FROM_REF(param), zb_mlme_associate_indication_t);
  TRACE_MSG(TRACE_NWK1, ">>mlme_associate_ind %hd", (FMT__H, param));
  /*
    Very simple implementation: accept anybody, assign address 0x3344
   */
  ZB_IEEE_ADDR_COPY(device_address, request->device_address);

  ZB_MLME_BUILD_ASSOCIATE_RESPONSE(ZB_BUF_FROM_REF(param), device_address, 0x3344, 0);

  ZB_SCHEDULE_CALLBACK(zb_mlme_associate_response, param);

  TRACE_MSG(TRACE_NWK1, "<<mlme_associate_ind", (FMT__0));
}


void zb_mcps_data_indication(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_MAC1, ">> zb_mcps_data_indication param %hd", (FMT__H, param));
  
  state++;
  TRACE_MSG(TRACE_MAC1, "incoming data state %hd, len %hd", (FMT__H_H, state, ZB_BUF_LEN(buf)));

  if (state == 8)
  {
    /* i. Tester to D.U.T: Short Address to Short Address, Indirect, with ACK  */

    {
      zb_uint8_t *pl;

      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 5, pl);
      pl[0] = 0x00;
      pl[1] = 0x01;
      pl[2] = 0x02;
      pl[3] = 0x03;
      pl[4] = 0x04;
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      data_req->dst_addr.addr_short = 0x3344;
      data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->src_addr.addr_short = 0x1122;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = PAN_ID;
      data_req->msdu_handle = 0;
      data_req->tx_options = (MAC_TX_OPTION_INDIRECT_TRANSMISSION_BIT | MAC_TX_OPTION_ACKNOWLEDGED_BIT);
    }

    TRACE_MSG(TRACE_MAC1, "i. Tester to D.U.T: Short Address to Short Address, Indirect, with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);
  }
  else
  {
    zb_free_buf(ZB_BUF_FROM_REF(param));
  }
}


void zb_mcps_data_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mcps_data_confirm_params_t *confirm_params = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_confirm_params_t);
  TRACE_MSG(TRACE_NWK2, "zb_mcps_data_confirm param %hd handle 0x%hx status 0x%hx state %hd",
            (FMT__H_H_H_H, (zb_uint8_t)param, (zb_uint8_t)confirm_params->msdu_handle,
             (zb_uint8_t)ZB_BUF_FROM_REF(param)->u.hdr.status, state));

  state++;
  switch (state)
  {
    case 9:
      /* j. Tester to D.U.T: Short Address to Extended Address, Indirect, with ACK */
    {
      zb_uint8_t *pl;

      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 5, pl);
      pl[0] = 0x00;
      pl[1] = 0x01;
      pl[2] = 0x02;
      pl[3] = 0x03;
      pl[4] = 0x04;
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->src_addr.addr_short = 0x1122;
      ZB_IEEE_ADDR_COPY(data_req->dst_addr.addr_long, g_rfd_addr);
      data_req->dst_addr_mode = ZB_ADDR_64BIT_DEV;
      data_req->dst_pan_id = PAN_ID;
      data_req->msdu_handle = 0xd;
      data_req->tx_options = (MAC_TX_OPTION_INDIRECT_TRANSMISSION_BIT | MAC_TX_OPTION_ACKNOWLEDGED_BIT);
    }

    TRACE_MSG(TRACE_MAC1, "j. Tester to D.U.T: Short Address to Extended Address, Indirect, with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;

    case 10:
      /* k. Tester to D.U.T: Extended Address to Short Address, Indirect, with ACK  */

    {
      zb_uint8_t *pl;

      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 5, pl);
      pl[0] = 0x00;
      pl[1] = 0x01;
      pl[2] = 0x02;
      pl[3] = 0x03;
      pl[4] = 0x04;
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      ZB_IEEE_ADDR_COPY(data_req->src_addr.addr_long, g_zc_addr);
      data_req->src_addr_mode = ZB_ADDR_64BIT_DEV;
      data_req->dst_addr.addr_short = 0x3344;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = PAN_ID;
      data_req->msdu_handle = 0xd;
      data_req->tx_options = (MAC_TX_OPTION_INDIRECT_TRANSMISSION_BIT | MAC_TX_OPTION_ACKNOWLEDGED_BIT);
    }

    TRACE_MSG(TRACE_MAC1, "k. Tester to D.U.T: Extended Address to Short Address, Indirect, with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;

    case 11:
      /* l. Tester to D.U.T: Extended Address to Extended Address, Indirect, with ACK */
    {
      zb_uint8_t *pl;

      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 5, pl);
      pl[0] = 0x00;
      pl[1] = 0x01;
      pl[2] = 0x02;
      pl[3] = 0x03;
      pl[4] = 0x04;
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      ZB_IEEE_ADDR_COPY(data_req->src_addr.addr_long, g_zc_addr);
      data_req->src_addr_mode = ZB_ADDR_64BIT_DEV;
      ZB_IEEE_ADDR_COPY(data_req->dst_addr.addr_long, g_rfd_addr);
      data_req->dst_addr_mode = ZB_ADDR_64BIT_DEV;
      data_req->dst_pan_id = PAN_ID;
      data_req->msdu_handle = 0xd;
      data_req->tx_options = (MAC_TX_OPTION_INDIRECT_TRANSMISSION_BIT | MAC_TX_OPTION_ACKNOWLEDGED_BIT);
    }

    TRACE_MSG(TRACE_MAC1, "l. Tester to D.U.T: Extended Address to Extended Address, Indirect, with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);


    break;

    case 12:
      /* m. Tester to D.U.T: Short Address to Short Address, Indirect, with ACK, maximum data size */
    {
      zb_uint8_t *pl;

      zb_ushort_t i;

      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 74, pl);
      for (i = 0 ; i < 74 ; ++i)
      {
        pl[i] = i;
      }
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      data_req->dst_addr.addr_short = 0x3344;
      data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->src_addr.addr_short = 0x1122;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = PAN_ID;
      data_req->msdu_handle = 0xd;
      data_req->tx_options = (MAC_TX_OPTION_INDIRECT_TRANSMISSION_BIT | MAC_TX_OPTION_ACKNOWLEDGED_BIT);
    }

    TRACE_MSG(TRACE_MAC1, "m. Tester to D.U.T: Short Address to Short Address, Indirect, with ACK, maximum data size", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;
  }

}


/*! @} */
