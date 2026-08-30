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
int main(void) {
    UmiAccountingReconciliationRule v;
    if(umi_accounting_reconciliation_rule_init(&v, "recon-rule", 5, true)!=UMI_STATUS_OK) return 1;
    if(!umi_accounting_reconciliation_rule_valid(&v)) return 2;
    return 0;
}
