/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/workflow/completion.c
 *
 * PURPOSE:
 *   Apply Workflow Completion without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/workflow/completion.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_workflow_completion(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "workflow.completion",
        "Workflow Completion",
        UMI_FINANCIAL_POLICY_COMPLETION,
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
