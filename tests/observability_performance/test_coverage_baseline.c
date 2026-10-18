/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_coverage_baseline.c
 *
 * PURPOSE:
 *   Implement the test coverage baseline behavior for
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
#include "../value_archive/construction_cases.h"
#include "umicom/observability/performance/coverage_baseline.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/observability/performance/coverage_baseline.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPerformanceCoverageBaselineTransferEqual(const UmiPerformanceCoverageBaseline *a, const UmiPerformanceCoverageBaseline *b)
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
static void UmiPerformanceCoverageBaselineTransferTails(UmiPerformanceCoverageBaseline *value)
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
static int UmiPerformanceCoverageBaselineTransferMalformed(const UmiPerformanceCoverageBaseline *sample)
{
    (void)sample;
    {
        UmiPerformanceCoverageBaseline invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_performance_coverage_baseline_validate(&invalid) != UMI_STATUS_OK) ||
            umi_performance_coverage_baseline_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPerformanceCoverageBaseline invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject_id, 'x', sizeof(invalid.subject_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_performance_coverage_baseline_validate(&invalid) != UMI_STATUS_OK) ||
            umi_performance_coverage_baseline_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPerformanceCoverageBaselineTransferCases, UmiPerformanceCoverageBaseline,
    umi_performance_coverage_baseline_archive_encode, umi_performance_coverage_baseline_archive_decode,
    UmiPerformanceCoverageBaselineTransferEqual, UmiPerformanceCoverageBaselineTransferTails, UmiPerformanceCoverageBaselineTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiPerformanceCoverageBaselineTransferCasesWithWorkspace, UmiPerformanceCoverageBaseline,
    umi_performance_coverage_baseline_archive_encode, umi_performance_coverage_baseline_archive_decode_with_workspace, UmiPerformanceCoverageBaselineTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiPerformanceCoverageBaselineConditionalArchiveCases, UmiPerformanceCoverageBaseline,
    umi_performance_coverage_baseline_archive_encode, umi_performance_coverage_baseline_archive_decode_if_unchanged,
    UmiPerformanceCoverageBaselineTransferEqual, UmiPerformanceCoverageBaselineTransferTails, UmiPerformanceCoverageBaselineTransferMalformed)



/* A rejected replacement must preserve the previously accepted coverage baseline value. */
static int UmiPerformanceCoverageBaselineConstructionCases(void)
{
    UmiPerformanceCoverageBaseline *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_performance_coverage_baseline_init(value, "coverage_baseline", "framework") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_performance_coverage_baseline_init(value, oversized, "framework") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_performance_coverage_baseline_init(value, value->id, "framework") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "coverage_baseline") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->subject_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_performance_coverage_baseline_init(value, "coverage_baseline", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_performance_coverage_baseline_init(value, "coverage_baseline", value->subject_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->subject_id, "framework") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiPerformanceCoverageBaselineConstructionCases() != 0) return 1;

    UmiPerformanceCoverageBaseline left;
    UmiPerformanceCoverageBaseline right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_coverage_baseline_init(&left, "coverage_baseline", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_coverage_baseline_validate(&left) != UMI_STATUS_OK) return 2;
    if (UmiPerformanceCoverageBaselineTransferCases(&left) != 0) return 1;
    if (UmiPerformanceCoverageBaselineTransferCasesWithWorkspace(&left) != 0) return 1;
    if (UmiPerformanceCoverageBaselineConditionalArchiveCases(&left) != 0) return 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_coverage_baseline_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_coverage_baseline_coverage_percent(80U, 100U) != 80.0) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_coverage_baseline_init(&right, "coverage_baseline", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_coverage_baseline_same_identity(&left, &right)) return 6;
    puts("coverage_baseline: ok");
    return 0;
}
