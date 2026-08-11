/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/market_state.c
 *
 * PURPOSE:
 *   Validate market-state transitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of market state. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/market_state.h"

int umi_market_state_transition_allowed(UmiMarketState from,
                                        UmiMarketState to)
{
    if (from == to) {
        return 1;
    }

    switch (from) {
    case UMI_MARKET_CLOSED:
        return to == UMI_MARKET_PREOPEN;
    case UMI_MARKET_PREOPEN:
        return to == UMI_MARKET_OPEN || to == UMI_MARKET_CLOSED;
    case UMI_MARKET_OPEN:
        return to == UMI_MARKET_HALTED || to == UMI_MARKET_CLOSED;
    case UMI_MARKET_HALTED:
        return to == UMI_MARKET_OPEN || to == UMI_MARKET_CLOSED;
    default:
        return 0;
    }
}
