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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_cashflow.h"

/*
 * Initialise pru liquidity cashflow from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_pru_liquidity_cashflow_init(UmiPrudentialLiquidityCashflow *flow,int32_t day,double inflow,double outflow,UmiCurrency currency) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(flow==NULL||day<0||inflow<0.0||outflow<0.0)return UMI_STATUS_INVALID_ARGUMENT; flow->horizon_day=day; flow->inflow=inflow; flow->outflow=outflow; flow->currency=currency; return UMI_STATUS_OK; }
/*
 * Provide the pru liquidity cashflow net operation used by this module and its client
 * applications.
 */
double umi_pru_liquidity_cashflow_net(const UmiPrudentialLiquidityCashflow *flow) { return flow==NULL?0.0:flow->inflow-flow->outflow; }
