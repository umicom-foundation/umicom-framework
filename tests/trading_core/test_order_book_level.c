/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_order_book_level.c
 *
 * PURPOSE:
 *   Exercise represent aggregated price-time liquidity at one book level.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_book_level.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTradingOrderBookLevel v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_order_book_level_init(&v,100,20,2U)!=UMI_STATUS_OK) return 1;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(!umi_trading_order_book_level_valid(&v)) return 2;
     return 0;
}
