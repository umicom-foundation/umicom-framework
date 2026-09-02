/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/treasury_account.h
 *
 * PURPOSE:
 *   Model a treasury-managed account with currency and operating balance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_TREASURY_ACCOUNT_H
#define UMICOM_FINANCE_TREASURY_TREASURY_ACCOUNT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury treasury account data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryTreasuryAccount {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiCurrency currency;
    int64_t balance_minor;
    int64_t minimum_balance_minor;
} UmiTreasuryTreasuryAccount;
/**
 * Initialise treasury treasury account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_treasury_account_init(UmiTreasuryTreasuryAccount *value,
    const char *id,
    const char *currency_code,
    int64_t balance_minor,
    int64_t minimum_balance_minor);
/**
 * Check that treasury treasury account satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_treasury_account_valid(const UmiTreasuryTreasuryAccount *value);
/**
 * Provide the treasury treasury account headroom minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_treasury_account_headroom_minor(const UmiTreasuryTreasuryAccount *value);
#ifdef __cplusplus
}
#endif
#endif
