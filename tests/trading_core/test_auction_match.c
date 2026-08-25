/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_auction_match.c
 *
 * PURPOSE:
 *   Exercise choose a deterministic auction clearing price by maximum executable quantity then minimum imbalance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/auction_match.h"
int main(void) {

    UmiTradingAuctionBook b;
    UmiTradingAuctionOrder x,y;
    UmiTradingAuctionMatch m;
    umi_trading_auction_book_init(&b);
    umi_trading_auction_order_init(&x,"b",UMI_SIDE_BUY,10,101,1);
    umi_trading_auction_order_init(&y,"s",UMI_SIDE_SELL,8,99,2);
    umi_trading_auction_book_add(&b,&x);
    umi_trading_auction_book_add(&b,&y);
    if(umi_trading_auction_match_compute(&b,&m)!=UMI_STATUS_OK)return 1;
    return m.executable_lots==8?0:2;
}
