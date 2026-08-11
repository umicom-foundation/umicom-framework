/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/tick.c
 *
 * PURPOSE:
 *   Validate normalised market trade ticks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of tick. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/tick.h"
int umi_trade_tick_valid(const UmiTradeTick *t){return t!=NULL&&t->price>0.0&&t->size>=0.0&&t->event_time_ms>=0;}
