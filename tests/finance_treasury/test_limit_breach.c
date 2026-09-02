/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_limit_breach.c
 *
 * PURPOSE:
 *   Exercise limit breach validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/limit_breach.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryLimitBreach v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_limit_breach_init(&v, "breach", 50, false) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_treasury_limit_breach_open(&v))return 2;
    return 0;
}
