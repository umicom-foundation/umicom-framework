/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/party/ownership_consistency.h
 *
 * PURPOSE:
 *   Evaluate Ownership Consistency through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_PARTY_OWNERSHIP_CONSISTENCY_H
#define UMICOM_FINANCIAL_PLATFORM_PARTY_OWNERSHIP_CONSISTENCY_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_party_ownership_consistency(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
