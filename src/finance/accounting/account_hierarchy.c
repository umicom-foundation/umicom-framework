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
UmiStatus umi_accounting_account_hierarchy_init(UmiAccountingAccountHierarchy *value,
    const char *id,
    const char *parent_account_id,
    const char *child_account_id) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_accounting_id_assign(&value->parent_account_id,parent_account_id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_accounting_id_assign(&value->child_account_id,child_account_id);
    if(rc!=UMI_STATUS_OK) return rc;
    return umi_accounting_account_hierarchy_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_accounting_account_hierarchy_valid(const UmiAccountingAccountHierarchy *value) {
    return value!=NULL && (umi_financial_id_compare(&value->parent_account_id,&value->child_account_id)!=0);
}

bool umi_accounting_account_hierarchy_self_reference(const UmiAccountingAccountHierarchy *value) {
    if(value==NULL) return (bool)0;
    return umi_financial_id_compare(&value->parent_account_id,&value->child_account_id)==0;
}
