/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/workflow/rollback.c
 *
 * PURPOSE:
 *   Apply Workflow Rollback Readiness without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/workflow/rollback.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_workflow_rollback(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "workflow.rollback",
        "Workflow Rollback Readiness",
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
