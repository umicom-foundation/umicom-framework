/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/payment/cutoff.h
 *
 * PURPOSE:
 *   Evaluate Payment Cut-off through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_PAYMENT_CUTOFF_H
#define UMICOM_FINANCIAL_PLATFORM_PAYMENT_CUTOFF_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_payment_cutoff(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
