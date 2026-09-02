/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_execution_policy.c
 *
 * PURPOSE:
 *   Exercise define venue-count, participation and urgency bounds for execution strategies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/execution_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTradingExecutionPolicy v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_execution_policy_init(&v,4U,1000U,50U)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_execution_policy_valid(&v)) return 2;
     return 0;
}
