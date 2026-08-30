/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/reconciliation_item.c
 *
 * PURPOSE:
 *   Implement compare external and ledger values and expose deterministic difference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/reconciliation_item.h"
#include <string.h>
UmiStatus umi_accounting_reconciliation_item_init(UmiAccountingReconciliationItem *value,
    const char *id,
    int64_t external_minor,
    int64_t ledger_minor,
    int64_t tolerance_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->external_minor=external_minor;
    value->ledger_minor=ledger_minor;
    value->tolerance_minor=tolerance_minor;
    return umi_accounting_reconciliation_item_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_accounting_reconciliation_item_valid(const UmiAccountingReconciliationItem *value) {
    return value!=NULL && (value->tolerance_minor>=0);
}

bool umi_accounting_reconciliation_item_matched(const UmiAccountingReconciliationItem *value) {
    if(value==NULL) return (bool)0;
    return umi_accounting_abs_i64(value->external_minor-value->ledger_minor)<=value->tolerance_minor;
}
