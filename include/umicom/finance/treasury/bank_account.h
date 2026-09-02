/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/bank_account.h
 *
 * PURPOSE:
 *   Model external bank account routing and currency ownership for treasury.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_BANK_ACCOUNT_H
#define UMICOM_FINANCE_TREASURY_BANK_ACCOUNT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury bank account data shared with callers of this public contract.
 */
typedef struct UmiTreasuryBankAccount {
    char id[UMI_TREASURY_ID_CAPACITY];
    char bank_id[UMI_TREASURY_ID_CAPACITY];
    UmiCurrency currency;
    bool active;
} UmiTreasuryBankAccount;
/**
 * Initialise treasury bank account from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_bank_account_init(UmiTreasuryBankAccount *value,
    const char *id,
    const char *bank_id,
    const char *currency_code,
    bool active);
/**
 * Check that treasury bank account satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_bank_account_valid(const UmiTreasuryBankAccount *value);
/**
 * Provide the treasury bank account usable operation used by this module and its client
 * applications.
 */
bool umi_treasury_bank_account_usable(const UmiTreasuryBankAccount *value);
#ifdef __cplusplus
}
#endif
#endif
