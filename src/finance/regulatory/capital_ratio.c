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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/capital_ratio.h"

/*
 * Provide the reg capital ratio compute operation used by this module and its client
 * applications.
 */
UmiStatus umi_reg_capital_ratio_compute(UmiCapitalRatio *r,double capital,double rwa){ UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_reg_ratio(capital,rwa,&r->ratio);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;r->capital=capital;r->risk_weighted_assets=rwa;return UMI_STATUS_OK; }
