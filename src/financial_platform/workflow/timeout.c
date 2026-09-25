/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/workflow/timeout.c
 *
 * PURPOSE:
 *   Apply Workflow Timeout without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/workflow/timeout.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_workflow_timeout(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "workflow.timeout",
        "Workflow Timeout",
        UMI_FINANCIAL_POLICY_AGEING,
        0.000000,
        3600000.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
