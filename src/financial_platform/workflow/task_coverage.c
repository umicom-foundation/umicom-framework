/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/workflow/task_coverage.c
 *
 * PURPOSE:
 *   Apply Workflow Task Coverage without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/workflow/task_coverage.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_workflow_task_coverage(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "workflow.task-coverage",
        "Workflow Task Coverage",
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
