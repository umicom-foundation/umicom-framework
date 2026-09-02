/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_trading_platform_snapshot.c
 *
 * PURPOSE:
 *   Exercise publish a compact deterministic snapshot of brokerage and exchange platform health.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/trading_platform_snapshot.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingPlatformSnapshot s;
    umi_trading_platform_snapshot_build(&s,10U,20U,2U,0U,true,true);
    return umi_trading_platform_snapshot_ready(&s)?0:1;
}
