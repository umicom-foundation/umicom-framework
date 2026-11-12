/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/liquidity_ladder.h
 *
 * PURPOSE:
 *   Aggregate treasury liquidity inflows/outflows/buffers across bounded time buckets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_LIQUIDITY_LADDER_H
#define UMICOM_OPEN_TMS_LIQUIDITY_LADDER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OPEN_TMS_LIQUIDITY_BUCKET_CAPACITY 64U

typedef struct UmiOpenTmsLiquidityBucket {
    char id[48];
    uint64_t endMilliseconds;
    double inflows;
    double outflows;
    double buffer;
} UmiOpenTmsLiquidityBucket;

typedef struct UmiOpenTmsLiquidityLadder {
    UmiOpenTmsLiquidityBucket buckets[UMI_OPEN_TMS_LIQUIDITY_BUCKET_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiOpenTmsLiquidityLadder;

/* Public storage is validated before traversal. Inline IDs must be nonempty
 * and terminated, all numeric observations finite, bucket times increasing and
 * revision nonzero. Repeated descriptive IDs are allowed; times define order.
 * This currency-free preview requires the caller to supply consistent units. */
UmiStatus umi_open_tms_liquidity_bucket_validate(const UmiOpenTmsLiquidityBucket *bucket);
UmiStatus umi_open_tms_liquidity_ladder_validate(const UmiOpenTmsLiquidityLadder *ladder);
typedef struct UmiOpenTmsLiquidityMeasures { double gap; double worstGap; } UmiOpenTmsLiquidityMeasures;
/* Worst gap includes the initial zero and every cumulative bucket. A complete
 * finite result is published once; refusal leaves out unchanged. Keep inputs
 * stable, with output disjoint from the ladder. Scalar wrappers return zero on
 * refusal, which is not evidence of sufficient funding. */
UmiStatus umi_open_tms_liquidity_ladder_measure(const UmiOpenTmsLiquidityLadder *ladder, UmiOpenTmsLiquidityMeasures *out);

void umi_open_tms_liquidity_ladder_init(UmiOpenTmsLiquidityLadder *ladder);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_open_tms_liquidity_ladder_add(
    UmiOpenTmsLiquidityLadder *ladder,
    const UmiOpenTmsLiquidityBucket *bucket);
double umi_open_tms_liquidity_ladder_gap(
    const UmiOpenTmsLiquidityLadder *ladder);
double umi_open_tms_liquidity_ladder_worst_gap(
    const UmiOpenTmsLiquidityLadder *ladder);

#ifdef __cplusplus
}
#endif
#endif
