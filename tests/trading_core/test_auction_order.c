/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_auction_order.c
 *
 * PURPOSE:
 *   Exercise represent eligible auction interest with side, price and timestamp priority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/auction_order.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingAuctionOrder o;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_auction_order_init(&o,"o",UMI_SIDE_BUY,10,100,1)!=UMI_STATUS_OK)return 1;
    return umi_trading_auction_order_valid(&o)?0:2;
}
