/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/position/quantity.h
 *
 * PURPOSE:
 *   Evaluate Position Quantity through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_POSITION_QUANTITY_H
#define UMICOM_FINANCIAL_PLATFORM_POSITION_QUANTITY_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_position_quantity(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
