/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/auction_match.h
 *
 * PURPOSE:
 *   Choose a deterministic auction clearing price by maximum executable quantity then minimum imbalance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_AUCTION_MATCH_H
#define UMICOM_TRADING_CORE_AUCTION_MATCH_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/auction_book.h"
typedef struct UmiTradingAuctionMatch { UmiTradingPriceTicks clearing_ticks; UmiTradingQuantityLots executable_lots; UmiTradingQuantityLots imbalance_lots; bool matched; } UmiTradingAuctionMatch;
/* Evaluate candidate prices drawn from submitted limit prices and select a clearing result. */
UmiStatus umi_trading_auction_match_compute(const UmiTradingAuctionBook *book,UmiTradingAuctionMatch *out_match);
#ifdef __cplusplus
}
#endif
#endif
