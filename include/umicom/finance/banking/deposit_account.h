/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/deposit_account.h
 *
 * PURPOSE:
 *   Represent customer deposit balances, currency and available funds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_DEPOSIT_ACCOUNT_H
#define UMICOM_FINANCE_BANKING_DEPOSIT_ACCOUNT_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingDepositAccount {
    UmiFinancialId id;
    UmiFinancialId customer_id;
    UmiFinancialId product_id;
    UmiCurrency currency;
    int64_t ledger_minor;
    int64_t available_minor;
    UmiBankingAccountStatus status;
} UmiBankingDepositAccount;
UmiStatus umi_banking_deposit_account_init(UmiBankingDepositAccount *value,
    const char *id,
    const char *customer_id,
    const char *product_id,
    const char *currency_code,
    int64_t ledger_minor,
    int64_t available_minor,
    UmiBankingAccountStatus status);
bool umi_banking_deposit_account_valid(const UmiBankingDepositAccount *value);
int64_t umi_banking_deposit_account_reserved_minor(const UmiBankingDepositAccount *value);
#ifdef __cplusplus
}
#endif
#endif
