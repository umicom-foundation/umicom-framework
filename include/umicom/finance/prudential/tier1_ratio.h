/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/tier1_ratio.h
 *
 * PURPOSE:
 *   Calculate the Tier 1 capital ratio.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_TIER1_RATIO_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_TIER1_RATIO_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialTier1Ratio { double numerator; double denominator; double ratio; } UmiPrudentialTier1Ratio;
/* Calculate the guarded prudential ratio for this measure. */
UmiStatus umi_pru_tier1_ratio_calculate(UmiPrudentialTier1Ratio *result, double numerator, double denominator);

#ifdef __cplusplus
}
#endif

#endif
