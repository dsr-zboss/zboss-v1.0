/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Test for Discovery
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_ringbuffer.h"
#include "zb_bufpool.h"
#include "zb_mac_transport.h"
#include "zb_nwk.h"
#include "zb_aps.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

static int g_rejoin = 0;
static int g_orphan_rejoin = 0;

void zb_nlme_network_discovery_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_nlme_network_discovery_confirm_t *cnf;
  zb_nlme_network_descriptor_t *dsc;
  zb_uint_t i;

  TRACE_MSG(TRACE_NWK1, "disc status %d", (FMT__D, ((zb_buf_t *)ZB_BUF_FROM_REF(param))->u.hdr.status));
  cnf = (zb_nlme_network_discovery_confirm_t *)ZB_BUF_BEGIN((zb_buf_t *)ZB_BUF_FROM_REF(param));
  dsc = (zb_nlme_network_descriptor_t *)(cnf + 1);

  TRACE_MSG(TRACE_NWK1, "Discovery result: status %d, network_count %d", (FMT__D_D,
            (int)cnf->status, (int)cnf->network_count));

  for (i = 0 ; i < cnf->network_count ; ++i)
  {
    TRACE_MSG(TRACE_NWK1,
              "net %d: xpanid " TRACE_FORMAT_64 ", chan %d, s.prof %d, zb v %d, beacon_order %d, superframe_order %d, permit_joining %d, router_capacity %d, end_device_capacity %d", (FMT__D_A_D_D_D_D_D_D_D_D,
              i, TRACE_ARG_64(dsc->extended_pan_id),
              (int)dsc->logical_channel, (int)dsc->stack_profile, (int)dsc->zigbee_version,
              (int)dsc->beacon_order,
              (int)dsc->superframe_order, (int)dsc->permit_joining,
              (int)dsc->router_capacity, (int)dsc->end_device_capacity));
    dsc++;
  }

  /* In ns-3 test we have only one PAN. Anyway. connect to the first found PAN */

  if ( !g_rejoin )
  {
    /* Now join thru Association */
    if (cnf->network_count > 0)
    {
      zb_nlme_join_request_t *req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_nlme_join_request_t);

      dsc = (zb_nlme_network_descriptor_t *)(cnf + 1);
      ZB_MEMSET(req, 0, sizeof(*req)); /* all defaults to 0 */
      ZB_EXTPANID_COPY(req->extended_pan_id, dsc->extended_pan_id);
      ZB_MAC_CAP_SET_DEVICE_TYPE(req->capability_information, 1); /* join as router */
      ZB_MAC_CAP_SET_POWER_SOURCE(req->capability_information, 1);
      ZB_MAC_CAP_SET_RX_ON_WHEN_IDLE(req->capability_information, 1);
      ZB_MAC_CAP_SET_ALLOCATE_ADDRESS(req->capability_information, 1);

      ZB_SCHEDULE_CALLBACK(zb_nlme_join_request, param);
    }
  }
  else
  {
    /* Join throught rejoin */
    if (cnf->network_count > 0)
    {
      zb_nlme_join_request_t *req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_nlme_join_request_t);
      dsc = (zb_nlme_network_descriptor_t *)(cnf + 1);

      /* add extended address of potential parent to emulate that we've already
       * been connected to it */
      {
        zb_ieee_addr_t ieee_address;
        zb_address_ieee_ref_t ref;

        ZB_64BIT_ADDR_ZERO(ieee_address);
        ieee_address[7] = 8;

        zb_address_update(ieee_address, 0, ZB_FALSE, &ref);
      }

      ZB_MEMSET(req, 0, sizeof(*req)); /* all defaults to 0 */
      ZB_EXTPANID_COPY(req->extended_pan_id, dsc->extended_pan_id);
      ZB_MAC_CAP_SET_DEVICE_TYPE(req->capability_information, 1); /* join as router */
      ZB_MAC_CAP_SET_POWER_SOURCE(req->capability_information, 1);
      ZB_MAC_CAP_SET_RX_ON_WHEN_IDLE(req->capability_information, 1);
      ZB_MAC_CAP_SET_ALLOCATE_ADDRESS(req->capability_information, 1);

      req->rejoin_network = ZB_NLME_REJOIN_METHOD_REJOIN;
      ZB_SCHEDULE_CALLBACK(zb_nlme_join_request, param);
    }
  }
}


void send_data(zb_buf_t *buf);

void zb_nlme_join_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_nlme_join_confirm_t *confirm = ZB_GET_BUF_PARAM((zb_buf_t *)ZB_BUF_FROM_REF(param), zb_nlme_join_confirm_t);

  TRACE_MSG(TRACE_NWK1, ">>zb_nlme_join_confirm %hd", (FMT__H, param));

  if (confirm->status == 0)
  {
    TRACE_MSG(TRACE_NWK1, "CONGRATULATIONS! joined %d, nwk_addr %d, ext pan id " TRACE_FORMAT_64 ", active_channel %d", (FMT__D_D_A_D,
              (int)confirm->status, confirm->network_address,
              TRACE_ARG_64(confirm->extended_pan_id),
              (int)confirm->active_channel));
    send_data((zb_buf_t *)ZB_BUF_FROM_REF(param));
  }
  else
  {
    TRACE_MSG(TRACE_NWK1, "join failed status %d", (FMT__D, (int)confirm->status));
  }

  TRACE_MSG(TRACE_NWK1, "<<zb_nlme_join_confirm", (FMT__0));
}


void send_data(zb_buf_t *buf)
{
  zb_apsde_data_req_t *req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
  zb_uint8_t *ptr = NULL;
  zb_short_t i;

  req->dst_addr.addr_short = 0; /* send to ZC */
  req->addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  req->tx_options = 0;          /* no ACK for now */
  req->radius = 1;
  req->profileid = 2;
  req->src_endpoint = 0;
  req->dst_endpoint = 0;

  buf->u.hdr.handle = 0x11;
  ZB_BUF_INITIAL_ALLOC(buf, 80, ptr);

  for (i = 0 ; i < 80 ; ++i)
  {
    ptr[i] = i % 32 + '0';
  }
  TRACE_MSG(TRACE_APS3, "Sending apsde_data.request", (FMT__0));

  ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
}


void usage(char **argv)
{
#ifndef ZB8051
  printf("%s <read pipe path> <write pipe path> [-rejoin]\n", argv[0]);
#else
  (void)argv;
#endif
}


static void parce_cmd(int argc, char **argv)
{
#ifndef KEIL
  int i = 3;
#else
  int i = 1;
#endif

  for ( ; i < argc; i++ )
  {
    if ( !strcmp(argv[i], "-rejoin") )
    {
      g_rejoin = 1;
    }
    else if ( !strcmp(argv[i], "-orphan_rejoin") )
    {
      g_orphan_rejoin = 1;
    }
  }
}

MAIN()
{
  zb_int_t i;
  ARGV_UNUSED;

#ifndef KEIL
  if ( argc < 3 )
  {
    usage(argv);
    return 0;
  }
#endif

#ifndef ZB8051
  ZB_INIT("disc", argv[1], argv[2]);
#else
  ZB_INIT("disc", "1", "1");
#endif

  parce_cmd(argc, argv);

  /* Let's be router */
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ROUTER;

  {
    zb_buf_t *buf = zb_get_out_buf();
    zb_nlme_network_discovery_request_t *req = ZB_GET_BUF_PARAM(buf, zb_nlme_network_discovery_request_t);
/*    req->scan_channels = ~0;*/

    req->scan_channels = (1<<12)|(1<<13); /* scan only 2 channels: be a bit
                                           * faster in ns-3 test */
    /* duration  is
                                   ((1<<duration) + 1) * 15360 / 1000000

                                   For duration 11 ~ 31s
                                   For duration 10 ~ 15s
                                   For duration 8 ~ 4s
                                   For duration 5 ~0.5s
                                   For duration 2 ~0.08s
                                   For duration 1 ~0.05s
                                */
    req->scan_duration = 8;
    /* call nwk API from the scheduler loop */
    ZB_SCHEDULE_CALLBACK(zb_nlme_network_discovery_request, ZB_REF_FROM_BUF(buf));
  }

  for (i = 0 ; i < 1000 ; ++i)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


/* just to compile... */
void zb_apsde_data_confirm(zb_uint8_t param)
{
  /* make orphan rejoin once */
  if ( g_orphan_rejoin )
  {
    zb_nlme_join_request_t *req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_nlme_join_request_t);

    g_orphan_rejoin = 0;

    ZB_MEMSET(req, 0, sizeof(*req)); /* all defaults to 0 */
    ZB_EXTPANID_COPY(req->extended_pan_id, ZB_PIB_EXTENDED_ADDRESS() );
    req->rejoin_network = ZB_NLME_REJOIN_METHOD_DIRECT;
    req->scan_channels = (1<<12)|(1<<13);

    ZB_SCHEDULE_CALLBACK(zb_nlme_join_request, param);
  }
}

#if 0
void zb_apsde_data_indication(zb_uint8_t param)
{
  ZVUNUSED(param);
}
#endif

#ifndef KEIL
void zb_nlme_join_indication(zb_uint8_t param)
{
  ZVUNUSED(param);
}
#endif

void zb_nlme_network_formation_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = (zb_buf_t *)ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_NWK1, ">>zb_nlme_network_formation vbuf %hd", (FMT__H, param));

  if ( buf )
  {
    zb_nlme_network_formation_confirm_t *confirm = (zb_nlme_network_formation_confirm_t *)ZB_BUF_BEGIN(buf);
    ZVUNUSED(confirm);
  }

  TRACE_MSG(TRACE_NWK1, "<<zb_nlme_network_formation", (FMT__0));
}


void zb_aps_in_transport_key(zb_uint8_t param)
{
  (void)param;
}

void zb_aps_in_update_device(zb_uint8_t param)
{
  (void)param;
}

void zb_aps_secure_frame(zb_buf_t *src, zb_uint_t mac_hdr_size, zb_buf_t *dst)
{
  (void)src;
  (void)dst;
  (void)mac_hdr_size;
}

zb_ret_t zb_aps_unsecure_frame(zb_buf_t *buf)
{
  (void)buf;
  return 0;
}

void zb_nwk_secure_frame(zb_buf_t *nsdu, zb_ushort_t hdr_size)
{
  (void)nsdu;
  (void)hdr_size;
}

zb_ret_t zb_nwk_unsecure_frame(zb_buf_t *nsdu)
{
  (void)nsdu;
  return 0;
}

void zb_secur_aps_aux_hdr_fill(zb_uint8_t *p, zb_bool_t nwk_key)
{
  (void)p;
  (void)nwk_key;
}

void zb_zdo_poll_parent(zb_uint8_t param)
{
  (void)param;
}

zb_bool_t zb_aps_command_add_secur(zb_buf_t *buf, zb_uint8_t command_id)
{
  (void)buf;
  (void)command_id;
  return 0;
}

zb_ushort_t zb_aps_secur_aux_size(zb_uint8_t *p)
{
  (void)p;
  return 0;
}

void secur_generate_keys()
{
}


/*! @} */
