/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/audit/sequence.h
 *
 * PURPOSE:
 *   Evaluate Financial Audit Sequence through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_AUDIT_SEQUENCE_H
#define UMICOM_FINANCIAL_PLATFORM_AUDIT_SEQUENCE_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_audit_sequence(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
