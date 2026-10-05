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
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading market data event data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingMarketDataEvent { UmiFinancialId instrument_id; UmiFinancialId venue_id; uint64_t sequence; int64_t event_time_ms; } UmiTradingMarketDataEvent;
/* Initialise and validate normalise venue market-data sequence, instrument identity and event time. */
UmiStatus umi_trading_market_data_event_init(UmiTradingMarketDataEvent *value,const UmiFinancialId * instrument_id, const UmiFinancialId * venue_id, uint64_t sequence, int64_t event_time_ms);
/* Validate the invariant set for this trading record. */
bool umi_trading_market_data_event_valid(const UmiTradingMarketDataEvent *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_market_data_event_archive_encode(const UmiTradingMarketDataEvent *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_market_data_event_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingMarketDataEvent *value);

#ifdef __cplusplus
}
#endif
#endif
