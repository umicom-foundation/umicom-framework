/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/market_phase.c
 *
 * PURPOSE:
 *   Validate market-phase transitions used by venue and exchange sessions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/market_phase.h"

/* Determine whether a venue phase transition is permitted by the common exchange lifecycle. */
bool umi_trading_market_phase_transition_allowed(UmiTradingCoreMarketPhase from,UmiTradingCoreMarketPhase to){if(from==to)return true;switch(from){case UMI_TRADING_CORE_PHASE_CLOSED:return to==UMI_TRADING_CORE_PHASE_PREOPEN;case UMI_TRADING_CORE_PHASE_PREOPEN:return to==UMI_TRADING_CORE_PHASE_AUCTION||to==UMI_TRADING_CORE_PHASE_CONTINUOUS||to==UMI_TRADING_CORE_PHASE_CLOSED;case UMI_TRADING_CORE_PHASE_AUCTION:return to==UMI_TRADING_CORE_PHASE_CONTINUOUS||to==UMI_TRADING_CORE_PHASE_HALT;case UMI_TRADING_CORE_PHASE_CONTINUOUS:return to==UMI_TRADING_CORE_PHASE_HALT||to==UMI_TRADING_CORE_PHASE_POSTCLOSE;case UMI_TRADING_CORE_PHASE_HALT:return to==UMI_TRADING_CORE_PHASE_AUCTION||to==UMI_TRADING_CORE_PHASE_CONTINUOUS||to==UMI_TRADING_CORE_PHASE_CLOSED;case UMI_TRADING_CORE_PHASE_POSTCLOSE:return to==UMI_TRADING_CORE_PHASE_CLOSED;default:return false;}}
/* Return whether order matching may run during a phase. */
bool umi_trading_market_phase_matching_enabled(UmiTradingCoreMarketPhase phase){return phase==UMI_TRADING_CORE_PHASE_AUCTION||phase==UMI_TRADING_CORE_PHASE_CONTINUOUS;}
