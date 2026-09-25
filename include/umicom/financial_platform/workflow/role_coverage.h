/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/workflow/role_coverage.h
 *
 * PURPOSE:
 *   Evaluate Workflow Role Coverage through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_WORKFLOW_ROLE_COVERAGE_H
#define UMICOM_FINANCIAL_PLATFORM_WORKFLOW_ROLE_COVERAGE_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_workflow_role_coverage(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
