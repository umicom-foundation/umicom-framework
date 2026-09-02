/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_custody_account.c
 *
 * PURPOSE:
 *   Exercise custody account validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/custody_account.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryCustodyAccount v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_custody_account_init(&v, "cust", "custodian", true) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_treasury_custody_account_is_segregated(&v))return 2;
    return 0;
}
