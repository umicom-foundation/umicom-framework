/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/kyc_profile.c
 *
 * PURPOSE:
 *   Implement record customer onboarding, risk and screening evidence without embedding application workflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/kyc_profile.h"
#include <string.h>
UmiStatus umi_banking_kyc_profile_init(UmiBankingKycProfile *value,
    const char *id,
    const char *customer_id,
    uint32_t risk_score_bps,
    bool identity_verified,
    bool sanctions_clear) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->risk_score_bps=risk_score_bps;
    value->identity_verified=identity_verified;
    value->sanctions_clear=sanctions_clear;
    return umi_banking_kyc_profile_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_kyc_profile_valid(const UmiBankingKycProfile *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->customer_id) && value->risk_score_bps<=10000U);
}

bool umi_banking_kyc_profile_eligible(const UmiBankingKycProfile *value) {
    if(value==NULL) return (bool)0;
    return value->identity_verified && value->sanctions_clear && value->risk_score_bps<8000U;
}
