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
UmiStatus umi_accounting_reconciliation_run_init(UmiAccountingReconciliationRun *value,
    const char *id,
    size_t total_items,
    size_t matched_items,
    int64_t residual_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->total_items=total_items;
    value->matched_items=matched_items;
    value->residual_minor=residual_minor;
    return umi_accounting_reconciliation_run_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_accounting_reconciliation_run_valid(const UmiAccountingReconciliationRun *value) {
    return value!=NULL && (value->matched_items<=value->total_items);
}

bool umi_accounting_reconciliation_run_complete(const UmiAccountingReconciliationRun *value) {
    if(value==NULL) return (bool)0;
    return value->total_items==value->matched_items && value->residual_minor==0;
}
