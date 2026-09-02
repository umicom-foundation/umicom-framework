/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_posttrade_risk.c
 *
 * PURPOSE:
 *   Exercise evaluate post-fill gross position and daily loss limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/posttrade_risk.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingPosttradeRiskPolicy p={100,10000};
    return umi_trading_posttrade_risk_check(&p,-50,-500).allowed?0:1;
}
