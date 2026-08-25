/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/capital_ratio.c
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

#include "umicom/finance/regulatory/capital_ratio.h"

UmiStatus umi_reg_capital_ratio_compute(UmiCapitalRatio *r,double capital,double rwa){ UmiStatus s;if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_reg_ratio(capital,rwa,&r->ratio);if(s!=UMI_STATUS_OK)return s;r->capital=capital;r->risk_weighted_assets=rwa;return UMI_STATUS_OK; }
