/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_concentration_monitor.c
 *
 * PURPOSE:
 *   Exercise measure participant share of venue volume against a configurable concentration limit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/concentration_monitor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_trading_concentration_monitor_bps(25U,100U)!=2500U)return 1;
    return umi_trading_concentration_monitor_breached(60U,100U,5000U)?0:2;
}
