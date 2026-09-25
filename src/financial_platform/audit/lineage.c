/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/audit/lineage.c
 *
 * PURPOSE:
 *   Apply Financial Audit Lineage without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/audit/lineage.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_audit_lineage(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "audit.lineage",
        "Financial Audit Lineage",
        UMI_FINANCIAL_POLICY_COVERAGE,
        100.000000,
        95.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
