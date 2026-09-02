/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/nsfr_measure.h
 *
 * PURPOSE:
 *   Calculate the net stable funding ratio.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_NSFR_MEASURE_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_NSFR_MEASURE_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential nsfr measure data shared with callers of this public contract.
 */
typedef struct UmiPrudentialNsfrMeasure { double numerator; double denominator; double ratio; } UmiPrudentialNsfrMeasure;
/* Calculate the guarded prudential ratio for this measure. */
UmiStatus umi_pru_nsfr_measure_calculate(UmiPrudentialNsfrMeasure *result, double numerator, double denominator);

#ifdef __cplusplus
}
#endif

#endif
