/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_pretrade_risk.c
 *
 * PURPOSE:
 *   Exercise apply reusable pre-trade quantity, notional and buying-power gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/pretrade_risk.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingPretradeRiskPolicy p={100,100000};
    return umi_trading_pretrade_risk_check(&p,10,5000,10000).allowed?0:1;
}
