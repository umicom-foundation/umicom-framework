/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/auction_order.h
 *
 * PURPOSE:
 *   Represent eligible auction interest with side, price and timestamp priority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_AUCTION_ORDER_H
#define UMICOM_TRADING_CORE_AUCTION_ORDER_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the trading auction order data shared with callers of this public contract.
 */
typedef struct UmiTradingAuctionOrder { UmiFinancialId order_id; UmiSide side; UmiTradingQuantityLots lots; UmiTradingPriceTicks limit_ticks; int64_t priority_time_ms; } UmiTradingAuctionOrder;
/**
 * Initialise trading auction order from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_trading_auction_order_init(UmiTradingAuctionOrder *order,const char *order_id,UmiSide side,UmiTradingQuantityLots lots,UmiTradingPriceTicks limit_ticks,int64_t priority_time_ms);
/**
 * Check that trading auction order satisfies its contract before another service relies on
 * it.
 */
bool umi_trading_auction_order_valid(const UmiTradingAuctionOrder *order);
#ifdef __cplusplus
}
#endif
#endif
