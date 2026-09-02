/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_bank_account.c
 *
 * PURPOSE:
 *   Exercise bank account validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/bank_account.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryBankAccount v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_bank_account_init(&v, "ba-1", "bank-1", "GBP", true) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_treasury_bank_account_usable(&v))return 2;
    return 0;
}
