/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_metric_series.c
 *
 * PURPOSE:
 *   Implement the test metric series behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <stdio.h>
#include "umicom/observability/performance/metric_series.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/observability/performance/metric_series.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPerformanceMetricSeriesTransferEqual(const UmiPerformanceMetricSeries *a, const UmiPerformanceMetricSeries *b)
{
    return a->structure_size == b->structure_size &&
        a->api_version == b->api_version &&
        strcmp(a->id, b->id) == 0 &&
        strcmp(a->subject_id, b->subject_id) == 0 &&
        a->state == b->state &&
        a->severity == b->severity &&
        a->sequence == b->sequence &&
        a->timestamp_ns == b->timestamp_ns &&
        a->value == b->value &&
        a->auxiliary == b->auxiliary &&
        a->count == b->count &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPerformanceMetricSeriesTransferTails(UmiPerformanceMetricSeries *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->subject_id) + 1U;
        memset(value->subject_id + used, 0xa5, sizeof(value->subject_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPerformanceMetricSeriesTransferMalformed(const UmiPerformanceMetricSeries *sample)
{
    (void)sample;
    {
        UmiPerformanceMetricSeries invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_performance_metric_series_validate(&invalid) != UMI_STATUS_OK) ||
            umi_performance_metric_series_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPerformanceMetricSeries invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject_id, 'x', sizeof(invalid.subject_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_performance_metric_series_validate(&invalid) != UMI_STATUS_OK) ||
            umi_performance_metric_series_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPerformanceMetricSeriesTransferCases, UmiPerformanceMetricSeries,
    umi_performance_metric_series_archive_encode, umi_performance_metric_series_archive_decode,
    UmiPerformanceMetricSeriesTransferEqual, UmiPerformanceMetricSeriesTransferTails, UmiPerformanceMetricSeriesTransferMalformed)

int main(void) {
    UmiPerformanceMetricSeries left;
    UmiPerformanceMetricSeries right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_metric_series_init(&left, "metric_series", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_metric_series_validate(&left) != UMI_STATUS_OK) return 2;
    if (UmiPerformanceMetricSeriesTransferCases(&left) != 0) return 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_metric_series_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_performance_metric_series_capacity_available(1U, 2U)) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_metric_series_init(&right, "metric_series", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_metric_series_same_identity(&left, &right)) return 6;
    puts("metric_series: ok");
    return 0;
}
