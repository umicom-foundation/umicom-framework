/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_market_features.c
 *
 * PURPOSE:
 *   Validate market features behaviour in the trading foundation.
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
    UmiBar b={0};b.instrument=test_instrument();b.open=100;b.high=105;b.low=95;b.close=100;b.volume=1;b.start_time_ms=0;b.end_time_ms=1;
    assert(umi_bar_volatility_proxy(&b)==0.1);assert(umi_price_momentum(100,101)==0.01);assert(umi_mean_reversion_distance(101,100)==1);assert(umi_room_to_level(100,120,1)==20);return 0;
}
