/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_replay.c
 *
 * PURPOSE:
 *   Validate replay behaviour in the trading foundation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include <stdio.h>
#include "umicom/trading/trading.h"
int main(void){
    UmiReplayEvent e={1U,1000,{0},{0}};(void)snprintf(e.type,sizeof(e.type),"%s","tick");assert(umi_replay_event_valid(&e));
    UmiReplayCursor c;umi_replay_cursor_init(&c,1U);assert(umi_replay_cursor_accept(&c,&e));assert(c.next_sequence==2U);
    UmiReplayClock clock;umi_replay_clock_init(&clock,0,2.0);umi_replay_clock_advance(&clock,1000);assert(clock.now_ms==1000);
    assert(umi_market_replay_in_window(&e,0,2000));assert(umi_replay_event_digest(&e)!=0U);return 0;
}
