/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/audit/readiness.c
 *
 * PURPOSE:
 *   Apply Financial Audit Readiness without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/audit/readiness.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_audit_readiness(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "audit.readiness",
        "Financial Audit Readiness",
        UMI_FINANCIAL_POLICY_HEALTH,
        90.000000,
        75.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
