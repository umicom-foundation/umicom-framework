/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/capital_stress_result.h
 *
 * PURPOSE:
 *   Represent post-stress capital ratio and regulatory headroom.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_STRESS_RESULT_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_STRESS_RESULT_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential capital stress result data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialCapitalStressResult { double stressed_ratio; double minimum_ratio; double headroom; int passed; } UmiPrudentialCapitalStressResult;
/* Evaluate a post-stress prudential ratio and retain explicit headroom. */
UmiStatus umi_pru_capital_stress_result_evaluate(UmiPrudentialCapitalStressResult *result, double stressed_ratio, double minimum_ratio);

#ifdef __cplusplus
}
#endif

#endif
