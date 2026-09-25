/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/party/lifecycle.h
 *
 * PURPOSE:
 *   Evaluate Party Lifecycle through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_PARTY_LIFECYCLE_H
#define UMICOM_FINANCIAL_PLATFORM_PARTY_LIFECYCLE_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_party_lifecycle(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
