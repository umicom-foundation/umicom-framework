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
#ifdef __cplusplus
}
#endif
#endif
