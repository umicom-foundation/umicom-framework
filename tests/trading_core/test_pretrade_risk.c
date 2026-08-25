/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_pretrade_risk.c
 *
 * PURPOSE:
 *   Exercise apply reusable pre-trade quantity, notional and buying-power gates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/pretrade_risk.h"
int main(void) {

    UmiTradingPretradeRiskPolicy p={100,100000};
    return umi_trading_pretrade_risk_check(&p,10,5000,10000).allowed?0:1;
}
