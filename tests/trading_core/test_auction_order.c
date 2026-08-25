/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_auction_order.c
 *
 * PURPOSE:
 *   Exercise represent eligible auction interest with side, price and timestamp priority.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/auction_order.h"
int main(void) {

    UmiTradingAuctionOrder o;
    if(umi_trading_auction_order_init(&o,"o",UMI_SIDE_BUY,10,100,1)!=UMI_STATUS_OK)return 1;
    return umi_trading_auction_order_valid(&o)?0:2;
}
