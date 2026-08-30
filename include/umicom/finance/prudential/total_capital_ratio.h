/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/total_capital_ratio.h
 *
 * PURPOSE:
 *   Calculate the total regulatory capital ratio.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_TOTAL_CAPITAL_RATIO_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_TOTAL_CAPITAL_RATIO_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialTotalCapitalRatio { double numerator; double denominator; double ratio; } UmiPrudentialTotalCapitalRatio;
/* Calculate the guarded prudential ratio for this measure. */
UmiStatus umi_pru_total_capital_ratio_calculate(UmiPrudentialTotalCapitalRatio *result, double numerator, double denominator);

#ifdef __cplusplus
}
#endif

#endif
