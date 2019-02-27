/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: DUT for scanning-02 test.
Became ZC, disallow joins.
*/


/**********************************************************************************************************************
*Initial Conditions:                                                                                                  *
*Device name  Description                                                                                             *
*FFD          MAC: 0xACDE480000000001                                                                                 *
***********************************************************************************************************************                                                                                                       *
*Test Procedure                                                                                                       *
*                                                                                                                     *
*Step No. Step description                        Expected outcome                                                    *
***********************************************************************************************************************                                                                                                       *
*         Set MLME-RESET.request(TRUE) to DUT.    Pass: FFD returns SUCCESS.                                          *
*                                                 Fail: FFD does not respond.                                         *
***********************************************************************************************************************
*                                                                                                                     *
*Set to DUT:                                      Pass:                                                               *
***********************************************************************************************************************
*                                                                                                                     *
* MLME-SCAN.request (                             It is possible to verify by a PAN analyzer that a beacon request    *
*   ScanType      = 0x01,                         command frame was issued from the DUT.                              *
*   ScanChannels  ==0x07fff800                                                                                        *
*   ScanDuration  = 0x05,                         Frame Length: 10 bytes                                              *
*   ChannelPage   = 0x00,                         IEEE 802.15.4                                                       *
*   SecurityLevel = 0x00,                           Frame Control: 0x0803                                             *
*   KeyIDMode     = xx,                               .... .... .... .011  = Frame Type: Command (0x0003)             *
*   KeySource     = NULL,                             .... .... .... 0...  = Security Enabled: Disabled               *
*   KeyIndex      = xx                                .... .... ...0 ....  = Frame Pending: No more data              *
*)                                                    .... .... ..0. ....  = Acknowledgment Request: Ack not required *
*                                                     .... .... .0.. ....  = Intra PAN: Not within the PAN            *
*                                                     .... ..00 0... ....  = Reserved                                 *
*                                                     .... 10.. .... ....  = Destination Addressing Mode: Address     *
*                                                                            field contains a 16-bit short address    *
*                                                     ..00 .... .... ....  = Reserved                                 *
*                                                     00.. .... .... ....  = Source Addressing Mode: PAN identifier   *
*                                                                            and address field are not present        *
*                                                   Sequence Number:            xx                                    *
*                                                   Destination PAN Identifier: 0xffff                                *
*                                                   Destination Address:        0xffff                                *
*                                                                                                                     *
*                                                   MAC Payload                                                       *
*                                                     Command Frame Identifier =  Beacon Request: (0x07)              *
*                                                   Frame Check Sequence:         Correct                             *
*                                                                                                                     *
*                                                   The DUT shall return a Success result, such that:                 *
*                                                   MLME-SCAN.confirm (                                               *
*                                                      Status            = 0xea = "NO_BEACON",                        *
*                                                      ScanType          = 0x01 = Active Scan,                        *
*                                                      ChannelPage       = 0,                                         *
*                                                      UnscannedChannels = 0x00 00 00 00                              *
*                                                      ResultListSize    = 0,                                         *
*                                                      EnergyDetectList  = NULL,                                      *
*                                                      PANDescriptorList = NULL                                       *
*                                                  )                                                                  *
*                                                                                                                     *
*                                                  Fail:                                                              *
*                                                  DUT does not respond, or                                           *
*                                                  DUT responds with SUCCESS*                                         *                                                                                                                                                                  *
*                                                                                                                     *                                                                                      *
*                                                                                                                     *
* Active Scan, all channels,                      Fail:                                                               *
* duration=[aBaseSuperframeDuration * (2n + 1)]   DUT does not respond, or EnergyDetectList                           *
* symbols per channel                             contains unexpected values                                          *
*                                                                                                                     *
***********************************************************************************************************************/

/********************************************************************************************
*TP/154/MAC/SCANNING-02  - can be testes using full stack; problem of ED scan existence.    *
*                                                                                           *
*MLME-RESET, then Active scan when no any devices around,                                   *
*check zb_mlme_scan_confirm result (must be NO_BEACON).                                     *
*Can use any ZC test which makes formation - say, zdo_startup_zc.                           *
*                                                                                           *
*Note that Formation first does ED scan, then Active scan, except case when channels mask   *
*contains only one channel.                                                                 *
*                                                                                           *
*To exclude ED scan, can fill ZB_DEFAULT_APS_CHANNEL_MASK in zb_config.h by only one channel*
*(only one bit set).                                                                        *
*But, test requests to do Active Scan at all channels.                                      *
*Not sure: can presence of ED scan be a problem in this test passing?                       *
*ED scan is not externally visible.                                                         *
*                                                                                           *
*Verify Active Scan result using trace in zb_mlme_scan_confirm() at nwk_discovery.c.        *
*Verify that scan_confirm -> status = MAC_NO_BEACON.                                        *
*Note: currently no such trace in zb_mlme_scan_confirm() - it should be added.              *
********************************************************************************************/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"

//#define USE_ZB_MLME_ASSOCIATE_CONFIRM
#define USE_ZB_MLME_RESET_CONFIRM
#define USE_ZB_MLME_SCAN_CONFIRM

#include "zb_mac_only_stubs.h"

/*! \addtogroup ZB_TESTS */
/*! @{ */

#ifndef ZB_COORDINATOR_ROLE
#error Coordinator role is not compiled!
#endif

#define MY_ADDR   0x0001
#define LOG_FILE  "scan_02"

/* MAC: 0xACDE480000000001 */
static zb_ieee_addr_t g_zc_addr = {0xac, 0xde, 0x48, 0x00, 0x00, 0x00, 0x00, 0x01};

#define TEST_CHANEL_MASK  0x00007FFF800
#define TEST_SCAN_TYPE    ACTIVE_SCAN
#define TEST_DURATION     0x05

MAIN()
{
    ARGV_UNUSED;

    /* Init device, load IB values from nvram or set it to default */
    ZB_INIT(LOG_FILE, argv[1], argv[2]);

    ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_zc_addr);

    {
        zb_buf_t *buf = zb_get_out_buf();
        zb_mlme_reset_request_t *reset_req = ZB_GET_BUF_PARAM(buf, zb_mlme_reset_request_t);
        reset_req -> set_default_pib = 1;

        ZB_SCHEDULE_CALLBACK(zb_mlme_reset_request, ZB_REF_FROM_BUF(buf));
    }

    for (;;)
    {
        zb_sched_loop_iteration();
    }

    TRACE_DEINIT();
    MAIN_RETURN(0);
}


void zb_mlme_reset_confirm(zb_uint8_t param) ZB_CALLBACK
{
    zb_mlme_scan_params_t* pbuf = ZB_GET_BUF_PARAM((zb_buf_t *)ZB_BUF_FROM_REF(param), zb_mlme_scan_params_t);

    TRACE_MSG(TRACE_NWK2, "zb_mlme_reset_confirm", (FMT__0));
    ZB_MLME_BUILD_SCAN_REQUEST((zb_buf_t *)ZB_BUF_FROM_REF(param), TEST_CHANEL_MASK, TEST_SCAN_TYPE, TEST_DURATION);
    ZB_SCHEDULE_CALLBACK(zb_mlme_scan_request, param);      // -> zb_mlme_scan_request
}

void zb_mlme_scan_confirm(zb_uint8_t param) ZB_CALLBACK
{

    zb_mac_scan_confirm_t *scan_confirm;

    TRACE_MSG(TRACE_NWK1, ">>zb_mlme_scan_confirm %hd", (FMT__H, param));

    scan_confirm = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mac_scan_confirm_t);
    TRACE_MSG(TRACE_NWK3, "scan type %hd, status %hd, auto_req %hd",
              (FMT__H_H_H, scan_confirm -> scan_type, scan_confirm -> status, MAC_PIB().mac_auto_request));



    if ( scan_confirm -> scan_type == ACTIVE_SCAN   &&
            scan_confirm -> status    == MAC_NO_BEACON &&
            scan_confirm -> unscanned_channels == 0    &&
            scan_confirm -> result_list_size   == 0 )
    {
        TRACE_MSG(TRACE_NWK1, "zb scan-02 test OK", (FMT__0));
    }
    else
    {
        TRACE_MSG(TRACE_NWK1, "zb scan-02 test FAIL", (FMT__0));
    }

    TRACE_MSG(TRACE_NWK1, "<<zb_mlme_scan_confirm", (FMT__0));
}

/*! @} */
