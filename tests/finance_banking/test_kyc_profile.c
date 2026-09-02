/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_kyc_profile.c
 *
 * PURPOSE:
 *   Exercise kyc profile validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/kyc_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingKycProfile v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_kyc_profile_init(&v, "kyc-1", "cust-1", 2500U, true, true)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_banking_kyc_profile_eligible(&v)) return 2;
    return 0;
}
