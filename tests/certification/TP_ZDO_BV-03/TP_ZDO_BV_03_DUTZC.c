/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: 14.9 TP/ZDO/BV-03: ZC-ZDO-Receive Service Discovery
The DUT as ZigBee coordinator shall respond to mandatory service discovery requests
from a remote node. Coordinator side
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"

zb_ieee_addr_t g_ieee_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};

ZB_DECLARE_SIMPLE_DESC(10, 10);

zb_af_simple_desc_10_10_t test_simple_desc;


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

  /* Init device, load IB values from nvram or set it to default */
#ifndef ZB8051
  ZB_INIT("zdo_zc", argv[1], argv[2]);
#else
  ZB_INIT("zdo_zc", "1", "1");
#endif

//  zb_set_default_ffd_descriptor_values(ZB_COORDINATOR);


/* set power descriptor to appropriate values (sadly, these values differs from
 * test to test */
zb_set_node_power_descriptor(ZB_POWER_MODE_SYNC_ON_WHEN_IDLE,
                             ZB_POWER_SRC_CONSTATNT | ZB_POWER_SRC_RECHARGEABLE_BATTERY | ZB_POWER_SRC_DISPOSABLE_BATTERY,
                               ZB_POWER_SRC_CONSTATNT, ZB_POWER_LEVEL_100);



/*
  simple descriptor for test
  SimpleDescriptor=
  Endpoint=0x01, Application profile identifier=0x0103, Application device
  identifier=0x0000, Application device version=0b0000, Application
  flags=0b0000, Application input cluster count=0x0A, Application input
  cluster list=0x00 0x03 0x04 0x38 0x54 0x70 0x8c 0xc4 0xe0 0xff,
  Application output cluster count=0x0A, Application output cluster
  list=0x00 0x01 0x02 0x1c 0x38 0x70 0x8c 0xa8 0xc4 0xff
*/


  zb_set_simple_descriptor((zb_af_simple_desc_1_1_t*)&test_simple_desc,
                           1 /* endpoint */,                0x0103 /* app_profile_id */,
                           0x0 /* app_device_id */,         0x0   /* app_device_version*/,
                           0xA /* app_input_cluster_count */, 0xA /* app_output_cluster_count */);


  zb_set_input_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 0,  0x0);
  zb_set_input_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 1,  0x3);
  zb_set_input_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 2,  0x4);
  zb_set_input_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 3,  0x38);
  zb_set_input_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 4,  0x54);
  zb_set_input_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 5,  0x70);
  zb_set_input_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 6,  0x8c);
  zb_set_input_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 7,  0xc4);
  zb_set_input_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 8,  0xe0);
  zb_set_input_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 9,  0xff);


  zb_set_output_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 0,  0x0);
  zb_set_output_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 1,  0x1);
  zb_set_output_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 2,  0x2);
  zb_set_output_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 3,  0x1c);
  zb_set_output_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 4,  0x38);
  zb_set_output_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 5,  0x70);
  zb_set_output_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 6,  0x8c);
  zb_set_output_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 7,  0xa8);
  zb_set_output_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 8,  0xc4);
  zb_set_output_cluster_id((zb_af_simple_desc_1_1_t*)&test_simple_desc, 9,  0xff);

  zb_add_simple_descriptor((zb_af_simple_desc_1_1_t*)&test_simple_desc);



  /* let's always be coordinator */
  ZB_AIB().aps_designated_coordinator = 1;

  /* set ieee addr */
  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ieee_addr);

  MAC_PIB().mac_pan_id = 0x1aaa;

#ifndef ZB_NS_BUILD
  ZB_UPDATE_LONGMAC();
  ZB_UPDATE_PAN_ID();
#endif

#ifdef ZB_SECURITY
  /* turn off security */
  ZB_NIB_SECURITY_LEVEL() = 0;
#endif

  /* accept only one child */
  ZB_NWK().max_children = 2;


  if ( zdo_dev_start() != RET_OK )
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
  TRACE_MSG(TRACE_ERROR, ">>zb_zdo_startup_complete status %d", (FMT__D, (int)buf->u.hdr.status));
  if (buf->u.hdr.status == 0)
  {
    TRACE_MSG(TRACE_ERROR, "Device STARTED OK", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "Device START FAILED", (FMT__0));
  }

  zb_free_buf(buf);
}
