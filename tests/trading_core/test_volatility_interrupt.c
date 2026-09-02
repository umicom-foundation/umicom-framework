/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_volatility_interrupt.c
 *
 * PURPOSE:
 *   Exercise track short-horizon reference-price deviations and recommend auction interruption.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/volatility_interrupt.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingVolatilityInterrupt v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_volatility_interrupt_init(&v,10000,500U)!=UMI_STATUS_OK)return 1;
    return umi_trading_volatility_interrupt_evaluate(&v,10600)?0:2;
}
