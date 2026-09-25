/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/settlement/fail_age.h
 *
 * PURPOSE:
 *   Evaluate Settlement Fail Age through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_SETTLEMENT_FAIL_AGE_H
#define UMICOM_FINANCIAL_PLATFORM_SETTLEMENT_FAIL_AGE_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_settlement_fail_age(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
