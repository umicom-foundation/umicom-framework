/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_trading_platform_snapshot.c
 *
 * PURPOSE:
 *   Exercise publish a compact deterministic snapshot of brokerage and exchange platform health.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/trading_platform_snapshot.h"
int main(void) {

    UmiTradingPlatformSnapshot s;
    umi_trading_platform_snapshot_build(&s,10U,20U,2U,0U,true,true);
    return umi_trading_platform_snapshot_ready(&s)?0:1;
}
