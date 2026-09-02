/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/suspense_account.c
 *
 * PURPOSE:
 *   Implement track unresolved items and balance held in a suspense account.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/suspense_account.h"
#include <string.h>
/*
 * Initialise accounting suspense account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_suspense_account_init(UmiAccountingSuspenseAccount *value,
    const char *id,
    const char *account_id,
    size_t unresolved_items,
    int64_t balance_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_accounting_id_assign(&value->account_id,account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->unresolved_items=unresolved_items;
    value->balance_minor=balance_minor;
    return umi_accounting_suspense_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting suspense account satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_suspense_account_valid(const UmiAccountingSuspenseAccount *value) {
    return value!=NULL && (true);
}

/*
 * Provide the accounting suspense account cleared operation used by this module and its
 * client applications.
 */
bool umi_accounting_suspense_account_cleared(const UmiAccountingSuspenseAccount *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->unresolved_items==0U && value->balance_minor==0;
}
