/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac - TP/154/MAC/DATA-03, FFD0

Check D.U.T. correctly receives data frame sent to broadcast short address on different PAN.
Tester is coordinator for PAN 1, DUT is coordinator for PAN2 

*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac.h"

#define USE_ZB_MLME_START_CONFIRM
#define USE_ZB_MCPS_DATA_CONFIRM
#define USE_ZB_MCPS_DATA_INDICATION
#include "zb_mac_only_stubs.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

#define CHANNEL 0x16
#define MY_PAN 0x1aaa
#define FFD1_PAN 0x1bbb
static zb_ieee_addr_t g_zc_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};
static zb_ieee_addr_t g_rfd_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};

static zb_short_t state = 0;

MAIN()
{
  ARGV_UNUSED;

  ZB_INIT("data_03_ffd0", argv[1], argv[2]);

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_zc_addr);
  MAC_PIB().mac_pan_id = MY_PAN;
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


void zb_mcps_data_indication(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_MAC1, ">> zb_mcps_data_indication param %hd", (FMT__H, param));
  
  state++;
  TRACE_MSG(TRACE_MAC1, "incoming data state %hd, len %hd", (FMT__H_H, state, ZB_BUF_LEN(buf)));

  if (state == 7)
  {
    /* Tester to D.U.T: Short Address to Short Address, Unicast, no ACK  */

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
      data_req->dst_pan_id = FFD1_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = 0;
    }

    TRACE_MSG(TRACE_MAC1, "Tester to D.U.T: Short Address to Short Address, Unicast, no ACK", (FMT__0));
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

  state++;

  TRACE_MSG(TRACE_NWK2, "zb_mcps_data_confirm param %hd handle 0x%hx status 0x%hx state %hd",
            (FMT__H_H_H_H, (zb_uint8_t)param, (zb_uint8_t)confirm_params->msdu_handle,
             (zb_uint8_t)ZB_BUF_FROM_REF(param)->u.hdr.status, state));

  switch (state)
  {
    case 8:
      /* Tester to D.U.T: Short Address to Short Address, Unicast, with ACK  */

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

      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_addr.addr_short = 0x3344;
      data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->src_addr.addr_short = 0x1122;
      data_req->dst_pan_id = FFD1_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }

    TRACE_MSG(TRACE_MAC1, "Tester to D.U.T: Short Address to Short Address, Unicast, with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;

    case 9:
      /* Tester to D.U.T: Short Address to Extended Address, Unicast, with ACK  */

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
      data_req->dst_pan_id = FFD1_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }

    TRACE_MSG(TRACE_MAC1, "Tester to D.U.T: Short Address to Extended Address, Unicast, with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;


    case 10:
      /* Tester to D.U.T: Extended Address to Short Address, Unicast, with ACK  */

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

      data_req->src_addr_mode = ZB_ADDR_64BIT_DEV;
      ZB_IEEE_ADDR_COPY(data_req->src_addr.addr_long, g_zc_addr);
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_addr.addr_short = 0x3344;
      data_req->dst_pan_id = FFD1_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }

    TRACE_MSG(TRACE_MAC1, "Tester to D.U.T: Extended Address to Short Address, Unicast, with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;


    case 11:
      /* Tester to D.U.T: Extended Address to Extended Address, Unicast, with ACK */
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
      data_req->dst_pan_id = FFD1_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }

    TRACE_MSG(TRACE_MAC1, "Tester to D.U.T: Extended Address to Extended Address, Unicast, with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);


    break;

    case 12:
      /* Tester to D.U.T: Extended Address to Broadcast Address  */

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

      data_req->src_addr_mode = ZB_ADDR_64BIT_DEV;
      ZB_IEEE_ADDR_COPY(data_req->src_addr.addr_long, g_zc_addr);
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_addr.addr_short = 0xffff;
      data_req->dst_pan_id = FFD1_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = 0;
    }

    TRACE_MSG(TRACE_MAC1, "Tester to D.U.T: Extended Address to Broadcast Address", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;

    case 13:
      /* Tester to D.U.T: Short Address to Broadcast Address */
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
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_addr.addr_short = 0xffff;
      data_req->dst_pan_id = FFD1_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = 0;
    }

    TRACE_MSG(TRACE_MAC1, "Tester to D.U.T: Short Address to Broadcast Address", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;
  }

}


/*! @} */
