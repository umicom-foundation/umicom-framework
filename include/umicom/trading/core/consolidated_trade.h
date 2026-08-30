/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/consolidated_trade.h
 *
 * PURPOSE:
 *   Represent a venue trade print with integer-normalised price and quantity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_CONSOLIDATED_TRADE_H
#define UMICOM_TRADING_CORE_CONSOLIDATED_TRADE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradingConsolidatedTrade { UmiFinancialId instrument_id; UmiFinancialId venue_id; UmiTradingPriceTicks price_ticks; UmiTradingQuantityLots quantity_lots; int64_t event_time_ms; } UmiTradingConsolidatedTrade;
/* Initialise and validate represent a venue trade print with integer-normalised price and quantity. */
UmiStatus umi_trading_consolidated_trade_init(UmiTradingConsolidatedTrade *value,const UmiFinancialId * instrument_id, const UmiFinancialId * venue_id, UmiTradingPriceTicks price_ticks, UmiTradingQuantityLots quantity_lots, int64_t event_time_ms);
/* Validate the invariant set for this trading record. */
bool umi_trading_consolidated_trade_valid(const UmiTradingConsolidatedTrade *value);
#ifdef __cplusplus
}
#endif
#endif
