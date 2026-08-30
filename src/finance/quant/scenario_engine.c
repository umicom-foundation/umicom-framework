/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/scenario_engine.c
 *
 * PURPOSE:
 *   Apply additive/relative market shocks to valuation inputs.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/scenario_engine.h"

#include <math.h>
#include <string.h>

UmiStatus umi_quant_scenario_engine_apply(double base_rate,double base_spot,double base_volatility,double rate_shift,double spot_relative_shift,double vol_shift,double *out_rate,double *out_spot,double *out_volatility){if(out_rate==NULL||out_spot==NULL||out_volatility==NULL||base_spot<0.0||base_volatility<0.0)return UMI_STATUS_INVALID_ARGUMENT;*out_rate=base_rate+rate_shift;*out_spot=base_spot*(1.0+spot_relative_shift);*out_volatility=fmax(base_volatility+vol_shift,0.0);return UMI_STATUS_OK;}
