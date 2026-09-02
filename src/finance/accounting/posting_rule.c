/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/posting_rule.c
 *
 * PURPOSE:
 *   Implement map canonical accounting event types to debit and credit ledger accounts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/posting_rule.h"
#include <string.h>
/*
 * Initialise accounting posting rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_posting_rule_init(UmiAccountingPostingRule *value,
    const char *id,
    const char *event_type,
    const char *debit_account_id,
    const char *credit_account_id,
    bool active) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->event_type,sizeof value->event_type,event_type);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    rc=umi_accounting_id_assign(&value->debit_account_id,debit_account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_accounting_id_assign(&value->credit_account_id,credit_account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->active=active;
    return umi_accounting_posting_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting posting rule satisfies its contract before another service relies
 * on it.
 */
bool umi_accounting_posting_rule_valid(const UmiAccountingPostingRule *value) {
    return value!=NULL && (value->event_type[0]!='\0' && umi_financial_id_compare(&value->debit_account_id,&value->credit_account_id)!=0);
}

/*
 * Provide the accounting posting rule usable operation used by this module and its client
 * applications.
 */
bool umi_accounting_posting_rule_usable(const UmiAccountingPostingRule *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->active;
}
