/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/position/market_value.h
 *
 * PURPOSE:
 *   Evaluate Position Market Value through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_POSITION_MARKET_VALUE_H
#define UMICOM_FINANCIAL_PLATFORM_POSITION_MARKET_VALUE_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_position_market_value(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
