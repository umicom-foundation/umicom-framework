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
/*
 * Initialise accounting reconciliation rule from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_accounting_reconciliation_rule_init(UmiAccountingReconciliationRule *value,
    const char *id,
    int64_t tolerance_minor,
    bool auto_match) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->tolerance_minor=tolerance_minor;
    value->auto_match=auto_match;
    return umi_accounting_reconciliation_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting reconciliation rule satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_reconciliation_rule_valid(const UmiAccountingReconciliationRule *value) {
    return value!=NULL && (value->tolerance_minor>=0);
}

/*
 * Provide the accounting reconciliation rule automatic operation used by this module and
 * its client applications.
 */
bool umi_accounting_reconciliation_rule_automatic(const UmiAccountingReconciliationRule *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->auto_match;
}
