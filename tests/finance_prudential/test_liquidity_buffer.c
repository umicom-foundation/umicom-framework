/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_liquidity_buffer.c
 *
 * PURPOSE:
 *   Verify represent a minimum liquidity buffer and available stock.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_buffer.h"

int main(void) { UmiPrudentialLiquidityBuffer r; if(umi_pru_liquidity_buffer_calculate(&r,100.0,40.0)!=UMI_STATUS_OK)return 1; return r.usable_amount==60.0?0:2; }
