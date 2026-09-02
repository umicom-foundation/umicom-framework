/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_limit_utilization.c
 *
 * PURPOSE:
 *   Exercise limit utilization validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/limit_utilization.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryLimitUtilization v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_limit_utilization_init(&v, "util", 750, 1000) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_limit_utilization_utilization_bps(&v)!=7500U)return 2;
    return 0;
}
