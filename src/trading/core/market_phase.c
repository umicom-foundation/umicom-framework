/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/market_phase.c
 *
 * PURPOSE:
 *   Validate market-phase transitions used by venue and exchange sessions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/market_phase.h"

/* Determine whether a venue phase transition is permitted by the common exchange lifecycle. */
bool umi_trading_market_phase_transition_allowed(UmiTradingCoreMarketPhase from,UmiTradingCoreMarketPhase to){/* Apply this operation only while the related capability or state is available. */ if(from==to)return true;/* Select the behaviour associated with the requested command or state value. */ switch(from){case UMI_TRADING_CORE_PHASE_CLOSED:return to==UMI_TRADING_CORE_PHASE_PREOPEN;case UMI_TRADING_CORE_PHASE_PREOPEN:return to==UMI_TRADING_CORE_PHASE_AUCTION||to==UMI_TRADING_CORE_PHASE_CONTINUOUS||to==UMI_TRADING_CORE_PHASE_CLOSED;case UMI_TRADING_CORE_PHASE_AUCTION:return to==UMI_TRADING_CORE_PHASE_CONTINUOUS||to==UMI_TRADING_CORE_PHASE_HALT;case UMI_TRADING_CORE_PHASE_CONTINUOUS:return to==UMI_TRADING_CORE_PHASE_HALT||to==UMI_TRADING_CORE_PHASE_POSTCLOSE;case UMI_TRADING_CORE_PHASE_HALT:return to==UMI_TRADING_CORE_PHASE_AUCTION||to==UMI_TRADING_CORE_PHASE_CONTINUOUS||to==UMI_TRADING_CORE_PHASE_CLOSED;case UMI_TRADING_CORE_PHASE_POSTCLOSE:return to==UMI_TRADING_CORE_PHASE_CLOSED;default:return false;}}
/* Return whether order matching may run during a phase. */
bool umi_trading_market_phase_matching_enabled(UmiTradingCoreMarketPhase phase){return phase==UMI_TRADING_CORE_PHASE_AUCTION||phase==UMI_TRADING_CORE_PHASE_CONTINUOUS;}
