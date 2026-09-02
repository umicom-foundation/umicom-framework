/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/exchange_session.c
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
#include "umicom/trading/core/exchange_session.h"

#include <string.h>
/*
 * Initialise trading exchange session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_exchange_session_init(UmiTradingExchangeSession *session,const UmiFinancialId *venue_id,uint32_t breaker_bps,uint32_t halt_seconds){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(session==NULL||venue_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(session,0,sizeof *session);session->venue_id=*venue_id;session->phase=UMI_TRADING_CORE_PHASE_CLOSED;session->sequence=1U;return umi_trading_circuit_breaker_init(&session->breaker,breaker_bps,halt_seconds);}
/*
 * Provide the trading exchange session transition operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_exchange_session_transition(UmiTradingExchangeSession *session,UmiTradingCoreMarketPhase phase){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(session==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_trading_market_phase_transition_allowed(session->phase,phase))return UMI_STATUS_INVALID_STATE;session->phase=phase;session->sequence++;return UMI_STATUS_OK;}
/*
 * Provide the trading exchange session matching enabled operation used by this module and
 * its client applications.
 */
bool umi_trading_exchange_session_matching_enabled(UmiTradingExchangeSession *session,int64_t now_ms){return session!=NULL&&umi_trading_market_phase_matching_enabled(session->phase)&&!umi_trading_circuit_breaker_halted(&session->breaker,now_ms);}
