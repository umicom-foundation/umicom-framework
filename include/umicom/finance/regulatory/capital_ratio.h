/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/capital_ratio.h
 *
 * PURPOSE:
 *   Calculate regulatory capital adequacy ratios with guarded denominators.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_CAPITAL_RATIO_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_CAPITAL_RATIO_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCapitalRatio { double capital; double risk_weighted_assets; double ratio; } UmiCapitalRatio;
/* Compute capital divided by risk-weighted assets. */
UmiStatus umi_reg_capital_ratio_compute(UmiCapitalRatio *result,double capital,double risk_weighted_assets);

#ifdef __cplusplus
}
#endif

#endif
