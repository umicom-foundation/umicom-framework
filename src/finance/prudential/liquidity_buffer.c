/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/liquidity_buffer.c
 *
 * PURPOSE:
 *   Represent a minimum liquidity buffer and available stock.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_buffer.h"

UmiStatus umi_pru_liquidity_buffer_calculate(UmiPrudentialLiquidityBuffer *result,double available,double minimum_required) { if(result==NULL||available<0.0||minimum_required<0.0)return UMI_STATUS_INVALID_ARGUMENT; result->available=available; result->minimum_required=minimum_required; result->usable_amount=available>minimum_required?available-minimum_required:0.0; return UMI_STATUS_OK; }
