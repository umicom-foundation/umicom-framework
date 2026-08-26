/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_liquidity_headroom.c
 *
 * PURPOSE:
 *   Verify calculate liquidity headroom over a minimum required buffer.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_headroom.h"

int main(void) { UmiPrudentialLiquidityHeadroom r; if(umi_pru_liquidity_headroom_calculate(&r,120.0,100.0)!=UMI_STATUS_OK)return 1; if(r.headroom!=20.0||r.breached!=0)return 2; if(umi_pru_liquidity_headroom_calculate(&r,80.0,100.0)!=UMI_STATUS_OK||r.breached!=1)return 3; return 0; }
