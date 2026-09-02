/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_reconciliation_item.c
 *
 * PURPOSE:
 *   Exercise reconciliation item validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/reconciliation_item.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingReconciliationItem v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_reconciliation_item_init(&v, "recon-item", 1000, 998, 5)!=UMI_STATUS_OK) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(!umi_accounting_reconciliation_item_matched(&v)) return 2;
    return 0;
}
