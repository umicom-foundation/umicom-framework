/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_treasury_account.c
 *
 * PURPOSE:
 *   Exercise treasury account validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_account.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryTreasuryAccount v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_treasury_account_init(&v, "treasury-gbp", "GBP", 1000, 200) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_treasury_treasury_account_headroom_minor(&v) != 800) return 2;
    return 0;
}
