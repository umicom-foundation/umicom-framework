/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/workflow/approval.h
 *
 * PURPOSE:
 *   Evaluate Workflow Approval through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_WORKFLOW_APPROVAL_H
#define UMICOM_FINANCIAL_PLATFORM_WORKFLOW_APPROVAL_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_workflow_approval(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
