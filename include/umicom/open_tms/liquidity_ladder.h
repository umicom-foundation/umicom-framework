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

void umi_open_tms_liquidity_ladder_init(UmiOpenTmsLiquidityLadder *ladder);
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
