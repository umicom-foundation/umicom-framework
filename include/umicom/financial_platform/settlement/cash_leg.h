/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/settlement/cash_leg.h
 *
 * PURPOSE:
 *   Evaluate Settlement Cash Leg through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_SETTLEMENT_CASH_LEG_H
#define UMICOM_FINANCIAL_PLATFORM_SETTLEMENT_CASH_LEG_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_settlement_cash_leg(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
