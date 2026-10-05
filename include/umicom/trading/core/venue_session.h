/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/venue_session.h
 *
 * PURPOSE:
 *   Model a bounded venue trading session and its current phase.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_VENUE_SESSION_H
#define UMICOM_TRADING_CORE_VENUE_SESSION_H
#include "umicom/trading/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading venue session data shared with callers of this public contract.
 */
typedef struct UmiTradingVenueSession { UmiFinancialId venue_id; int64_t open_time_ms; int64_t close_time_ms; UmiTradingCoreMarketPhase phase; } UmiTradingVenueSession;
/* Initialise and validate model a bounded venue trading session and its current phase. */
UmiStatus umi_trading_venue_session_init(UmiTradingVenueSession *value,const UmiFinancialId * venue_id, int64_t open_time_ms, int64_t close_time_ms, UmiTradingCoreMarketPhase phase);
/* Validate the invariant set for this trading record. */
bool umi_trading_venue_session_valid(const UmiTradingVenueSession *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_venue_session_archive_encode(const UmiTradingVenueSession *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_venue_session_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingVenueSession *value);

#ifdef __cplusplus
}
#endif
#endif
