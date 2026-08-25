/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_spoofing_detector.c
 *
 * PURPOSE:
 *   Exercise detect high cancellation ratios near the touch as a conservative spoofing signal.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/spoofing_detector.h"
int main(void) {

    UmiTradingSpoofingEvidence e={100U,90U,2U,80U};
    return umi_trading_spoofing_detector_detect(&e,80U,70U)?0:1;
}
