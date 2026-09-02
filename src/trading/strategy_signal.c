/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/strategy_signal.c
 *
 * PURPOSE:
 *   Validate research/strategy signals without granting execution authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of strategy signal. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/strategy_signal.h"
/* Check that strategy signal satisfies its contract before another service relies on it. */
int umi_strategy_signal_valid(const UmiStrategySignal *s,int64_t now){return s!=NULL&&s->probability>=0.0&&s->probability<=1.0&&s->direction>=-1&&s->direction<=1&&now<=s->valid_until_ms;}
