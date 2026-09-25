/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/audit/causation.h
 *
 * PURPOSE:
 *   Evaluate Financial Audit Causation through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_AUDIT_CAUSATION_H
#define UMICOM_FINANCIAL_PLATFORM_AUDIT_CAUSATION_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_audit_causation(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
