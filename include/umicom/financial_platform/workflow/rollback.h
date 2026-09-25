/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/workflow/rollback.h
 *
 * PURPOSE:
 *   Evaluate Workflow Rollback Readiness through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_WORKFLOW_ROLLBACK_H
#define UMICOM_FINANCIAL_PLATFORM_WORKFLOW_ROLLBACK_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_workflow_rollback(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
