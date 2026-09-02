/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_price_band.c
 *
 * PURPOSE:
 *   Exercise define price protection bands around a reference price without overflow-prone multiplication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/price_band.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingPriceBand b;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_price_band_init(&b,100000,100U,200U)!=UMI_STATUS_OK)return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_trading_price_band_contains(&b,101000)||!umi_trading_price_band_contains(&b,99000))return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_trading_price_band_contains(&b,98000))return 3;
     return 0;
}
