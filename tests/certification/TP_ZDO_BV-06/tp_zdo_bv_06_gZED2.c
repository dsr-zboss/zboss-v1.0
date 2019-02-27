/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: TP/PRO/BV-27 End Device – Joins the network
Verify that the end device is capable to join a network and make a device
announcement
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

/* For NS build first ieee addr byte should be unique */
#ifdef ZB_NS_BUILD
zb_ieee_addr_t g_ieee_addr_ed1 = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_ieee_addr_ed2 = {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
#else
zb_ieee_addr_t g_ieee_addr_ed1 = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_ieee_addr_ed2 = {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
#endif

zb_af_simple_desc_1_1_t test_simple_desc;

MAIN()
{
  ARGV_UNUSED;

#ifndef KEIL
  if ( argc < 3 )
  {
    printf("%s <read pipe path> <write pipe path>\n", argv[0]);
    return 0;
  }
#endif

#ifdef APS_SECUR
  aps_secure = 1;
#endif

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("tp_zdo_bv_06_gZED2", argv[1], argv[2]);
#else
  ZB_INIT("tp_zdo_bv_06_gZED1", "2", "2");
#endif

  /* ZED2 ep 0xf0, dev id = 0xaaaa, output clust id 0x54, input clust id 0x1c */

  zb_set_simple_descriptor((zb_af_simple_desc_1_1_t*)&test_simple_desc,
                           0xF0 /* endpoint */,               0x7f01 /* app_profile_id */,
                           0xaaaa /* app_device_id */,        0x0   /* app_device_version*/,
                           0x1 /* app_input_cluster_count */, 0x1   /* app_output_cluster_count */);
  zb_set_input_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 0,  0x54);
  zb_set_output_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 0,  0x1c);
  zb_add_simple_descriptor((zb_af_simple_desc_1_1_t*)&test_simple_desc);

  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr_ed2);

#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
#endif

#ifdef ZB_SECURITY
  /* turn off security */
  ZB_NIB_SECURITY_LEVEL() = 0;
#endif

  /* become an ED */
  ZB_NIB_DEVICE_TYPE() = ZB_NWK_DEVICE_TYPE_ED;
  ZB_PIB_RX_ON_WHEN_IDLE() = ZB_FALSE;

#if 0
/* TODO: it is temporary, need remove after fix device annonce */
  {
    zb_address_ieee_ref_t ref;
    zb_address_update(g_ieee_addr, 646, ZB_FALSE, &ref);
  }
  {
    zb_address_ieee_ref_t ref;
    zb_address_update(g_ieee_addr_d, 645, ZB_FALSE, &ref);
  }
#endif

/* ----------------------------------------------------------- */

  if (zdo_dev_start() != RET_OK)
  {
    TRACE_MSG(TRACE_ERROR, "zdo_dev_start failed", (FMT__0));
  }
  else
  {
    zdo_main_loop();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device started FAILED status %d", (FMT__D, (int)buf->u.hdr.status));
    zb_free_buf(buf);
  }
}
