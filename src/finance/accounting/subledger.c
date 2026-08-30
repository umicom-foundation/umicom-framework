/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/subledger.c
 *
 * PURPOSE:
 *   Implement represent sub-ledger control-account totals for reconciliation with the general ledger.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/subledger.h"
#include <string.h>
UmiStatus umi_accounting_subledger_init(UmiAccountingSubledger *value,
    const char *id,
    const char *control_account_id,
    int64_t debit_minor,
    int64_t credit_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_accounting_id_assign(&value->control_account_id,control_account_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->debit_minor=debit_minor;
    value->credit_minor=credit_minor;
    return umi_accounting_subledger_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_accounting_subledger_valid(const UmiAccountingSubledger *value) {
    return value!=NULL && (value->debit_minor>=0 && value->credit_minor>=0);
}

int64_t umi_accounting_subledger_net_minor(const UmiAccountingSubledger *value) {
    if(value==NULL) return (int64_t)0;
    return value->debit_minor-value->credit_minor;
}
