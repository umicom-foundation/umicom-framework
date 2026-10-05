/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/market_status.h
 *
 * PURPOSE:
 *   Capture exchange phase, sequence and operational availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_MARKET_STATUS_H
#define UMICOM_TRADING_CORE_MARKET_STATUS_H
#include "umicom/trading/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading market status data shared with callers of this public contract.
 */
typedef struct UmiTradingMarketStatus { UmiTradingCoreMarketPhase phase; uint64_t sequence; bool operational; } UmiTradingMarketStatus;
/* Initialise and validate capture exchange phase, sequence and operational availability. */
UmiStatus umi_trading_market_status_init(UmiTradingMarketStatus *value,UmiTradingCoreMarketPhase phase, uint64_t sequence, bool operational);
/* Validate the invariant set for this trading record. */
bool umi_trading_market_status_valid(const UmiTradingMarketStatus *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_market_status_archive_encode(const UmiTradingMarketStatus *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_market_status_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingMarketStatus *value);

#ifdef __cplusplus
}
#endif
#endif
