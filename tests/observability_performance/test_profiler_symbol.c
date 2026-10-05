/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_profiler_symbol.c
 *
 * PURPOSE:
 *   Implement the test profiler symbol behavior for
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
#include "umicom/observability/performance/profiler_symbol.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/observability/performance/profiler_symbol.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPerformanceProfilerSymbolTransferEqual(const UmiPerformanceProfilerSymbol *a, const UmiPerformanceProfilerSymbol *b)
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
static void UmiPerformanceProfilerSymbolTransferTails(UmiPerformanceProfilerSymbol *value)
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
static int UmiPerformanceProfilerSymbolTransferMalformed(const UmiPerformanceProfilerSymbol *sample)
{
    (void)sample;
    {
        UmiPerformanceProfilerSymbol invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_performance_profiler_symbol_validate(&invalid) != UMI_STATUS_OK) ||
            umi_performance_profiler_symbol_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPerformanceProfilerSymbol invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject_id, 'x', sizeof(invalid.subject_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_performance_profiler_symbol_validate(&invalid) != UMI_STATUS_OK) ||
            umi_performance_profiler_symbol_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPerformanceProfilerSymbolTransferCases, UmiPerformanceProfilerSymbol,
    umi_performance_profiler_symbol_archive_encode, umi_performance_profiler_symbol_archive_decode,
    UmiPerformanceProfilerSymbolTransferEqual, UmiPerformanceProfilerSymbolTransferTails, UmiPerformanceProfilerSymbolTransferMalformed)

int main(void) {
    UmiPerformanceProfilerSymbol left;
    UmiPerformanceProfilerSymbol right;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_profiler_symbol_init(&left, "profiler_symbol", "framework") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_profiler_symbol_validate(&left) != UMI_STATUS_OK) return 2;
    if (UmiPerformanceProfilerSymbolTransferCases(&left) != 0) return 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_profiler_symbol_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_profiler_symbol_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_profiler_symbol_init(&right, "profiler_symbol", "framework") != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_profiler_symbol_same_identity(&left, &right)) return 6;
    puts("profiler_symbol: ok");
    return 0;
}
