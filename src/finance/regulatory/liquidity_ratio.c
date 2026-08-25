/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/liquidity_ratio.c
 *
 * PURPOSE:
 *   Calculate regulatory liquidity coverage ratios with capped net outflows.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/liquidity_ratio.h"

UmiStatus umi_reg_liquidity_ratio_compute(UmiLiquidityRatio *r,double assets,double outflows){ UmiStatus s;if(r==NULL||assets<0.0||outflows<=0.0)return UMI_STATUS_INVALID_ARGUMENT;s=umi_reg_ratio(assets,outflows,&r->ratio);if(s!=UMI_STATUS_OK)return s;r->liquid_assets=assets;r->net_outflows=outflows;return UMI_STATUS_OK; }
