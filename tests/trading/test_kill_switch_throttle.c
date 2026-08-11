/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_kill_switch_throttle.c
 *
 * PURPOSE:
 *   Validate kill switch throttle behaviour in the trading foundation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include "umicom/trading/trading.h"
int main(void){
    UmiTradingKillSwitch k={0};umi_kill_switch_engage(&k,"test");assert(k.engaged);umi_kill_switch_reset(&k);assert(!k.engaged);
    UmiOrderThrottle t;umi_order_throttle_init(&t,2U,1000);assert(umi_order_throttle_accept(&t,100));assert(umi_order_throttle_accept(&t,200));assert(!umi_order_throttle_accept(&t,300));assert(umi_order_throttle_accept(&t,1200));return 0;
}
