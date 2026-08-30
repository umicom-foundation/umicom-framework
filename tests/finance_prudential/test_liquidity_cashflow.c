/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_liquidity_cashflow.c
 *
 * PURPOSE:
 *   Verify represent a dated prudential liquidity inflow or outflow.
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

int main(void) { UmiPrudentialLiquidityCashflow f; UmiCurrency c={"GBP"}; if(umi_pru_liquidity_cashflow_init(&f,1,80.0,100.0,c)!=UMI_STATUS_OK)return 1; return umi_pru_liquidity_cashflow_net(&f)==-20.0?0:2; }
