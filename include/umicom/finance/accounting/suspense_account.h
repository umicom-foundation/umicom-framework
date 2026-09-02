/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/suspense_account.h
 *
 * PURPOSE:
 *   Track unresolved items and balance held in a suspense account.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_SUSPENSE_ACCOUNT_H
#define UMICOM_FINANCE_ACCOUNTING_SUSPENSE_ACCOUNT_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting suspense account data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingSuspenseAccount {
    UmiFinancialId id;
    UmiFinancialId account_id;
    size_t unresolved_items;
    int64_t balance_minor;
} UmiAccountingSuspenseAccount;
/**
 * Initialise accounting suspense account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_suspense_account_init(UmiAccountingSuspenseAccount *value,
    const char *id,
    const char *account_id,
    size_t unresolved_items,
    int64_t balance_minor);
/**
 * Check that accounting suspense account satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_suspense_account_valid(const UmiAccountingSuspenseAccount *value);
/**
 * Provide the accounting suspense account cleared operation used by this module and its
 * client applications.
 */
bool umi_accounting_suspense_account_cleared(const UmiAccountingSuspenseAccount *value);
#ifdef __cplusplus
}
#endif
#endif
