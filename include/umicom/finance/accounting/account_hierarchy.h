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
/**
 * Represent the accounting account hierarchy data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingAccountHierarchy {
    UmiFinancialId id;
    UmiFinancialId parent_account_id;
    UmiFinancialId child_account_id;
} UmiAccountingAccountHierarchy;
/**
 * Initialise accounting account hierarchy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_account_hierarchy_init(UmiAccountingAccountHierarchy *value,
    const char *id,
    const char *parent_account_id,
    const char *child_account_id);
/**
 * Check that accounting account hierarchy satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_account_hierarchy_valid(const UmiAccountingAccountHierarchy *value);
/**
 * Provide the accounting account hierarchy self reference operation used by this module
 * and its client applications.
 */
bool umi_accounting_account_hierarchy_self_reference(const UmiAccountingAccountHierarchy *value);
#ifdef __cplusplus
}
#endif
#endif
