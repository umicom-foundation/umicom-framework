/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/integrated_stress_result.h
 *
 * PURPOSE:
 *   Combine capital and liquidity stress outcomes into one control decision.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_INTEGRATED_STRESS_RESULT_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_INTEGRATED_STRESS_RESULT_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialIntegratedStressResult { int capital_passed; int liquidity_passed; int overall_passed; UmiPrudentialSeverity severity; } UmiPrudentialIntegratedStressResult;
/* Combine capital and liquidity control outcomes without allowing one to mask the other. */
UmiStatus umi_pru_integrated_stress_result_evaluate(UmiPrudentialIntegratedStressResult *result, int capital_passed, int liquidity_passed);

#ifdef __cplusplus
}
#endif

#endif
