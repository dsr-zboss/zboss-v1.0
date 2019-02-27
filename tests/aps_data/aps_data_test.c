/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Test for trivial APS data transfer
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_ringbuffer.h"
#include "zb_bufpool.h"
#include "zb_mac_transport.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

#if defined FIRST_TEST
  #define MY_ADDR   0x1
  #define PEER_ADDR 0x2
  #define LOG_FILE "aps_test_first"
#elif defined SECOND_TEST
  #define MY_ADDR   0x2
  #define PEER_ADDR 0x1
  #define LOG_FILE "aps_test_second"
#else
  #error "Test built without define"
#endif

extern zb_io_ctx_t g_io_ctx;

/**
   \par APS data test.

   This test sends 80-bytes packet and receives 80-bytes packet.
   Supposed that 2 tests exemplars connected via ns-3.

 */

void func1(zb_uint8_t param1) ZB_CALLBACK
{
  zb_apsde_data_req_t req;
  zb_uint8_t *ptr = NULL;
  zb_short_t i;
  zb_buf_t *asdu;

  ZVUNUSED(param1);

  asdu = zb_get_out_buf();

  req.dst_addr.addr_short = PEER_ADDR;
  req.addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  req.tx_options = 0/*ZB_APSDE_TX_OPT_ACK_TX*/; // aps retransmissions not fixed debug
  req.radius = 1;
  req.profileid = 2;
  req.src_endpoint = 0;
  req.dst_endpoint = 0;

  if (!asdu)
  {
    TRACE_MSG(TRACE_ERROR, "out buf alloc failed!", (FMT__0));
  }
  else
  {
    asdu->u.hdr.handle = 0x11;
    ZB_BUF_INITIAL_ALLOC(asdu, 80, ptr);

    for (i = 0 ; i < 80 ; ++i)
    {
      ptr[i] = i % 32 + '0';
    }
    TRACE_MSG(TRACE_APS3, "Sending apsde_data.request", (FMT__0));

    ZB_MEMCPY(
      ZB_GET_BUF_TAIL(asdu, sizeof(req)),
      &req, sizeof(req));
    ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(asdu));
  }
}


#if 0
/*
  Link without APS.
  Define here APS routines.
*/

void zb_apsde_data_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *asdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);
  TRACE_MSG(TRACE_APS3, "apsde_data_confirm: packet %p status %d", (FMT__P_D,
            asdu, asdu->u.hdr.status));

  /* FIXME: Seems this buffer removed twice */
  zb_free_buf(asdu);
}
#endif

void aps_test_data_indication(zb_uint8_t param) ZB_CALLBACK
{
  int ret = 0;
  zb_short_t i;
  zb_short_t len;
  zb_uint8_t *ptr;
  zb_buf_t *asdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);

  TRACE_MSG(TRACE_APS3, "apsde_data_indication: packet %p handle 0x%x", (FMT__P_D,
            asdu, asdu->u.hdr.status));

  /* Remove APS header from the packet */
  ZB_APS_HDR_CUT_P(asdu, ptr);
  len = ZB_BUF_LEN(asdu);

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

  if ( !ret )
  {
    TRACE_MSG(TRACE_ERROR, "SUCCESS!!!", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "ERROR!!!", (FMT__0));
  }

  zb_free_buf(asdu);
}


void usage(char **argv)
{
#ifndef ZB8051
  printf("%s <read pipe path> <write pipe path>\n", argv[0]);
#else
  ZVUNUSED(argv);
#endif
}


MAIN()
{
  zb_int_t i;

#ifndef KEIL
  if ( argc < 3 )
  {
    usage(argv);
    return 0;
  }
#endif

#ifndef KEIL
  ZB_INIT(LOG_FILE, argv[1], argv[2]);
#else
  ZB_INIT(LOG_FILE, NULL, NULL);
#endif

  /* In the real life network address will be assigned at network join; now
   * hardcode it. */
  ZB_NIB_NETWORK_ADDRESS() = MY_ADDR;
  ZG->nwk.handle.joined = ZB_TRUE;

  /* call nwk API from the scheduler loop */
  ZB_SCHEDULE_CALLBACK(func1, 0);

  for (i = 0 ; i < 10 ; ++i)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();
  MAIN_RETURN(0);
}


/* just to compile... */

void zb_zdo_startup_complete(zb_uint8_t param)
{
  (void)param;
  zb_af_set_data_indication(aps_test_data_indication);
}

#if 0
void zb_nlme_network_discovery_confirm(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
}

void zb_nlme_network_formation_confirm(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
}

void zb_nlme_join_indication(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_join_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}
#endif


/*! @} */
