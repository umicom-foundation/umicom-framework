/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/liquidity_ladder.h
 *
 * PURPOSE:
 *   Build a bounded ordered liquidity ladder and cumulative balance.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_LADDER_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_LADDER_H

#include "umicom/finance/prudential/types.h"
#include "umicom/finance/prudential/liquidity_bucket.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialLiquidityLadder { UmiPrudentialLiquidityBucket buckets[UMI_PRU_MAX_PERIODS]; size_t count; } UmiPrudentialLiquidityLadder;
/* Append a horizon bucket while enforcing strictly increasing day order. */
UmiStatus umi_pru_liquidity_ladder_add(UmiPrudentialLiquidityLadder *ladder, const UmiPrudentialLiquidityBucket *bucket);
/* Return the last closing-liquidity amount, or zero for an empty ladder. */
double umi_pru_liquidity_ladder_closing(const UmiPrudentialLiquidityLadder *ladder);

#ifdef __cplusplus
}
#endif

#endif
