/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/reconciliation_rule.c
 *
 * PURPOSE:
 *   Implement define reconciliation tolerance and automatic matching policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/reconciliation_rule.h"
#include <string.h>
UmiStatus umi_accounting_reconciliation_rule_init(UmiAccountingReconciliationRule *value,
    const char *id,
    int64_t tolerance_minor,
    bool auto_match) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->tolerance_minor=tolerance_minor;
    value->auto_match=auto_match;
    return umi_accounting_reconciliation_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_accounting_reconciliation_rule_valid(const UmiAccountingReconciliationRule *value) {
    return value!=NULL && (value->tolerance_minor>=0);
}

bool umi_accounting_reconciliation_rule_automatic(const UmiAccountingReconciliationRule *value) {
    if(value==NULL) return (bool)0;
    return value->auto_match;
}
