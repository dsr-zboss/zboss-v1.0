/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Power control test;
*/

/*! \addtogroup ZB_TESTS */
/*! @{ */

#ifndef ZB_ED_ROLE
#define ZB_ED_ROLE
#endif


#include <intrins.h>
#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"


static void send_data(zb_buf_t *buf);
void data_indication(zb_uint8_t param) ZB_CALLBACK;
void test();

void test()
{
zb_uint8_t i,j;
for (i = 0 ; i < ZB_SECUR_N_SECUR_MATERIAL ; ++i)
  {
    /* active_key_seq_number, active_secur_material_i set to 0 by global init -
       not need to init it here */

    ZG->nwk.nib.secur_material_set[i].key_seq_number = i;
    ZG->nwk.nib.security_level = ZB_SECURITY_LEVEL;
    ZG->nwk.nib.secure_all_frames = ZB_SECURE_ALL_FRAMES;
    ZG->nwk.handle.is_tc = 1;
    ZB_IEEE_ADDR_COPY(ZB_AIB().trust_center_address, ZB_PIB_EXTENDED_ADDRESS());

    for (j = 0 ; j < ZB_CCM_KEY_SIZE ; ++j)
    {
      ZG->nwk.nib.secur_material_set[i].key[j] = (ZB_RANDOM() >> 4) & 0xff;
    }
  }
}

#pragma OT(0);
MAIN()
{
char buffer[5000];
  ARGV_UNUSED;

#ifndef KEIL
  if ( argc < 3 )
  {
    printf("%s <read pipe path> <write pipe path>\n", argv[0]);
    return 0;
  }
#endif

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_ze", argv[1], argv[2]);
#else
  ZB_INIT("zdo_ze", "3", "3");
#endif
zb_uz2400_standby();
zb_uz2400_register_wakeup();

/* nvram test */
#if 0
test();
zb_write_security_key();
test();
zb_read_security_key();
#endif

  if (zdo_dev_start() != RET_OK)
  {
    TRACE_MSG(TRACE_ERROR, "zdo_dev_start failed", (FMT__0));
  }
  else
  {
    zdo_main_loop();
  }
}


void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
    zb_af_set_data_indication(data_indication);
    send_data((zb_buf_t *)ZB_BUF_FROM_REF(param));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}


static void send_data(zb_buf_t *buf)
{
  zb_apsde_data_req_t *req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
  zb_uint8_t *ptr = NULL;
  zb_short_t i;

  req->dst_addr.addr_short = 0; /* send to ZC */
  req->addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  req->tx_options = ZB_APSDE_TX_OPT_ACK_TX;
  req->radius = 1;
  req->profileid = 2;
  req->src_endpoint = 10;
  req->dst_endpoint = 10;

  buf->u.hdr.handle = 0x11;
  ZB_BUF_INITIAL_ALLOC(buf, 80, ptr);

  for (i = 0 ; i < 80 ; ++i)
  {
    ptr[i] = i % 32 + '0';
  }
  TRACE_MSG(TRACE_APS3, "Sending apsde_data.request", (FMT__0));

  ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
}


void data_indication(zb_uint8_t param)
{
  zb_ushort_t i;
  zb_uint8_t *ptr;
  zb_buf_t *asdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);

  /* Remove APS header from the packet */
  ZB_APS_HDR_CUT(asdu, ptr);

  TRACE_MSG(TRACE_APS3, "data_indication: packet %p len %d handle 0x%x", (FMT__P_D_D,
                         asdu, (int)ZB_BUF_LEN(asdu), asdu->u.hdr.status));

  for (i = 0 ; i < ZB_BUF_LEN(asdu) ; ++i)
  {
    TRACE_MSG(TRACE_APS3, "%x %c", (FMT__D_C, (int)ptr[i], ptr[i]));
  }

  send_data(asdu);
}


/*! @} */
