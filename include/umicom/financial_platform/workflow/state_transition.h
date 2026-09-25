/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/workflow/state_transition.h
 *
 * PURPOSE:
 *   Evaluate Workflow State Transition through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_WORKFLOW_STATE_TRANSITION_H
#define UMICOM_FINANCIAL_PLATFORM_WORKFLOW_STATE_TRANSITION_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_workflow_state_transition(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
