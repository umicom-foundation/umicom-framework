/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/auction_order.h
 *
 * PURPOSE:
 *   Represent eligible auction interest with side, price and timestamp priority.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_AUCTION_ORDER_H
#define UMICOM_TRADING_CORE_AUCTION_ORDER_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingAuctionOrder { UmiFinancialId order_id; UmiSide side; UmiTradingQuantityLots lots; UmiTradingPriceTicks limit_ticks; int64_t priority_time_ms; } UmiTradingAuctionOrder;
UmiStatus umi_trading_auction_order_init(UmiTradingAuctionOrder *order,const char *order_id,UmiSide side,UmiTradingQuantityLots lots,UmiTradingPriceTicks limit_ticks,int64_t priority_time_ms);
bool umi_trading_auction_order_valid(const UmiTradingAuctionOrder *order);
#ifdef __cplusplus
}
#endif
#endif
