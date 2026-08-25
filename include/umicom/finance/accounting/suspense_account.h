/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/suspense_account.h
 *
 * PURPOSE:
 *   Track unresolved items and balance held in a suspense account.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_SUSPENSE_ACCOUNT_H
#define UMICOM_FINANCE_ACCOUNTING_SUSPENSE_ACCOUNT_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingSuspenseAccount {
    UmiFinancialId id;
    UmiFinancialId account_id;
    size_t unresolved_items;
    int64_t balance_minor;
} UmiAccountingSuspenseAccount;
UmiStatus umi_accounting_suspense_account_init(UmiAccountingSuspenseAccount *value,
    const char *id,
    const char *account_id,
    size_t unresolved_items,
    int64_t balance_minor);
bool umi_accounting_suspense_account_valid(const UmiAccountingSuspenseAccount *value);
bool umi_accounting_suspense_account_cleared(const UmiAccountingSuspenseAccount *value);
#ifdef __cplusplus
}
#endif
#endif
