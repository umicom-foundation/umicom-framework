/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/party/counterparty.c
 *
 * PURPOSE:
 *   Apply Counterparty Readiness without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/party/counterparty.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_party_counterparty(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "party.counterparty",
        "Counterparty Readiness",
        UMI_FINANCIAL_POLICY_READINESS,
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
