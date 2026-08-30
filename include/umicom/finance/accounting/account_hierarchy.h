/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/account_hierarchy.h
 *
 * PURPOSE:
 *   Represent parent-child chart-of-accounts hierarchy edges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_ACCOUNT_HIERARCHY_H
#define UMICOM_FINANCE_ACCOUNTING_ACCOUNT_HIERARCHY_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingAccountHierarchy {
    UmiFinancialId id;
    UmiFinancialId parent_account_id;
    UmiFinancialId child_account_id;
} UmiAccountingAccountHierarchy;
UmiStatus umi_accounting_account_hierarchy_init(UmiAccountingAccountHierarchy *value,
    const char *id,
    const char *parent_account_id,
    const char *child_account_id);
bool umi_accounting_account_hierarchy_valid(const UmiAccountingAccountHierarchy *value);
bool umi_accounting_account_hierarchy_self_reference(const UmiAccountingAccountHierarchy *value);
#ifdef __cplusplus
}
#endif
#endif
