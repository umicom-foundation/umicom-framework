/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/market_data_event.h
 *
 * PURPOSE:
 *   Normalise venue market-data sequence, instrument identity and event time.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_MARKET_DATA_EVENT_H
#define UMICOM_TRADING_CORE_MARKET_DATA_EVENT_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradingMarketDataEvent { UmiFinancialId instrument_id; UmiFinancialId venue_id; uint64_t sequence; int64_t event_time_ms; } UmiTradingMarketDataEvent;
/* Initialise and validate normalise venue market-data sequence, instrument identity and event time. */
UmiStatus umi_trading_market_data_event_init(UmiTradingMarketDataEvent *value,const UmiFinancialId * instrument_id, const UmiFinancialId * venue_id, uint64_t sequence, int64_t event_time_ms);
/* Validate the invariant set for this trading record. */
bool umi_trading_market_data_event_valid(const UmiTradingMarketDataEvent *value);
#ifdef __cplusplus
}
#endif
#endif
