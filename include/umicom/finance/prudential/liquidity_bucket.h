/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/liquidity_bucket.h
 *
 * PURPOSE:
 *   Aggregate inflows, outflows and closing liquidity for one horizon bucket.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_BUCKET_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_BUCKET_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialLiquidityBucket { int32_t horizon_day; double opening_liquidity; double inflows; double outflows; double closing_liquidity; } UmiPrudentialLiquidityBucket;
/* Calculate one liquidity horizon bucket from opening stock and flows. */
UmiStatus umi_pru_liquidity_bucket_calculate(UmiPrudentialLiquidityBucket *bucket, int32_t horizon_day, double opening_liquidity, double inflows, double outflows);

#ifdef __cplusplus
}
#endif

#endif
