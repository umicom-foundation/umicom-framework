/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_reconciliation_rule.c
 *
 * PURPOSE:
 *   Exercise reconciliation rule validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/reconciliation_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingReconciliationRule v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_reconciliation_rule_init(&v, "recon-rule", 5, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_accounting_reconciliation_rule_valid(&v)) return 2;
    return 0;
}
