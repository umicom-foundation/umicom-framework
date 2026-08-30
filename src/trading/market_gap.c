/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/market_gap.c
 *
 * PURPOSE:
 *   Measure time gaps between market events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of market gap. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/market_gap.h"
int64_t umi_market_event_gap_ms(int64_t previous,int64_t current){return current>=previous?current-previous:-1;}
