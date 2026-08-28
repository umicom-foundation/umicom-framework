/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/order_book_level.h
 *
 * PURPOSE:
 *   Represent aggregated price-time liquidity at one book level.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_ORDER_BOOK_LEVEL_H
#define UMICOM_TRADING_CORE_ORDER_BOOK_LEVEL_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradingOrderBookLevel { UmiTradingPriceTicks price_ticks; UmiTradingQuantityLots quantity_lots; uint32_t order_count; } UmiTradingOrderBookLevel;
/* Initialise and validate represent aggregated price-time liquidity at one book level. */
UmiStatus umi_trading_order_book_level_init(UmiTradingOrderBookLevel *value,UmiTradingPriceTicks price_ticks, UmiTradingQuantityLots quantity_lots, uint32_t order_count);
/* Validate the invariant set for this trading record. */
bool umi_trading_order_book_level_valid(const UmiTradingOrderBookLevel *value);
#ifdef __cplusplus
}
#endif
#endif
