/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/liquidity_headroom.h
 *
 * PURPOSE:
 *   Calculate liquidity headroom over a minimum required buffer.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_HEADROOM_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_HEADROOM_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential liquidity headroom data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialLiquidityHeadroom { double available; double required; double headroom; int breached; } UmiPrudentialLiquidityHeadroom;
/* Calculate prudential headroom and identify a deficit without hiding negative values. */
UmiStatus umi_pru_liquidity_headroom_calculate(UmiPrudentialLiquidityHeadroom *result, double available, double required);

#ifdef __cplusplus
}
#endif

#endif
