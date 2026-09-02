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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_buffer.h"

/*
 * Provide the pru liquidity buffer calculate operation used by this module and its client
 * applications.
 */
UmiStatus umi_pru_liquidity_buffer_calculate(UmiPrudentialLiquidityBuffer *result,double available,double minimum_required) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL||available<0.0||minimum_required<0.0)return UMI_STATUS_INVALID_ARGUMENT; result->available=available; result->minimum_required=minimum_required; result->usable_amount=available>minimum_required?available-minimum_required:0.0; return UMI_STATUS_OK; }
