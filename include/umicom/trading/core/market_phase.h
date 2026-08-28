/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/market_phase.h
 *
 * PURPOSE:
 *   Validate market-phase transitions used by venue and exchange sessions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_MARKET_PHASE_H
#define UMICOM_TRADING_CORE_MARKET_PHASE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Determine whether a venue phase transition is permitted by the common exchange lifecycle. */
bool umi_trading_market_phase_transition_allowed(UmiTradingCoreMarketPhase from,UmiTradingCoreMarketPhase to);
/* Return whether order matching may run during a phase. */
bool umi_trading_market_phase_matching_enabled(UmiTradingCoreMarketPhase phase);
#ifdef __cplusplus
}
#endif
#endif
