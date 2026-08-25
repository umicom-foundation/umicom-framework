/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_kyc_profile.c
 *
 * PURPOSE:
 *   Exercise kyc profile validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/kyc_profile.h"
int main(void) {
    UmiBankingKycProfile v;
    if(umi_banking_kyc_profile_init(&v, "kyc-1", "cust-1", 2500U, true, true)!=UMI_STATUS_OK) return 1;
    if(!umi_banking_kyc_profile_eligible(&v)) return 2;
    return 0;
}
