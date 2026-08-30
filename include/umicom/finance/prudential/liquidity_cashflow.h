/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/liquidity_cashflow.h
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_CASHFLOW_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_CASHFLOW_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialLiquidityCashflow { int32_t horizon_day; double inflow; double outflow; UmiCurrency currency; } UmiPrudentialLiquidityCashflow;
/* Initialise a non-negative liquidity cashflow for one horizon day. */
UmiStatus umi_pru_liquidity_cashflow_init(UmiPrudentialLiquidityCashflow *flow, int32_t horizon_day, double inflow, double outflow, UmiCurrency currency);
/* Return net liquidity movement for the cashflow. */
double umi_pru_liquidity_cashflow_net(const UmiPrudentialLiquidityCashflow *flow);

#ifdef __cplusplus
}
#endif

#endif
