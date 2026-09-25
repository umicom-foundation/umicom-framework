/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/position/gross_exposure.h
 *
 * PURPOSE:
 *   Evaluate Gross Exposure through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_POSITION_GROSS_EXPOSURE_H
#define UMICOM_FINANCIAL_PLATFORM_POSITION_GROSS_EXPOSURE_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_position_gross_exposure(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
