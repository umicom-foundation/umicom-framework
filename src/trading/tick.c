/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/tick.c
 *
 * PURPOSE:
 *   Validate normalised market trade ticks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of tick. Product-specific UI and vendor details stay outside this file.
 */

#include <math.h>
#include "umicom/trading/tick.h"
/* Check that trade tick satisfies its contract before another service relies on it. */
/* Finite validation is owned by the shared tick contract so direct callers and tape clients reject the same malformed numbers. The previous implementation is retained for review. */
#if 0
int umi_trade_tick_valid(const UmiTradeTick *t){return t!=NULL&&t->price>0.0&&t->size>=0.0&&t->event_time_ms>=0;}
#endif
int umi_trade_tick_valid(const UmiTradeTick *tick)
{
    /* Comparisons alone admit positive infinity. Every retained public print
     * must carry finite numeric evidence before it can reach tape or charts.
     * Instrument metadata is validated separately by the retaining owner. */
    return tick != NULL && isfinite(tick->price) && tick->price > 0.0 &&
           isfinite(tick->size) && tick->size >= 0.0 &&
           tick->event_time_ms >= 0;
}
