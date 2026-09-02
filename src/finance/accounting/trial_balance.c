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
/*
 * Initialise accounting trial balance from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_trial_balance_init(UmiAccountingTrialBalance *value,
    const char *id,
    int64_t debit_minor,
    int64_t credit_minor,
    size_t account_count) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->debit_minor=debit_minor;
    value->credit_minor=credit_minor;
    value->account_count=account_count;
    return umi_accounting_trial_balance_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting trial balance satisfies its contract before another service relies
 * on it.
 */
bool umi_accounting_trial_balance_valid(const UmiAccountingTrialBalance *value) {
    return value!=NULL && (value->debit_minor>=0 && value->credit_minor>=0);
}

/*
 * Provide the accounting trial balance balanced operation used by this module and its
 * client applications.
 */
bool umi_accounting_trial_balance_balanced(const UmiAccountingTrialBalance *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->debit_minor==value->credit_minor;
}
