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

#include "umicom/trading/tick.h"
/* Check that trade tick satisfies its contract before another service relies on it. */
int umi_trade_tick_valid(const UmiTradeTick *t){return t!=NULL&&t->price>0.0&&t->size>=0.0&&t->event_time_ms>=0;}
