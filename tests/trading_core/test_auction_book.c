/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_auction_book.c
 *
 * PURPOSE:
 *   Exercise maintain bounded auction buy and sell interest before uncrossing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/auction_book.h"
int main(void) {

    UmiTradingAuctionBook b;
    UmiTradingAuctionOrder x,y;
    umi_trading_auction_book_init(&b);
    umi_trading_auction_order_init(&x,"b",UMI_SIDE_BUY,10,101,1);
    umi_trading_auction_order_init(&y,"s",UMI_SIDE_SELL,7,99,2);
    umi_trading_auction_book_add(&b,&x);
    umi_trading_auction_book_add(&b,&y);
    return umi_trading_auction_book_executable(&b,100)==7?0:1;
}
