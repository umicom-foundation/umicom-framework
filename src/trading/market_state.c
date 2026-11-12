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
/* Keep enum admission separate from transition policy. Equality cannot turn
 * an unknown value from a plug-in or restored record into a declared state. */
static int MarketStateKnown(UmiMarketState state)
{
    switch (state) {
    case UMI_MARKET_CLOSED:
    case UMI_MARKET_PREOPEN:
    case UMI_MARKET_OPEN:
    case UMI_MARKET_HALTED:
        return 1;
    default:
        return 0;
    }
}

/* The shared transition policy now validates both enum values before treating an observation as a repeat. All declared-state transitions retain their previous behavior. The previous implementation is retained for review. */
#if 0
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
#endif
int umi_market_state_transition_allowed(UmiMarketState from,
                                        UmiMarketState to)
{
    if (!MarketStateKnown(from) || !MarketStateKnown(to)) return 0;
    /* A repeated declared state remains an allowed observation. */
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
