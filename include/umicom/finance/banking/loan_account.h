/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/loan_account.h
 *
 * PURPOSE:
 *   Represent lending principal, outstanding balance, rate and lifecycle state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_LOAN_ACCOUNT_H
#define UMICOM_FINANCE_BANKING_LOAN_ACCOUNT_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingLoanAccount {
    UmiFinancialId id;
    UmiFinancialId customer_id;
    UmiFinancialId product_id;
    int64_t principal_minor;
    int64_t outstanding_minor;
    int32_t annual_rate_bps;
    UmiBankingLoanStatus status;
} UmiBankingLoanAccount;
UmiStatus umi_banking_loan_account_init(UmiBankingLoanAccount *value,
    const char *id,
    const char *customer_id,
    const char *product_id,
    int64_t principal_minor,
    int64_t outstanding_minor,
    int32_t annual_rate_bps,
    UmiBankingLoanStatus status);
bool umi_banking_loan_account_valid(const UmiBankingLoanAccount *value);
int64_t umi_banking_loan_account_repaid_minor(const UmiBankingLoanAccount *value);
#ifdef __cplusplus
}
#endif
#endif
