/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/trial_balance.c
 *
 * PURPOSE:
 *   Implement represent trial-balance debit and credit control totals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/trial_balance.h"
#include <string.h>
UmiStatus umi_accounting_trial_balance_init(UmiAccountingTrialBalance *value,
    const char *id,
    int64_t debit_minor,
    int64_t credit_minor,
    size_t account_count) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->debit_minor=debit_minor;
    value->credit_minor=credit_minor;
    value->account_count=account_count;
    return umi_accounting_trial_balance_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_accounting_trial_balance_valid(const UmiAccountingTrialBalance *value) {
    return value!=NULL && (value->debit_minor>=0 && value->credit_minor>=0);
}

bool umi_accounting_trial_balance_balanced(const UmiAccountingTrialBalance *value) {
    if(value==NULL) return (bool)0;
    return value->debit_minor==value->credit_minor;
}
