/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/loan_installment.h
 *
 * PURPOSE:
 *   Represent scheduled lending principal/interest and payment progress.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_LOAN_INSTALLMENT_H
#define UMICOM_FINANCE_BANKING_LOAN_INSTALLMENT_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking loan installment data shared with callers of this public contract.
 */
typedef struct UmiBankingLoanInstallment {
    UmiFinancialId id;
    UmiFinancialId loan_id;
    UmiFinancialDate due_date;
    int64_t principal_minor;
    int64_t interest_minor;
    int64_t paid_minor;
} UmiBankingLoanInstallment;
/**
 * Initialise banking loan installment from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_loan_installment_init(UmiBankingLoanInstallment *value,
    const char *id,
    const char *loan_id,
    UmiFinancialDate due_date,
    int64_t principal_minor,
    int64_t interest_minor,
    int64_t paid_minor);
/**
 * Check that banking loan installment satisfies its contract before another service relies
 * on it.
 */
bool umi_banking_loan_installment_valid(const UmiBankingLoanInstallment *value);
/**
 * Provide the banking loan installment outstanding minor operation used by this module and
 * its client applications.
 */
int64_t umi_banking_loan_installment_outstanding_minor(const UmiBankingLoanInstallment *value);
#ifdef __cplusplus
}
#endif
#endif
