/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/scenario_engine.h
 *
 * PURPOSE:
 *   Apply additive/relative market shocks to valuation inputs.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_SCENARIO_ENGINE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_SCENARIO_ENGINE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Apply one scenario to rate, spot and volatility market inputs. */
UmiStatus umi_quant_scenario_engine_apply(double base_rate, double base_spot, double base_volatility, double rate_shift, double spot_relative_shift, double vol_shift, double *out_rate, double *out_spot, double *out_volatility);

#ifdef __cplusplus
}
#endif

#endif
