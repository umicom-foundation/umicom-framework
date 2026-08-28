/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/exchange_session.h
 *
 * PURPOSE:
 *   Coordinate market phase, sequence and circuit-breaker state for a reusable exchange session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_EXCHANGE_SESSION_H
#define UMICOM_TRADING_CORE_EXCHANGE_SESSION_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/circuit_breaker.h"
#include "umicom/trading/core/market_phase.h"
typedef struct UmiTradingExchangeSession { UmiFinancialId venue_id; UmiTradingCoreMarketPhase phase; uint64_t sequence; UmiTradingCircuitBreaker breaker; } UmiTradingExchangeSession;
UmiStatus umi_trading_exchange_session_init(UmiTradingExchangeSession *session,const UmiFinancialId *venue_id,uint32_t breaker_bps,uint32_t halt_seconds);
UmiStatus umi_trading_exchange_session_transition(UmiTradingExchangeSession *session,UmiTradingCoreMarketPhase phase);
bool umi_trading_exchange_session_matching_enabled(UmiTradingExchangeSession *session,int64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
