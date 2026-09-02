/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/liquidity_stress_result.h
 *
 * PURPOSE:
 *   Represent post-stress liquidity ratio and regulatory headroom.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_STRESS_RESULT_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_STRESS_RESULT_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential liquidity stress result data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialLiquidityStressResult { double stressed_ratio; double minimum_ratio; double headroom; int passed; } UmiPrudentialLiquidityStressResult;
/* Evaluate a post-stress prudential ratio and retain explicit headroom. */
UmiStatus umi_pru_liquidity_stress_result_evaluate(UmiPrudentialLiquidityStressResult *result, double stressed_ratio, double minimum_ratio);

#ifdef __cplusplus
}
#endif

#endif
