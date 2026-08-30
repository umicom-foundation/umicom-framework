/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_market_phase.c
 *
 * PURPOSE:
 *   Exercise validate market-phase transitions used by venue and exchange sessions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/market_phase.h"
int main(void) {

    if(!umi_trading_market_phase_transition_allowed(UMI_TRADING_CORE_PHASE_PREOPEN,UMI_TRADING_CORE_PHASE_AUCTION))return 1;
    if(umi_trading_market_phase_transition_allowed(UMI_TRADING_CORE_PHASE_CLOSED,UMI_TRADING_CORE_PHASE_CONTINUOUS))return 2;
    return umi_trading_market_phase_matching_enabled(UMI_TRADING_CORE_PHASE_CONTINUOUS)?0:3;
}
