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
typedef struct UmiAccountingTrialBalance {
    UmiFinancialId id;
    int64_t debit_minor;
    int64_t credit_minor;
    size_t account_count;
} UmiAccountingTrialBalance;
UmiStatus umi_accounting_trial_balance_init(UmiAccountingTrialBalance *value,
    const char *id,
    int64_t debit_minor,
    int64_t credit_minor,
    size_t account_count);
bool umi_accounting_trial_balance_valid(const UmiAccountingTrialBalance *value);
bool umi_accounting_trial_balance_balanced(const UmiAccountingTrialBalance *value);
#ifdef __cplusplus
}
#endif
#endif
