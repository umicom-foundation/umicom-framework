/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/ledger_balance.c
 *
 * PURPOSE:
 *   Implement calculate ledger-account net balance from debit/credit totals and normal side.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/ledger_balance.h"
#include <string.h>
UmiStatus umi_accounting_ledger_balance_init(UmiAccountingLedgerBalance *value,
    const char *id,
    const char *account_id,
    UmiAccountingNormalSide normal_side,
    int64_t debit_minor,
    int64_t credit_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_accounting_id_assign(&value->account_id,account_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->normal_side=normal_side;
    value->debit_minor=debit_minor;
    value->credit_minor=credit_minor;
    return umi_accounting_ledger_balance_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_accounting_ledger_balance_valid(const UmiAccountingLedgerBalance *value) {
    return value!=NULL && (value->debit_minor>=0 && value->credit_minor>=0 && (value->normal_side==UMI_ACCOUNTING_NORMAL_DEBIT||value->normal_side==UMI_ACCOUNTING_NORMAL_CREDIT));
}

int64_t umi_accounting_ledger_balance_net_minor(const UmiAccountingLedgerBalance *value) {
    if(value==NULL) return (int64_t)0;
    return value->normal_side==UMI_ACCOUNTING_NORMAL_DEBIT ? value->debit_minor-value->credit_minor : value->credit_minor-value->debit_minor;
}
