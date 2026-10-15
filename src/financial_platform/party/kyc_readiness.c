/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/party/kyc_readiness.c
 *
 * PURPOSE:
 *   Apply KYC Readiness without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/financial_platform/party/kyc_readiness.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_party_kyc_readiness(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "party.kyc-readiness",
        "KYC Readiness",
        UMI_FINANCIAL_POLICY_APPROVAL,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_financial_platform_evaluate_party_kyc_readiness_batch, umi_financial_platform_evaluate_party_kyc_readiness,
    UmiFinancialPlatformInput, UmiFinancialPlatformSnapshot)
