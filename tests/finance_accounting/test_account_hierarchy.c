/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_account_hierarchy.c
 *
 * PURPOSE:
 *   Exercise account hierarchy validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/account_hierarchy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingAccountHierarchy v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_account_hierarchy_init(&v, "edge-1", "1000", "1010")!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_accounting_account_hierarchy_self_reference(&v)) return 2;
    return 0;
}
