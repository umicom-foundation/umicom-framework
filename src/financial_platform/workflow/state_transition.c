/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/workflow/state_transition.c
 *
 * PURPOSE:
 *   Apply Workflow State Transition without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/financial_platform/workflow/state_transition.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_workflow_state_transition(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "workflow.state-transition",
        "Workflow State Transition",
        UMI_FINANCIAL_POLICY_TRANSITION,
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

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_financial_platform_evaluate_workflow_state_transition_batch, umi_financial_platform_evaluate_workflow_state_transition,
    UmiFinancialPlatformInput, UmiFinancialPlatformSnapshot)
