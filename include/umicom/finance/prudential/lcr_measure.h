/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/lcr_measure.h
 *
 * PURPOSE:
 *   Calculate the liquidity coverage ratio from HQLA and net stressed outflows.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LCR_MEASURE_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LCR_MEASURE_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential lcr measure data shared with callers of this public contract.
 */
typedef struct UmiPrudentialLcrMeasure { double numerator; double denominator; double ratio; } UmiPrudentialLcrMeasure;
/* Calculate the guarded prudential ratio for this measure. */
UmiStatus umi_pru_lcr_measure_calculate(UmiPrudentialLcrMeasure *result, double numerator, double denominator);

#ifdef __cplusplus
}
#endif

#endif
