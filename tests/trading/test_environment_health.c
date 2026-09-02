/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_environment_health.c
 *
 * PURPOSE:
 *   Validate environment health behaviour in the trading foundation.
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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){
    assert(umi_trading_health_ready(1,0,1,UMI_TRADING_SIMULATION));assert(!umi_trading_health_ready(1,0,1,UMI_TRADING_LIVE));
    assert(!umi_trading_environment_allows_live_execution(UMI_TRADING_LIVE,0));assert(umi_trading_environment_allows_live_execution(UMI_TRADING_PAPER,0));return 0;
}
