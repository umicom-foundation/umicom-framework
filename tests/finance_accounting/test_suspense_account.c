/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_suspense_account.c
 *
 * PURPOSE:
 *   Exercise suspense account validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/suspense_account.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingSuspenseAccount v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_suspense_account_init(&v, "susp-1", "9999", 0U, 0)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_accounting_suspense_account_cleared(&v)) return 2;
    return 0;
}
