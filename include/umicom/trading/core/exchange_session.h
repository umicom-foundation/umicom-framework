/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/exchange_session.h
 *
 * PURPOSE:
 *   Coordinate market phase, sequence and circuit-breaker state for a reusable exchange session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_EXCHANGE_SESSION_H
#define UMICOM_TRADING_CORE_EXCHANGE_SESSION_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/circuit_breaker.h"
#include "umicom/trading/core/market_phase.h"
/**
 * Represent the trading exchange session data shared with callers of this public contract.
 */
typedef struct UmiTradingExchangeSession { UmiFinancialId venue_id; UmiTradingCoreMarketPhase phase; uint64_t sequence; UmiTradingCircuitBreaker breaker; } UmiTradingExchangeSession;
/**
 * Initialise trading exchange session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_exchange_session_init(UmiTradingExchangeSession *session,const UmiFinancialId *venue_id,uint32_t breaker_bps,uint32_t halt_seconds);
/**
 * Provide the trading exchange session transition operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_exchange_session_transition(UmiTradingExchangeSession *session,UmiTradingCoreMarketPhase phase);
/**
 * Provide the trading exchange session matching enabled operation used by this module and
 * its client applications.
 */
bool umi_trading_exchange_session_matching_enabled(UmiTradingExchangeSession *session,int64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
