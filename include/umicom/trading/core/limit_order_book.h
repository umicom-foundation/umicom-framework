/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/limit_order_book.h
 *
 * PURPOSE:
 *   Maintain bounded aggregated bid and ask levels with deterministic price ordering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_LIMIT_ORDER_BOOK_H
#define UMICOM_TRADING_CORE_LIMIT_ORDER_BOOK_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/order_book_level.h"
typedef struct UmiTradingLimitOrderBook { UmiTradingOrderBookLevel bids[UMI_TRADING_CORE_MAX_BOOK_LEVELS]; UmiTradingOrderBookLevel asks[UMI_TRADING_CORE_MAX_BOOK_LEVELS]; size_t bid_count; size_t ask_count; uint64_t revision; } UmiTradingLimitOrderBook;
void umi_trading_limit_order_book_init(UmiTradingLimitOrderBook *book);
UmiStatus umi_trading_limit_order_book_update(UmiTradingLimitOrderBook *book,UmiSide side,UmiTradingPriceTicks price,UmiTradingQuantityLots quantity,uint32_t order_count);
const UmiTradingOrderBookLevel *umi_trading_limit_order_book_best(const UmiTradingLimitOrderBook *book,UmiSide side);
#ifdef __cplusplus
}
#endif
#endif
