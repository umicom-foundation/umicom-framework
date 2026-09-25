/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/workflow/approval.c
 *
 * PURPOSE:
 *   Apply Workflow Approval without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/workflow/approval.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_workflow_approval(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "workflow.approval",
        "Workflow Approval",
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
