/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_volatility_interrupt.c
 *
 * PURPOSE:
 *   Exercise track short-horizon reference-price deviations and recommend auction interruption.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/volatility_interrupt.h"
int main(void) {

    UmiTradingVolatilityInterrupt v;
    if(umi_trading_volatility_interrupt_init(&v,10000,500U)!=UMI_STATUS_OK)return 1;
    return umi_trading_volatility_interrupt_evaluate(&v,10600)?0:2;
}
