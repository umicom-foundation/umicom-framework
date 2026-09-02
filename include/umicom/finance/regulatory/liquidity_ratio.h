/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/liquidity_ratio.h
 *
 * PURPOSE:
 *   Calculate regulatory liquidity coverage ratios with capped net outflows.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_LIQUIDITY_RATIO_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_LIQUIDITY_RATIO_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the liquidity ratio data shared with callers of this public contract.
 */
typedef struct UmiLiquidityRatio { double liquid_assets; double net_outflows; double ratio; } UmiLiquidityRatio;
/* Compute liquid assets divided by positive net outflows. */
UmiStatus umi_reg_liquidity_ratio_compute(UmiLiquidityRatio *result,double liquid_assets,double net_outflows);

#ifdef __cplusplus
}
#endif

#endif
