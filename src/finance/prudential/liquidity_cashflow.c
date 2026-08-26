/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/liquidity_cashflow.c
 *
 * PURPOSE:
 *   Represent a dated prudential liquidity inflow or outflow.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_cashflow.h"

UmiStatus umi_pru_liquidity_cashflow_init(UmiPrudentialLiquidityCashflow *flow,int32_t day,double inflow,double outflow,UmiCurrency currency) { if(flow==NULL||day<0||inflow<0.0||outflow<0.0)return UMI_STATUS_INVALID_ARGUMENT; flow->horizon_day=day; flow->inflow=inflow; flow->outflow=outflow; flow->currency=currency; return UMI_STATUS_OK; }
double umi_pru_liquidity_cashflow_net(const UmiPrudentialLiquidityCashflow *flow) { return flow==NULL?0.0:flow->inflow-flow->outflow; }
