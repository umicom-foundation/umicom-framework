/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/liquidity_ladder.c
 *
 * PURPOSE:
 *   Implement treasury liquidity ladder aggregation and worst-gap evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/open_tms/liquidity_ladder.h"
#include <string.h>

#include "operational_internal.h"

/* Retained for owner review: these implementations trusted public counts,
 * inline strings and unchecked arithmetic. The active Framework boundary below
 * validates complete owners before traversal and commits results only after
 * finite calculations succeed. Existing entry points remain available. */
#if 0
void umi_open_tms_liquidity_ladder_init(UmiOpenTmsLiquidityLadder *ladder)
{
    if (ladder == NULL) return;
    (void)memset(ladder, 0, sizeof(*ladder));
    ladder->revision = 1U;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_open_tms_liquidity_ladder_add(
    UmiOpenTmsLiquidityLadder *ladder,
    const UmiOpenTmsLiquidityBucket *bucket)
{
    if (ladder == NULL || bucket == NULL || bucket->id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (ladder->count >= UMI_OPEN_TMS_LIQUIDITY_BUCKET_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (ladder->count > 0U &&
        bucket->endMilliseconds <= ladder->buckets[ladder->count - 1U].endMilliseconds)
        return UMI_STATUS_INVALID_STATE;
    ladder->buckets[ladder->count++] = *bucket;
    ladder->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_open_tms_liquidity_ladder_add(
    UmiOpenTmsLiquidityLadder *ladder,
    const UmiOpenTmsLiquidityBucket *bucket)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (ladder != NULL && ladder->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    if (ladder == NULL || bucket == NULL || bucket->id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (ladder->count >= UMI_OPEN_TMS_LIQUIDITY_BUCKET_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (ladder->count > 0U &&
        bucket->endMilliseconds <= ladder->buckets[ladder->count - 1U].endMilliseconds)
        return UMI_STATUS_INVALID_STATE;
    ladder->buckets[ladder->count++] = *bucket;
    ladder->revision += 1U;
    return UMI_STATUS_OK;
}

double umi_open_tms_liquidity_ladder_gap(const UmiOpenTmsLiquidityLadder *ladder)
{
    size_t i;
    double total = 0.0;
    if (ladder == NULL) return 0.0;
    for (i = 0U; i < ladder->count; ++i)
        total += ladder->buckets[i].inflows +
                 ladder->buckets[i].buffer -
                 ladder->buckets[i].outflows;
    return total;
}

double umi_open_tms_liquidity_ladder_worst_gap(const UmiOpenTmsLiquidityLadder *ladder)
{
    size_t i;
    double cumulative = 0.0;
    double worst = 0.0;
    if (ladder == NULL) return 0.0;
    for (i = 0U; i < ladder->count; ++i) {
        cumulative += ladder->buckets[i].inflows +
                      ladder->buckets[i].buffer -
                      ladder->buckets[i].outflows;
        if (cumulative < worst) worst = cumulative;
    }
    return worst;
}

#endif

UmiStatus umi_open_tms_liquidity_bucket_validate(const UmiOpenTmsLiquidityBucket *bucket)
{
    static const UmiSnapshotTextField fields[] = {
        {"id", offsetof(UmiOpenTmsLiquidityBucket, id), 48U, 1}
    };
    if (bucket == NULL || UmiSnapshotValidateTextFields(bucket, sizeof(*bucket),
            fields, 1U, NULL) != UMI_STATUS_OK || !isfinite(bucket->inflows) ||
        !isfinite(bucket->outflows) || !isfinite(bucket->buffer)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_liquidity_ladder_validate(const UmiOpenTmsLiquidityLadder *ladder)
{
    if (ladder == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (ladder->count > UMI_OPEN_TMS_LIQUIDITY_BUCKET_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (ladder->revision == 0U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < ladder->count; ++i) {
        if (umi_open_tms_liquidity_bucket_validate(&ladder->buckets[i]) != UMI_STATUS_OK ||
            (i != 0U && ladder->buckets[i].endMilliseconds <= ladder->buckets[i - 1U].endMilliseconds))
            return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
void umi_open_tms_liquidity_ladder_init(UmiOpenTmsLiquidityLadder *ladder)
{
    if (ladder == NULL) return;
    memset(ladder, 0, sizeof(*ladder));
    ladder->revision = 1U;
}
UmiStatus umi_open_tms_liquidity_ladder_add(UmiOpenTmsLiquidityLadder *ladder,
    const UmiOpenTmsLiquidityBucket *bucket)
{
    if (ladder != NULL && ladder->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_open_tms_liquidity_ladder_validate(ladder);
    if (status == UMI_STATUS_OK) status = umi_open_tms_liquidity_bucket_validate(bucket);
    if (status != UMI_STATUS_OK) return status;
    if (ladder->count == UMI_OPEN_TMS_LIQUIDITY_BUCKET_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (ladder->count > 0U && bucket->endMilliseconds <= ladder->buckets[ladder->count - 1U].endMilliseconds)
        return UMI_STATUS_INVALID_STATE;
    ladder->buckets[ladder->count++] = *bucket;
    ++ladder->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_liquidity_ladder_measure(const UmiOpenTmsLiquidityLadder *ladder,
    UmiOpenTmsLiquidityMeasures *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_liquidity_ladder_validate(ladder);
    if (status != UMI_STATUS_OK) return status;
    UmiOpenTmsLiquidityMeasures value = {0};
    for (size_t i = 0U; i < ladder->count; ++i) {
        const UmiOpenTmsLiquidityBucket *row = &ladder->buckets[i];
        double gap;
        status = TmsOpsAdd(row->inflows, row->buffer, &gap);
        if (status == UMI_STATUS_OK) status = TmsOpsAdd(gap, -row->outflows, &gap);
        if (status == UMI_STATUS_OK) status = TmsOpsAdd(value.gap, gap, &value.gap);
        if (status != UMI_STATUS_OK) return status;
        if (value.gap < value.worstGap) value.worstGap = value.gap;
    }
    *out = value;
    return UMI_STATUS_OK;
}
double umi_open_tms_liquidity_ladder_gap(const UmiOpenTmsLiquidityLadder *ladder)
{
    UmiOpenTmsLiquidityMeasures value;
    return umi_open_tms_liquidity_ladder_measure(ladder, &value) == UMI_STATUS_OK ? value.gap : 0.0;
}
double umi_open_tms_liquidity_ladder_worst_gap(const UmiOpenTmsLiquidityLadder *ladder)
{
    UmiOpenTmsLiquidityMeasures value;
    return umi_open_tms_liquidity_ladder_measure(ladder, &value) == UMI_STATUS_OK ? value.worstGap : 0.0;
}
