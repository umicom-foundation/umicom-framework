/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_interest_rate.c
 *
 * PURPOSE:
 *   Exercise interest rate validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/interest_rate.h"
int main(void) {
    UmiBankingInterestRate v;
    if(umi_banking_interest_rate_init(&v, "rate-1", 525, 365U)!=UMI_STATUS_OK) return 1;
    if(!umi_banking_interest_rate_valid(&v)) return 2;
    return 0;
}
