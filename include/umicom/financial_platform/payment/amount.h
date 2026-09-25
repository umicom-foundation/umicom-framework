/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/payment/amount.h
 *
 * PURPOSE:
 *   Evaluate Payment Amount through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_PAYMENT_AMOUNT_H
#define UMICOM_FINANCIAL_PLATFORM_PAYMENT_AMOUNT_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_payment_amount(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
