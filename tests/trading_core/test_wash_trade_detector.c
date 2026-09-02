/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_wash_trade_detector.c
 *
 * PURPOSE:
 *   Exercise detect same-beneficial-owner self-cross trade evidence for surveillance review.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/wash_trade_detector.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiFinancialId a,b;
    umi_trading_core_id_assign(&a,"owner");
    umi_trading_core_id_assign(&b,"owner");
    return umi_trading_wash_trade_detector_detect(&a,&b,10,5)?0:1;
}
