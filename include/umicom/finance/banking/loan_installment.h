/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/loan_installment.h
 *
 * PURPOSE:
 *   Represent scheduled lending principal/interest and payment progress.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_LOAN_INSTALLMENT_H
#define UMICOM_FINANCE_BANKING_LOAN_INSTALLMENT_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingLoanInstallment {
    UmiFinancialId id;
    UmiFinancialId loan_id;
    UmiFinancialDate due_date;
    int64_t principal_minor;
    int64_t interest_minor;
    int64_t paid_minor;
} UmiBankingLoanInstallment;
UmiStatus umi_banking_loan_installment_init(UmiBankingLoanInstallment *value,
    const char *id,
    const char *loan_id,
    UmiFinancialDate due_date,
    int64_t principal_minor,
    int64_t interest_minor,
    int64_t paid_minor);
bool umi_banking_loan_installment_valid(const UmiBankingLoanInstallment *value);
int64_t umi_banking_loan_installment_outstanding_minor(const UmiBankingLoanInstallment *value);
#ifdef __cplusplus
}
#endif
#endif
