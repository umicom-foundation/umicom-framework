/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/market_state.c
 *
 * PURPOSE:
 *   Validate market-state transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of market state. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/market_state.h"

/*
 * Provide the market state transition allowed operation used by this module and its client
 * applications.
 */
int umi_market_state_transition_allowed(UmiMarketState from,
                                        UmiMarketState to)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (from == to) {
        return 1;
    }

    /* Select the behaviour associated with the requested command or state value. */
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
