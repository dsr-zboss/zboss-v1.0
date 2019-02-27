/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac - RFD
Associate with FFD without discovery
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac.h"

#define USE_ZB_MLME_ASSOCIATE_CONFIRM
#include "zb_mac_only_stubs.h"

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

#define ASSOCIATE_TO 0x1122
#define ASSOCIATE_TO_PAN 0x1aaa
#define CHANNEL 0x14
#define CAP_INFO 0x80           /* 80 - "allocate address" */
static zb_ieee_addr_t g_mac_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};



MAIN()
{
  ARGV_UNUSED;

  ZB_INIT("association_01_rfd", argv[1], argv[2]);

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_mac_addr);
  MAC_PIB().mac_pan_id = 0x1aaa;

  {
    zb_buf_t *buf = zb_get_out_buf();
    zb_uint16_t addr = ASSOCIATE_TO;

    ZB_MLME_BUILD_ASSOCIATE_REQUEST(buf, CHANNEL,
                                    ASSOCIATE_TO_PAN,
                                    ZB_ADDR_16BIT_DEV_OR_BROADCAST, &addr,
                                    CAP_INFO);

    ZB_SCHEDULE_CALLBACK(zb_mlme_associate_request, ZB_REF_FROM_BUF(buf));
  }

  while(1)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


void zb_mlme_associate_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mlme_associate_confirm_t *request = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_associate_confirm_t);

  TRACE_MSG(TRACE_NWK2, "zb_mlme_associate_confirm param %hd status %hd short_address %hd",
            (FMT__H_H_H, param, request->status, request->assoc_short_address));

  zb_free_buf(ZB_BUF_FROM_REF(param));
}


/*! @} */
