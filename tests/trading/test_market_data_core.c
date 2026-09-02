/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_market_data_core.c
 *
 * PURPOSE:
 *   Validate market data core behaviour in the trading foundation.
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
#include "test_trading_common.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){
    UmiQuote q={0};q.instrument=test_instrument();q.bid=25000.0;q.ask=25002.0;q.bid_size=4.0;q.ask_size=2.0;q.event_time_ms=1000;
    assert(umi_quote_valid(&q));assert(umi_quote_mid(&q)==25001.0);assert(umi_quote_spread(&q)==2.0);
    UmiBar b={0};b.instrument=q.instrument;b.open=24990;b.high=25010;b.low=24980;b.close=25000;b.volume=100;b.start_time_ms=0;b.end_time_ms=1000;
    assert(umi_bar_valid(&b));assert(umi_bar_range(&b)==30.0);assert(umi_market_data_snapshot_aligned(&q,&b,0));
    assert(umi_market_data_quality_score(&q,1100,1000)>0.8);
    return 0;
}
