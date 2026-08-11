/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/strategy_signal.c
 *
 * PURPOSE:
 *   Validate research/strategy signals without granting execution authority.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of strategy signal. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/strategy_signal.h"
int umi_strategy_signal_valid(const UmiStrategySignal *s,int64_t now){return s!=NULL&&s->probability>=0.0&&s->probability<=1.0&&s->direction>=-1&&s->direction<=1&&now<=s->valid_until_ms;}
