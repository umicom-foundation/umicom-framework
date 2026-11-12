/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_liquidity_admission.c
 *
 * PURPOSE:
 *   Verify complete liquidity admission and unchanged state on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "operational_fixture.h"

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    static UmiOpenTmsLiquidityLadder book, before;
    UmiOpenTmsLiquidityBucket row = LiquidityRow();
    UmiOpenTmsLiquidityMeasures result = {123, 456};
    umi_open_tms_liquidity_ladder_init(&book);
    if (strcmp(test, "empty") == 0) {
        REQUIRE(umi_open_tms_liquidity_ladder_measure(&book, &result) == UMI_STATUS_OK);
        REQUIRE(result.gap == 0 && result.worstGap == 0); return 0;
    }
    REQUIRE(umi_open_tms_liquidity_ladder_add(&book, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        row.endMilliseconds = 2000; row.inflows = 200;
        REQUIRE(umi_open_tms_liquidity_ladder_add(&book, &row) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_liquidity_ladder_measure(&book, &result) == UMI_STATUS_OK);
        REQUIRE(result.gap == 40 && result.worstGap == -30);
    } else if (strcmp(test, "text") == 0) {
        memset(row.id, 'x', sizeof row.id); memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_liquidity_ladder_add(&book, &row) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "numbers") == 0) {
        const size_t offsets[] = {offsetof(UmiOpenTmsLiquidityBucket, inflows), offsetof(UmiOpenTmsLiquidityBucket, outflows), offsetof(UmiOpenTmsLiquidityBucket, buffer)};
        const double bad[] = {NAN, INFINITY, -INFINITY};
        for (size_t i = 0; i < 3; ++i) for (size_t j = 0; j < 3; ++j) {
            row = LiquidityRow(); memcpy((char *)&row + offsets[i], &bad[j], sizeof(double));
            REQUIRE(umi_open_tms_liquidity_bucket_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if (strcmp(test, "count") == 0 || strcmp(test, "order") == 0) {
        if (strcmp(test, "count") == 0) book.count = SIZE_MAX;
        else { book.buckets[1] = row; book.count = 2; }
        memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_liquidity_ladder_add(&book, &row) != UMI_STATUS_OK);
        REQUIRE(umi_open_tms_liquidity_ladder_measure(&book, &result) != UMI_STATUS_OK);
        REQUIRE(result.gap == 123 && result.worstGap == 456);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "revision") == 0) {
        book.revision = 0; REQUIRE(umi_open_tms_liquidity_ladder_validate(&book) == UMI_STATUS_INVALID_STATE);
        book.revision = UINT64_MAX; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_liquidity_ladder_add(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_OPEN_TMS_LIQUIDITY_BUCKET_CAPACITY; ++i) {
            book.buckets[i] = row; book.buckets[i].endMilliseconds = (uint64_t)i;
        }
        book.count = UMI_OPEN_TMS_LIQUIDITY_BUCKET_CAPACITY; memcpy(&before, &book, sizeof book);
        REQUIRE(umi_open_tms_liquidity_ladder_validate(&book) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_liquidity_ladder_add(&book, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &book, sizeof book) == 0);
    } else if (strcmp(test, "overflow") == 0) {
        book.buckets[0].inflows = DBL_MAX; book.buckets[0].buffer = DBL_MAX;
        REQUIRE(umi_open_tms_liquidity_ladder_measure(&book, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(result.gap == 123 && result.worstGap == 456);
        REQUIRE(umi_open_tms_liquidity_ladder_gap(&book) == 0);
        book.buckets[0].buffer = 0; book.buckets[0].outflows = 0;
        book.buckets[1] = book.buckets[0]; book.buckets[1].endMilliseconds = 2000; book.count = 2;
        REQUIRE(umi_open_tms_liquidity_ladder_measure(&book, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(result.gap == 123 && result.worstGap == 456);
    } else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_open_tms_liquidity_ladder_measure(NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(result.gap == 123 && result.worstGap == 456);
        REQUIRE(umi_open_tms_liquidity_ladder_measure(&book, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
