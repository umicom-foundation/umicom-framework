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
/*
 * Initialise banking kyc profile from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_banking_kyc_profile_init(UmiBankingKycProfile *value,
    const char *id,
    const char *customer_id,
    uint32_t risk_score_bps,
    bool identity_verified,
    bool sanctions_clear) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->risk_score_bps=risk_score_bps;
    value->identity_verified=identity_verified;
    value->sanctions_clear=sanctions_clear;
    return umi_banking_kyc_profile_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking kyc profile satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_kyc_profile_valid(const UmiBankingKycProfile *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->customer_id) && value->risk_score_bps<=10000U);
}

/*
 * Provide the banking kyc profile eligible operation used by this module and its client
 * applications.
 */
bool umi_banking_kyc_profile_eligible(const UmiBankingKycProfile *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->identity_verified && value->sanctions_clear && value->risk_score_bps<8000U;
}
