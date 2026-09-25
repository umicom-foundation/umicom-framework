/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/audit/readiness.h
 *
 * PURPOSE:
 *   Evaluate Financial Audit Readiness through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_AUDIT_READINESS_H
#define UMICOM_FINANCIAL_PLATFORM_AUDIT_READINESS_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_audit_readiness(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
