/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_watchlist_alerts.c
 *
 * PURPOSE:
 *   Validate watchlist alerts behaviour in the trading foundation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include "test_trading_common.h"
int main(void){
    UmiWatchlist w;umi_watchlist_init(&w);UmiInstrument i=test_instrument();assert(umi_watchlist_add(&w,&i)==UMI_STATUS_OK);assert(umi_watchlist_add(&w,&i)==UMI_STATUS_ALREADY_EXISTS);
    assert(umi_trading_alert_crossed(99,101,100));return 0;
}
