/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_market_session_gap.c
 *
 * PURPOSE:
 *   Validate market session gap behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include "umicom/trading/trading.h"
int main(void){assert(umi_market_session_contains(100,200,150));assert(!umi_market_session_contains(100,200,200));assert(umi_market_event_gap_ms(100,150)==50);assert(umi_research_window_contains(100,150,100));return 0;}
