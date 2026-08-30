/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_loan_installment.c
 *
 * PURPOSE:
 *   Exercise loan installment validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/loan_installment.h"
int main(void) {
    UmiBankingLoanInstallment v;
    if(umi_banking_loan_installment_init(&v, "inst-1", "loan-1", (UmiFinancialDate){2026,9U,1U}, 8000, 500, 4000)!=UMI_STATUS_OK) return 1;
    if(umi_banking_loan_installment_outstanding_minor(&v)!=4500) return 2;
    return 0;
}
