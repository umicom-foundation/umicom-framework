/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/kyc_profile.h
 *
 * PURPOSE:
 *   Record customer onboarding, risk and screening evidence without embedding application workflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_KYC_PROFILE_H
#define UMICOM_FINANCE_BANKING_KYC_PROFILE_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking kyc profile data shared with callers of this public contract.
 */
typedef struct UmiBankingKycProfile {
    UmiFinancialId id;
    UmiFinancialId customer_id;
    uint32_t risk_score_bps;
    bool identity_verified;
    bool sanctions_clear;
} UmiBankingKycProfile;
/**
 * Initialise banking kyc profile from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_banking_kyc_profile_init(UmiBankingKycProfile *value,
    const char *id,
    const char *customer_id,
    uint32_t risk_score_bps,
    bool identity_verified,
    bool sanctions_clear);
/**
 * Check that banking kyc profile satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_kyc_profile_valid(const UmiBankingKycProfile *value);
/**
 * Provide the banking kyc profile eligible operation used by this module and its client
 * applications.
 */
bool umi_banking_kyc_profile_eligible(const UmiBankingKycProfile *value);
#ifdef __cplusplus
}
#endif
#endif
