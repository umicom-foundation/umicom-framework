/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/party/kyc_readiness.h
 *
 * PURPOSE:
 *   Evaluate KYC Readiness through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_PARTY_KYC_READINESS_H
#define UMICOM_FINANCIAL_PLATFORM_PARTY_KYC_READINESS_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_party_kyc_readiness(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
