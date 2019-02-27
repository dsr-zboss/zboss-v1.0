/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Test for route discovery
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_ringbuffer.h"
#include "zb_bufpool.h"
#include "zb_mac_transport.h"
#include "zb_nwk.h"
#include "zb_secur.h"

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

void zb_init_wo_aps(zb_char_t *trace_comment, zb_char_t *rx_pipe, zb_char_t *tx_pipe) ZB_CALLBACK;


void route_discovery_request();
void route_data_request();

static int g_route_dest_address = 0;

void usage(char **argv)
{
#ifndef ZB8051
#ifdef NWK_ROUTE_SOURCE
  printf("%s <read pipe path> <write pipe path> <route dest>\n", argv[0]);
#else
  printf("%s <read pipe path> <write pipe path>\n", argv[0]);
#endif
#else
  (void)argv;
#endif
}


void zb_nlme_permit_joining_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK1, "permit joining confirm status %d", (FMT__D, ((zb_buf_t *)ZB_BUF_FROM_REF(param))->u.hdr.status));
  zb_free_buf((zb_buf_t *)ZB_BUF_FROM_REF(param));

#ifdef NWK_ROUTE_SOURCE
#if 0
    route_discovery_request();
#else
    route_data_request();
#endif
#endif
}


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

#if 1
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
#endif

}

void zb_nlme_join_indication(zb_uint8_t param) ZB_CALLBACK
{
  zb_nlme_join_indication_t *ind = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_nlme_join_indication_t);
  TRACE_MSG(TRACE_NWK1,
            "joined (status %d) dev %d/" TRACE_FORMAT_64 " cap: dev type %d rx on when idle. %d rejoin %d secure %d", (FMT__D_D_A_D_D_D_D,
            ((zb_buf_t *)ZB_BUF_FROM_REF(param))->u.hdr.status,
            ind->network_address, TRACE_ARG_64(ind->extended_address),
            ZB_MAC_CAP_GET_DEVICE_TYPE(ind->capability_information),
            ZB_MAC_CAP_GET_RX_ON_WHEN_IDLE(ind->capability_information),
            ind->rejoin_network, ind->secure_rejoin));
  zb_free_buf((zb_buf_t *)ZB_BUF_FROM_REF(param));
}

void zb_nlme_join_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_nlme_join_confirm_t *confirm = ZB_GET_BUF_PARAM((zb_buf_t *)ZB_BUF_FROM_REF(param), zb_nlme_join_confirm_t);
  zb_bool_t buf_reused = ZB_FALSE;

  TRACE_MSG(TRACE_NWK1, ">>zb_nlme_join_confirm %p", (FMT__P, ZB_BUF_FROM_REF(param)));

  if (confirm->status == 0)
  {
    TRACE_MSG(TRACE_NWK1, "CONGRATULATIONS! joined %d, nwk_addr %d, ext pan id " TRACE_FORMAT_64 ", active_channel %d", (FMT__D_D_A_D,
              (int)confirm->status, confirm->network_address,
              TRACE_ARG_64(confirm->extended_pan_id),
              (int)confirm->active_channel));

#ifndef NWK_NO_ROUTER
    ZB_BUF_REUSE((zb_buf_t *)ZB_BUF_FROM_REF(param));
    {
      zb_nlme_start_router_request_t *request = ZB_GET_BUF_PARAM((zb_buf_t *)ZB_BUF_FROM_REF(param), zb_nlme_start_router_request_t);
      request->beacon_order = 0;
      request->superframe_order = 0;
      request->battery_life_extension = 0;
      ZB_SCHEDULE_CALLBACK(zb_nlme_start_router_request, param);
      buf_reused = ZB_TRUE;
    }
#endif
  }
  else
  {
    TRACE_MSG(TRACE_NWK1, "join failed status %d", (FMT__D, (int)confirm->status));
  }
  if (!buf_reused)
  {
    zb_free_buf((zb_buf_t*)ZB_BUF_FROM_REF(param));
  }

  TRACE_MSG(TRACE_NWK1, "<<zb_nlme_join_confirm", (FMT__0));
}

void zb_nlme_start_router_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK1, ">> zb_nlme_start_router_confirm", (FMT__0));

  if ( ZB_BUF_FROM_REF(param) )
  {
    zb_nlme_permit_joining_request_t *request = NULL;

    TRACE_MSG(TRACE_NWK1, "start router confirm status %d", (FMT__D, ((zb_buf_t *)ZB_BUF_FROM_REF(param))->u.hdr.status));

    request = ZB_GET_BUF_PARAM((zb_buf_t *)ZB_BUF_FROM_REF(param), zb_nlme_permit_joining_request_t);
    request->permit_duration = 0xff; /* permit forewer */
    ZB_SCHEDULE_CALLBACK(zb_nlme_permit_joining_request, param);
  }

  TRACE_MSG(TRACE_NWK1, "<< zb_nlme_start_router_confirm", (FMT__0));
}


void zb_nlme_network_formation_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK1, ">> zb_nlme_network_formation_confirm", (FMT__0));

  if ( ZB_BUF_FROM_REF(param) )
  {
    zb_nlme_permit_joining_request_t *request = NULL;

    TRACE_MSG(TRACE_NWK1, "formation confirm status %d", (FMT__D, ((zb_buf_t *)ZB_BUF_FROM_REF(param))->u.hdr.status));

    request = ZB_GET_BUF_PARAM((zb_buf_t *)ZB_BUF_FROM_REF(param), zb_nlme_permit_joining_request_t);
    request->permit_duration = 0xff; /* permit forewer */
    ZB_SCHEDULE_CALLBACK(zb_nlme_permit_joining_request, param);
  }

  TRACE_MSG(TRACE_NWK1, "<< zb_nlme_network_formation_confirm", (FMT__0));

}


void route_discovery_request()
{
  zb_buf_t *buf = zb_get_out_buf();
  zb_nlme_route_discovery_request_t *req = ZB_GET_BUF_PARAM(buf, zb_nlme_route_discovery_request_t);

  TRACE_MSG(TRACE_NWK1, ">> route_discovery_request", (FMT__0));

  req->address_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
  req->network_addr = g_route_dest_address;
  req->radius = 3;
  req->no_route_cache = ZB_FALSE;

  /* call nwk API from the scheduler loop */
  ZB_SCHEDULE_CALLBACK(zb_nlme_route_discovery_request, ZB_REF_FROM_BUF(buf));

  TRACE_MSG(TRACE_NWK1, "<< route_discovery_request", (FMT__0));
}


void zb_nlme_route_discovery_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = (zb_buf_t *)ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_NWK1, ">> zb_nlme_route_discovery_confirm buf %p", (FMT__P, buf));

  if ( buf )
  {
    zb_nlme_route_discovery_confirm_t *confirm = (zb_nlme_route_discovery_confirm_t *)ZB_BUF_BEGIN(buf);

    if ( confirm )
    {
      TRACE_MSG(TRACE_NWK1, "confirm %p status %d", (FMT__P_D, confirm, confirm->status));
      TRACE_MSG(TRACE_NWK1, "ROUTE DISCOVERY SUCCESS", (FMT__0));
    }
  }

  zb_free_buf((zb_buf_t *)ZB_BUF_FROM_REF(param));
  TRACE_MSG(TRACE_NWK1, "<< zb_nlme_route_discovery_confirm", (FMT__0));
}

void route_data_request()
{
  zb_nlde_data_req_t req;
  zb_uint8_t *ptr = NULL;
  zb_short_t i;
  zb_buf_t *nsdu;

  req.dst_addr = g_route_dest_address;
  req.radius = 3;
  nsdu = zb_get_out_buf();
  req.addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
  req.nonmember_radius = 0;
  req.discovery_route = 1;
  req.security_enable = 0;
  req.ndsu_handle = 1;

  nsdu->u.hdr.handle = 0x11;
  ZB_BUF_INITIAL_ALLOC(nsdu, 80, ptr);
  for (i = 0 ; i < 80 ; ++i)
  {
    ptr[i] = i % 32 + '0';
  }
  TRACE_MSG(TRACE_NWK3, "Sending nlde_data.request", (FMT__0));

  ZB_MEMCPY(ZB_GET_BUF_TAIL(nsdu, sizeof(zb_nlde_data_req_t)), &req, sizeof(req));
  ZB_SCHEDULE_CALLBACK(zb_nlde_data_request, ZB_REF_FROM_BUF(nsdu));
}

void zb_nlde_data_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = (zb_buf_t *)ZB_BUF_FROM_REF(param);

  TRACE_MSG(TRACE_NWK1, ">> zb_nlde_data_confirm buf %p", (FMT__P, buf));

  if ( buf  )
  {
    TRACE_MSG(TRACE_NWK1, "status %d", (FMT__D, buf->u.hdr.status));
  }

  zb_free_buf((zb_buf_t *)ZB_BUF_FROM_REF(param));
  TRACE_MSG(TRACE_NWK1, "<< zb_nlde_data_confirm buf %p", (FMT__P, buf));
}

void zb_nlde_data_indication(zb_uint8_t param) ZB_CALLBACK
{
  int ret = 0;
  zb_short_t i;
  zb_short_t len;
  zb_uint8_t *ptr;
  zb_buf_t *nsdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);

  TRACE_MSG(TRACE_NWK3, ">>nlde_data_indication: packet %p handle 0x%x", (FMT__P_D, nsdu, nsdu->u.hdr.status));

  /* Remove NWK header from the packet */
  ZB_NWK_HDR_CUT(nsdu, ptr);

  len = ZB_BUF_LEN(nsdu);
  if (len != 80)
  {
    TRACE_MSG(TRACE_ERROR, "Wrong len: %d, wants 80", (FMT__D, len));
    ret = -1;
  }

  for (i = 0 ; i < len ; ++i)
  {
    if (ptr[i] != i % 32 + '0')
    {
      TRACE_MSG(TRACE_ERROR, "Bad ptr[%d]: %d, wants %d", (FMT__D_D_D, i, ptr[i], i % 32 + '0'));
      ret = -1;
    }
  }

  TRACE_MSG(TRACE_NWK1, "ROUTE DISCOVERY SUCCESS", (FMT__0));

  zb_free_buf(nsdu);
}


MAIN()
{
  ARGV_UNUSED;

#ifdef NWK_ROUTE_SOURCE
  if ( argc < 4 )
#else
  if ( argc < 3 )
#endif
  {
    usage(argv);
    return 0;
  }

#ifdef NWK_ROUTE_SOURCE
  g_route_dest_address = atoi(argv[3]);
#endif

#if defined NWK_ROUTE_SOURCE
  zb_init_wo_aps("route_disc_src", argv[1], argv[2]);
#elif defined NWK_ROUTE_DESTINATION
  zb_init_wo_aps("route_disc_dst", argv[1], argv[2]);
#elif defined NWK_ROUTE_COORDINATOR
  zb_init_wo_aps("route_disc_coord", argv[1], argv[2]);
#elif defined NWK_NO_ROUTER
  zb_init_wo_aps("route_disc_no_route", argv[1], argv[2]);
#else
  zb_init_wo_aps("route_disc", argv[1], argv[2]);
#endif

  /* Let's be router */
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ROUTER;

#ifdef NWK_ROUTE_COORDINATOR
  /* formation */
  {
    zb_buf_t *buf = zb_get_out_buf();
    zb_nlme_network_formation_request_t *req = ZB_GET_BUF_PARAM(buf, zb_nlme_network_formation_request_t);

    /* Let's be ZC */
    ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_COORDINATOR;

    req->scan_channels = ~0;
    req->scan_duration = 1;
    ZB_SCHEDULE_CALLBACK(zb_nlme_network_formation_request, ZB_REF_FROM_BUF(buf));
  }
#else
  /* join */
  {
    zb_buf_t *buf = zb_get_out_buf();
    zb_nlme_network_discovery_request_t *req = ZB_GET_BUF_PARAM(buf, zb_nlme_network_discovery_request_t);

    req->scan_channels = (1<<12)|(1<<13);
    req->scan_duration = 4;
    ZB_SCHEDULE_CALLBACK(zb_nlme_network_discovery_request, ZB_REF_FROM_BUF(buf));
  }
#endif

  while (1)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}

/* just to compile... */
void zb_apsde_data_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
  TRACE_MSG(TRACE_NWK1, "zb_apsde_data_confirm", (FMT__0));
}

void zb_apsde_data_indication(zb_uint8_t param)
{
  ZVUNUSED(param);
  TRACE_MSG(TRACE_NWK1, "zb_apsde_data_indication", (FMT__0));
}

void zb_nlme_ed_scan_confirm(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
  TRACE_MSG(TRACE_NWK1, "zb_nlme_ed_scan_confirm", (FMT__0));
}




void zb_nlme_reset_confirm(zb_uint8_t param)
{
  (void)param;
}

void zb_nlme_sync_confirm(zb_uint8_t param) ZB_CALLBACK
{
  (void)param;
}

zb_ret_t zb_nwk_secure_frame(zb_buf_t *src, zb_uint_t mac_hdr_size, zb_buf_t *dst)
{
  (void)src;
  (void)dst;
  (void)mac_hdr_size;
  return 0;
}


zb_ret_t zb_nwk_unsecure_frame(zb_uint8_t param, zb_mac_mhr_t *mhr, zb_uint8_t mhr_len)
{
  (void)param;
  (void)mhr;
  (void)mhr_len;
  return 0;
}


void zb_aps_secure_frame(zb_buf_t *src, zb_uint_t mac_hdr_size, zb_buf_t *dst)
{
  (void)src;
  (void)dst;
  (void)mac_hdr_size;
}

void secur_generate_keys()
{
}

zb_bool_t secur_has_preconfigured_key()
{
  return ZB_FALSE;
}

void zb_zdo_check_fails(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
}

void zb_nlme_status_indication(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
}

void zb_apsme_update_device_request(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_leave_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_leave_indication(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_mlme_set_confirm(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
}

void zb_aps_init()
{
}

void zb_zdo_init()
{
}



/*! @} */
