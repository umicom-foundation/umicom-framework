/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/audit/sequence.c
 *
 * PURPOSE:
 *   Apply Financial Audit Sequence without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/audit/sequence.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_audit_sequence(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "audit.sequence",
        "Financial Audit Sequence",
        UMI_FINANCIAL_POLICY_SEQUENCE,
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
