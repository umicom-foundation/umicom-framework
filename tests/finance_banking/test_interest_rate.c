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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingInterestRate v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_interest_rate_init(&v, "rate-1", 525, 365U)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_banking_interest_rate_valid(&v)) return 2;
    return 0;
}
