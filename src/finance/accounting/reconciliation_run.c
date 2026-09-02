/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/reconciliation_run.c
 *
 * PURPOSE:
 *   Implement summarise reconciliation matched/unmatched item counts and residual difference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/reconciliation_run.h"
#include <string.h>
/*
 * Initialise accounting reconciliation run from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_reconciliation_run_init(UmiAccountingReconciliationRun *value,
    const char *id,
    size_t total_items,
    size_t matched_items,
    int64_t residual_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->total_items=total_items;
    value->matched_items=matched_items;
    value->residual_minor=residual_minor;
    return umi_accounting_reconciliation_run_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting reconciliation run satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_reconciliation_run_valid(const UmiAccountingReconciliationRun *value) {
    return value!=NULL && (value->matched_items<=value->total_items);
}

/*
 * Provide the accounting reconciliation run complete operation used by this module and its
 * client applications.
 */
bool umi_accounting_reconciliation_run_complete(const UmiAccountingReconciliationRun *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->total_items==value->matched_items && value->residual_minor==0;
}
