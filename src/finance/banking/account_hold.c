/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/account_hold.c
 *
 * PURPOSE:
 *   Implement represent ring-fenced account funds and explicit release state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/account_hold.h"
#include <string.h>
/*
 * Initialise banking account hold from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_account_hold_init(UmiBankingAccountHold *value,
    const char *id,
    const char *account_id,
    int64_t amount_minor,
    bool active) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiBankingAccountHold candidate = {0};
    UmiStatus rc=umi_banking_id_assign(&candidate.id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&candidate.account_id,account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    candidate.amount_minor=amount_minor;
    candidate.active=active;
    if (!umi_banking_account_hold_valid(&candidate)) return UMI_STATUS_INVALID_ARGUMENT;
    *value = candidate;
    return UMI_STATUS_OK;
}
/*
 * Check that banking account hold satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_account_hold_valid(const UmiBankingAccountHold *value) {
    return value!=NULL && umi_financial_id_is_valid(&value->id) &&
        umi_financial_id_is_valid(&value->account_id) && value->amount_minor>0;
}

/*
 * Provide the banking account hold releasable operation used by this module and its client
 * applications.
 */
bool umi_banking_account_hold_releasable(const UmiBankingAccountHold *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return umi_banking_account_hold_valid(value) && value->active;
}
