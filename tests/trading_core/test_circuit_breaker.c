/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_circuit_breaker.c
 *
 * PURPOSE:
 *   Exercise apply venue-level price-move thresholds and timed halt state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/circuit_breaker.h"
int main(void) {

    UmiTradingCircuitBreaker b;
    if(umi_trading_circuit_breaker_init(&b,500U,60U)!=UMI_STATUS_OK)return 1;
    if(!umi_trading_circuit_breaker_evaluate(&b,600U,1000))return 2;
    return !umi_trading_circuit_breaker_halted(&b,62000)?0:3;
}
