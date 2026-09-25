/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/party/identity.c
 *
 * PURPOSE:
 *   Apply Party Identity without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/party/identity.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_party_identity(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "party.identity",
        "Party Identity",
        UMI_FINANCIAL_POLICY_VALIDATION,
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
