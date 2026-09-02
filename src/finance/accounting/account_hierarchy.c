/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/account_hierarchy.c
 *
 * PURPOSE:
 *   Implement represent parent-child chart-of-accounts hierarchy edges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/account_hierarchy.h"
#include <string.h>
/*
 * Initialise accounting account hierarchy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_account_hierarchy_init(UmiAccountingAccountHierarchy *value,
    const char *id,
    const char *parent_account_id,
    const char *child_account_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_accounting_id_assign(&value->parent_account_id,parent_account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_accounting_id_assign(&value->child_account_id,child_account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    return umi_accounting_account_hierarchy_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting account hierarchy satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_account_hierarchy_valid(const UmiAccountingAccountHierarchy *value) {
    return value!=NULL && (umi_financial_id_compare(&value->parent_account_id,&value->child_account_id)!=0);
}

/*
 * Provide the accounting account hierarchy self reference operation used by this module
 * and its client applications.
 */
bool umi_accounting_account_hierarchy_self_reference(const UmiAccountingAccountHierarchy *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return umi_financial_id_compare(&value->parent_account_id,&value->child_account_id)==0;
}
