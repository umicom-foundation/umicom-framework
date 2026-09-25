/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/payment/route.h
 *
 * PURPOSE:
 *   Evaluate Payment Route through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_PAYMENT_ROUTE_H
#define UMICOM_FINANCIAL_PLATFORM_PAYMENT_ROUTE_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_payment_route(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
