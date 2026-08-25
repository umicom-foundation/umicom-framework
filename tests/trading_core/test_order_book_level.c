/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_order_book_level.c
 *
 * PURPOSE:
 *   Exercise represent aggregated price-time liquidity at one book level.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_book_level.h"
int main(void) {
    UmiTradingOrderBookLevel v;
     if(umi_trading_order_book_level_init(&v,100,20,2U)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_order_book_level_valid(&v)) return 2;
     return 0;
}
