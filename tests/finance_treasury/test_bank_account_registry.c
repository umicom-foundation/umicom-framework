/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_bank_account_registry.c
 *
 * PURPOSE:
 *   Verify bounded bank account registry registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/bank_account_registry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTreasuryBankAccountRegistry r; UmiTreasuryBankAccount item; umi_treasury_bank_account_registry_init(&r);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_bank_account_init(&item, "ba-1", "bank-1", "USD", true) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_bank_account_registry_add(&r,&item) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_treasury_bank_account_registry_find(&r,item.id) == NULL) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_bank_account_registry_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
