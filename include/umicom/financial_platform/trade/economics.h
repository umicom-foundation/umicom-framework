/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/trade/economics.h
 *
 * PURPOSE:
 *   Evaluate Trade Economics through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_TRADE_ECONOMICS_H
#define UMICOM_FINANCIAL_PLATFORM_TRADE_ECONOMICS_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_trade_economics(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
