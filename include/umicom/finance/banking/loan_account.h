/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/loan_account.h
 *
 * PURPOSE:
 *   Represent lending principal, outstanding balance, rate and lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_LOAN_ACCOUNT_H
#define UMICOM_FINANCE_BANKING_LOAN_ACCOUNT_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking loan account data shared with callers of this public contract.
 */
typedef struct UmiBankingLoanAccount {
    UmiFinancialId id;
    UmiFinancialId customer_id;
    UmiFinancialId product_id;
    int64_t principal_minor;
    int64_t outstanding_minor;
    int32_t annual_rate_bps;
    UmiBankingLoanStatus status;
} UmiBankingLoanAccount;
/**
 * Initialise banking loan account from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_loan_account_init(UmiBankingLoanAccount *value,
    const char *id,
    const char *customer_id,
    const char *product_id,
    int64_t principal_minor,
    int64_t outstanding_minor,
    int32_t annual_rate_bps,
    UmiBankingLoanStatus status);
/**
 * Check that banking loan account satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_loan_account_valid(const UmiBankingLoanAccount *value);
/**
 * Provide the banking loan account repaid minor operation used by this module and its
 * client applications.
 */
int64_t umi_banking_loan_account_repaid_minor(const UmiBankingLoanAccount *value);
#ifdef __cplusplus
}
#endif
#endif
