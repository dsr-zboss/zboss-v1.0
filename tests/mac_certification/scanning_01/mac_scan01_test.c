/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: DUT for scanning_01 test.
Became ZC, disallow joins.
*/

/********************************************************************************************************
*Initial Conditions:                                                                                    *
*Device name  Description                                                                               *
*FFD          MAC: 0xACDE480000000001                                                                   *
*********************************************************************************************************                                                                                                       *
*Test Procedure                                                                                         *
*                                                                                                       *
*Step No. Step description                        Expected outcome                                      *
*********************************************************************************************************                                                                                                       *
*         Set MLME-RESET.request(TRUE) to DUT.    Pass: FFD returns SUCCESS.                            *
*                                                 Fail: FFD does not respond.                           *
*********************************************************************************************************
*                                                                                                       *
*Set to DUT:                                      Pass:                                                 *
*MLME-SCAN.request (                              The DUT shall return a Success result, such that:     *
*   ScanType      = 0x00,                                                                               *
*   ScanChannels  == 0x00007FFF800                MLME-SCAN.confirm (                                   *
*   ScanDuration  = 0x05,                            Status            = 0x00="SUCCESS",                *
*   ChannelPage   = 0x00,                            ScanType          = 0x00=ED Scan,                  *
*   SecurityLevel = 0x00,                            ChannelPage       = 0,                             *
*   KeyIDMode     = xx,                              UnscannedChannels = 0x00 00 00 00                  *
*   KeySource     = NULL,                            ResultListSize    = 16,                            *
*   KeyIndex      = xx                               EnergyDetectList  = values representing the energy *
*)                                                                       on the different channel,      *
*                                                    PANDescriptorList = NULL                           *
*                                                 )                                                     *
*                                                                                                       *
* Energy Scan, all channels,                      Fail:                                                 *
* duration=[aBaseSuperframeDuration * (2n + 1)]   DUT does not respond, or EnergyDetectList             *
* symbols per channel                             contains unexpected values                            *
*                                                                                                       *
*                                                                                                       *
*                                                                                                       *
* Notes:                                                                                                *
*   Test is done twice: In a shielded environment where the energy value is expected to be zero         *
*   on all channels and during the second test a designated signal is used on one channel.              *
********************************************************************************************************/



/*****************************************************************************************
*TP/154/MAC/SCANNING-01 - can be testes using full stack, problem with MLME-RESET.confirm*
*                                                                                        *
*DUT - FFD.                                                                              *
*MLME-RESET, then ED scan, check zb_mlme_scan_confirm result.                            *
*Can use any ZC test which makes formation - say, zdo_startup_zc.                        *
*                                                                                        *
*Normally, zb_mlme_reset_request is not used at the stack init time,                     *
*so MLME-RESET usage is a problem.                                                       *
*If it is strictly necessary for certification, stack must be modified.                  *
*zb_mlme_reset_request called only for zb_nlme_reset_request().                          *
*                                                                                        *
*Need to build with right channels mask - ZB_DEFAULT_APS_CHANNEL_MASK in zb_config.h.    *
*MLME-SCAN.confirm  result can be verified using trace at zb_mlme_scan_confirm().        *
*Add / change trace, if necessary, is trivial.                                           *
*Maybe, it will be easier to check trace of nwk_formation_ed_scan_confirm().             *
*****************************************************************************************/

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
#define LOG_FILE "scan_01"

/* MAC: 0xACDE480000000001 */
static zb_ieee_addr_t g_zc_addr = {0xac, 0xde, 0x48, 0x00, 0x00, 0x00, 0x00, 0x01};

#define TEST_CHANEL_MASK ZB_TRANSCEIVER_ALL_CHANNELS_MASK /* (0xffff << 11) */


#define TEST_SCAN_TYPE    ED_SCAN
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
        MAC_CTX().current_channel    = 14;

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
    zb_uint8_t i, ed;

    TRACE_MSG(TRACE_NWK1, ">>zb_mlme_scan_confirm %hd", (FMT__H, param));

    scan_confirm = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mac_scan_confirm_t);
    TRACE_MSG(TRACE_NWK3,
              "status %hd (m.b. 0x00 = SUCCESS), scan type: %hd (m.b. 0x00 = ED Scan), unscanned_channels: %hd",
              (__FILE__,__LINE__, 6, scan_confirm -> status, scan_confirm -> scan_type,  scan_confirm -> unscanned_channels));

    ed = 0;
    for ( i = 0; i < ZB_MAC_SUPPORTED_CHANNELS; i++ )
    {
        if ( scan_confirm -> list.energy_detect[i])
        {
            TRACE_MSG(TRACE_NWK3, "Energy detect on channel: %hd val = %hu", (FMT__H_H, (i + ZB_MAC_START_CHANNEL_NUMBER), scan_confirm -> list.energy_detect[i]));
            ed++;
        }
    }
    if (!ed)
    {
        TRACE_MSG(TRACE_NWK3, "No detect energy on any channel", (FMT__0));
    }
    TRACE_MSG(TRACE_NWK1, "<<zb_mlme_scan_confirm", (FMT__0));
}

/*! @} */
