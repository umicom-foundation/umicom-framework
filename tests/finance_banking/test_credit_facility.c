/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_credit_facility.c
 *
 * PURPOSE:
 *   Exercise credit facility validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/credit_facility.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingCreditFacility v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_credit_facility_init(&v, "credit-1", "cust-1", 50000, 12500)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_banking_credit_facility_available_minor(&v)!=37500) return 2;
    return 0;
}
