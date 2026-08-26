/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/liquidity_headroom.c
 *
 * PURPOSE:
 *   Calculate liquidity headroom over a minimum required buffer.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_headroom.h"

UmiStatus umi_pru_liquidity_headroom_calculate(UmiPrudentialLiquidityHeadroom *result,double available,double required) { if(result==NULL||!umi_pru_number_valid(available)||!umi_pru_number_valid(required)||available<0.0||required<0.0)return UMI_STATUS_INVALID_ARGUMENT; result->available=available; result->required=required; result->headroom=available-required; result->breached=result->headroom<0.0?1:0; return UMI_STATUS_OK; }
