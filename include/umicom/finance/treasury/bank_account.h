/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/bank_account.h
 *
 * PURPOSE:
 *   Model external bank account routing and currency ownership for treasury.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_BANK_ACCOUNT_H
#define UMICOM_FINANCE_TREASURY_BANK_ACCOUNT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryBankAccount {
    char id[UMI_TREASURY_ID_CAPACITY];
    char bank_id[UMI_TREASURY_ID_CAPACITY];
    UmiCurrency currency;
    bool active;
} UmiTreasuryBankAccount;
UmiStatus umi_treasury_bank_account_init(UmiTreasuryBankAccount *value,
    const char *id,
    const char *bank_id,
    const char *currency_code,
    bool active);
bool umi_treasury_bank_account_valid(const UmiTreasuryBankAccount *value);
bool umi_treasury_bank_account_usable(const UmiTreasuryBankAccount *value);
#ifdef __cplusplus
}
#endif
#endif
