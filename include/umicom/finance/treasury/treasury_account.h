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
typedef struct UmiTreasuryTreasuryAccount {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiCurrency currency;
    int64_t balance_minor;
    int64_t minimum_balance_minor;
} UmiTreasuryTreasuryAccount;
UmiStatus umi_treasury_treasury_account_init(UmiTreasuryTreasuryAccount *value,
    const char *id,
    const char *currency_code,
    int64_t balance_minor,
    int64_t minimum_balance_minor);
bool umi_treasury_treasury_account_valid(const UmiTreasuryTreasuryAccount *value);
int64_t umi_treasury_treasury_account_headroom_minor(const UmiTreasuryTreasuryAccount *value);
#ifdef __cplusplus
}
#endif
#endif
