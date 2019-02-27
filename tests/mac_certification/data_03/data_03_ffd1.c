/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac - TP/154/MAC/DATA-03, FFD1

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

#define FFD0_PAN 0x1aaa
#define MY_PAN 0x1bbb
#define CHANNEL 0x16
static zb_ieee_addr_t g_mac_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};
static zb_ieee_addr_t g_zc_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};

static zb_short_t state = 1;

void poll_alarm(zb_uint8_t param) ZB_CALLBACK;

MAIN()
{
  ARGV_UNUSED;

  ZB_INIT("data_03_ffd1", argv[1], argv[2]);

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_mac_addr);
  ZB_PIB_SHORT_ADDRESS() = 0x3344;
  MAC_PIB().mac_pan_id = MY_PAN;

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

  while(1)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


void zb_mlme_start_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK2, "zb_mlme_start_confirm", (FMT__0));

  /* D.U.T. to tester.: Short Address to Short Address, Unicast, no ACK  */
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

    data_req->dst_addr.addr_short = 0x1122;
    data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
    data_req->src_addr.addr_short = 0x3344;
    data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
    data_req->dst_pan_id = FFD0_PAN;
    data_req->msdu_handle = 0xa;
    data_req->tx_options = 0;
  }

  TRACE_MSG(TRACE_MAC1, "D.U.T. to tester.: Short Address to Short Address, Unicast, no ACK", (FMT__0));
  ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);
}


void zb_mcps_data_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mcps_data_confirm_params_t *confirm_params = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_confirm_params_t);
  TRACE_MSG(TRACE_NWK2, "zb_mcps_data_confirm param %hd handle 0x%hx status 0x%hx",
            (FMT__H_H_H, (zb_uint8_t)param, (zb_uint8_t)confirm_params->msdu_handle, (zb_uint8_t)ZB_BUF_FROM_REF(param)->u.hdr.status));


  switch (state)
  {
    case 1:
      /* D.U.T. to tester.: Short Address to Short Address, Unicast, with ACK  */
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

      data_req->dst_addr.addr_short = 0x1122;
      data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->src_addr.addr_short = 0x3344;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = FFD0_PAN;
      data_req->msdu_handle = 0xa;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }
    TRACE_MSG(TRACE_MAC1, "D.U.T. to tester.: Short Address to Short Address, Unicast, with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;

    case 2:
      /* D.U.T. to tester.: Short Address to Extended Address, Unicast with ACK  */

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

      data_req->src_addr.addr_short = 0x3344;
      data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      ZB_IEEE_ADDR_COPY(data_req->dst_addr.addr_long, g_zc_addr);
      data_req->dst_addr_mode = ZB_ADDR_64BIT_DEV;
      data_req->dst_pan_id = FFD0_PAN;
      data_req->msdu_handle = 0xc;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }
    TRACE_MSG(TRACE_MAC1, "D.U.T. to tester.: Short Address to Extended Address, Unicast with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;

    case 3:
      /* D.U.T. to tester.: Extended Address to Short Address , Unicast with ACK  */

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

      ZB_IEEE_ADDR_COPY(data_req->src_addr.addr_long, g_mac_addr);
      data_req->src_addr_mode = ZB_ADDR_64BIT_DEV;
      data_req->dst_addr.addr_short = 0x1122;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = FFD0_PAN;
      data_req->msdu_handle = 0xc;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }
    TRACE_MSG(TRACE_MAC1, "D.U.T. to tester.: Extended Address to Short Address , Unicast with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;


    case 4:
      /* D.U.T. to tester.: Extended Address to Extended Address , Unicast with ACK  */

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

      ZB_IEEE_ADDR_COPY(data_req->src_addr.addr_long, g_mac_addr);
      data_req->src_addr_mode = ZB_ADDR_64BIT_DEV;
      ZB_IEEE_ADDR_COPY(data_req->dst_addr.addr_long, g_zc_addr);
      data_req->dst_addr_mode = ZB_ADDR_64BIT_DEV;
      data_req->dst_pan_id = FFD0_PAN;
      data_req->msdu_handle = 0xc;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }
    TRACE_MSG(TRACE_MAC1, "D.U.T. to tester.: Extended Address to Extended Address , Unicast with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;

    case 5:
      /* D.U.T. to tester.: Extended Address to Broadcast Address  */

    {
      zb_uint8_t *pl;
      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 5, pl);
      pl[0] = 0x6c;
      pl[1] = 0x7d;
      pl[2] = 0x8e;
      pl[3] = 0x9f;
      pl[4] = 0xa0;
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      ZB_IEEE_ADDR_COPY(data_req->src_addr.addr_long, g_mac_addr);
      data_req->src_addr_mode = ZB_ADDR_64BIT_DEV;
      data_req->dst_addr.addr_short = 0xffff;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = FFD0_PAN;
      data_req->msdu_handle = 0xc;
      data_req->tx_options = 0;
    }
    TRACE_MSG(TRACE_MAC1, "D.U.T. to tester.: Extended Address to Broadcast Address", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;

    case 6:
      /* D.U.T. to tester.: Short Address to Broadcast Address */

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

      data_req->src_addr.addr_short = 0x3344;
      data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_addr.addr_short = 0xffff;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = FFD0_PAN;
      data_req->msdu_handle = 0xb;
      data_req->tx_options = 0;
    }

    TRACE_MSG(TRACE_MAC1, "D.U.T. to tester.: Short Address to Broadcast Address", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);
    break;
  }

  state++;
}

void zb_mcps_data_indication(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_MAC1, ">> zb_mcps_data_indication param %hd", (FMT__H, param));
  zb_free_buf(ZB_BUF_FROM_REF(param));
}



/*! @} */
