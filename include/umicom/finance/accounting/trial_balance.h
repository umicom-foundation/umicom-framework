/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/trial_balance.h
 *
 * PURPOSE:
 *   Represent trial-balance debit and credit control totals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_TRIAL_BALANCE_H
#define UMICOM_FINANCE_ACCOUNTING_TRIAL_BALANCE_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting trial balance data shared with callers of this public contract.
 */
typedef struct UmiAccountingTrialBalance {
    UmiFinancialId id;
    int64_t debit_minor;
    int64_t credit_minor;
    size_t account_count;
} UmiAccountingTrialBalance;
/**
 * Initialise accounting trial balance from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_trial_balance_init(UmiAccountingTrialBalance *value,
    const char *id,
    int64_t debit_minor,
    int64_t credit_minor,
    size_t account_count);
/**
 * Check that accounting trial balance satisfies its contract before another service relies
 * on it.
 */
bool umi_accounting_trial_balance_valid(const UmiAccountingTrialBalance *value);
/**
 * Provide the accounting trial balance balanced operation used by this module and its
 * client applications.
 */
bool umi_accounting_trial_balance_balanced(const UmiAccountingTrialBalance *value);
#ifdef __cplusplus
}
#endif
#endif
