/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_interest_accrual.c
 *
 * PURPOSE:
 *   Exercise interest accrual validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/interest_accrual.h"
int main(void) {
    UmiBankingInterestAccrual v;
    if(umi_banking_interest_accrual_init(&v, "accrual-1", 1000000, 500, 365U, 365U)!=UMI_STATUS_OK) return 1;
    if(umi_banking_interest_accrual_accrued_minor(&v)!=50000) return 2;
    return 0;
}
