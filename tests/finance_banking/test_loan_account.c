/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_loan_account.c
 *
 * PURPOSE:
 *   Exercise loan account validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/loan_account.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingLoanAccount v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_loan_account_init(&v, "loan-1", "cust-1", "loan-prod", 100000, 75000, 650, UMI_BANKING_LOAN_ACTIVE)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_banking_loan_account_repaid_minor(&v)!=25000) return 2;
    return 0;
}
