/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/workflow/escalation.c
 *
 * PURPOSE:
 *   Apply Workflow Escalation without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/workflow/escalation.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_workflow_escalation(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "workflow.escalation",
        "Workflow Escalation",
        UMI_FINANCIAL_POLICY_AGEING,
        0.000000,
        1800000.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
