/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/funding_concentration.h
 *
 * PURPOSE:
 *   Measure largest-source concentration within total funding.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_FUNDING_CONCENTRATION_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_FUNDING_CONCENTRATION_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialFundingConcentration { double numerator; double denominator; double ratio; } UmiPrudentialFundingConcentration;
/* Calculate the guarded prudential ratio for this measure. */
UmiStatus umi_pru_funding_concentration_calculate(UmiPrudentialFundingConcentration *result, double numerator, double denominator);

#ifdef __cplusplus
}
#endif

#endif
