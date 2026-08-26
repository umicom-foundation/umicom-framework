/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/liquidity_buffer.h
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_BUFFER_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_BUFFER_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialLiquidityBuffer { double available; double minimum_required; double usable_amount; } UmiPrudentialLiquidityBuffer;
/* Calculate usable liquidity above a protected minimum buffer. */
UmiStatus umi_pru_liquidity_buffer_calculate(UmiPrudentialLiquidityBuffer *result, double available, double minimum_required);

#ifdef __cplusplus
}
#endif

#endif
