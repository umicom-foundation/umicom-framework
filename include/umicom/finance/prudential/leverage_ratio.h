/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/leverage_ratio.h
 *
 * PURPOSE:
 *   Calculate the prudential leverage ratio against total exposure measure.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LEVERAGE_RATIO_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LEVERAGE_RATIO_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialLeverageRatio { double numerator; double denominator; double ratio; } UmiPrudentialLeverageRatio;
/* Calculate the guarded prudential ratio for this measure. */
UmiStatus umi_pru_leverage_ratio_calculate(UmiPrudentialLeverageRatio *result, double numerator, double denominator);

#ifdef __cplusplus
}
#endif

#endif
