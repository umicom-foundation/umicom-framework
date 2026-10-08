/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/archive_compare.h
 * PURPOSE: Compare recorded test outcomes without executing or replaying tests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_ARCHIVE_COMPARE_H
#define UMICOM_TESTING_ARCHIVE_COMPARE_H
#include "umicom/testing/archive.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiTestArchiveChange
    {
        UMI_TEST_ARCHIVE_UNCHANGED = 0,
        UMI_TEST_ARCHIVE_REGRESSION,
        UMI_TEST_ARCHIVE_RECOVERED,
        UMI_TEST_ARCHIVE_PERSISTING_FAILURE,
        UMI_TEST_ARCHIVE_ADDED,
        UMI_TEST_ARCHIVE_REMOVED,
        UMI_TEST_ARCHIVE_CONDITIONS_CHANGED,
        UMI_TEST_ARCHIVE_INCONCLUSIVE,
        UMI_TEST_ARCHIVE_OUTCOME_CHANGED,
        UMI_TEST_ARCHIVE_CHANGE_COUNT
    } UmiTestArchiveChange;
    typedef struct UmiTestArchiveOutcomes
    {
        size_t planned, recorded, passed, failed, skipped, cancelled, timed_out, not_run;
        size_t never_started, errors;
        uint64_t duration_ms;
    } UmiTestArchiveOutcomes;
    typedef struct UmiTestArchiveComparisonRow
    {
        UmiTestArchiveChange change;
        bool before_present, after_present;
        UmiCtestJobRequest before_request, after_request;
        UmiTestArchiveOutcomes before, after;
    } UmiTestArchiveComparisonRow;
    typedef struct UmiTestArchiveComparisonSummary
    {
        UmiTestArchiveEntry baseline, candidate;
        size_t row_count;
        size_t counts[UMI_TEST_ARCHIVE_CHANGE_COUNT];
        bool same_source_root;
        /* Descriptions are caller-supplied labels, not verified source identities.
     * Empty revision labels never count as equal recorded revisions. */
        bool same_recorded_revision;
    } UmiTestArchiveComparisonSummary;
    typedef struct UmiTestArchiveComparison UmiTestArchiveComparison;
    /* Read both immutable runs under one Data Server transaction. The result owns
 * its copied requests and outcome counts; it does not borrow the archive.
 * Source/build paths, configuration and names are compared byte-for-byte.
 * Matching uses build directory + configuration + test name, not transient IDs.
 * Duplicate matching keys are ambiguous and return PARSE_ERROR. No test runs.
 *
 * A regression requires a previously complete, all-passed test and a current
 * failed/timed-out attempt or reporting error. Recovery requires the reverse.
 * Changed source roots, repeat counts, early-stop policy, timeouts or enabled
 * flags are reported as CONDITIONS_CHANGED instead of claiming a regression.
 * Cancelled, not-run, never-started and skipped evidence never becomes a pass.
 * Duration differences alone do not change the outcome category.
 *
 * Up to two selections of 4096 attempts are read. Storage reads may be slow;
 * interactive callers should run this operation outside their event loop.
 * Cancellation and read/commit errors publish no partial comparison. Errors
 * leave *out_comparison unchanged; it must not alias archive storage.
 * Diagnostic output is validated but is not copied into the comparison. */
    UmiStatus UmiTestArchiveCompare(UmiTestArchive *archive, uint64_t baseline_id, uint64_t candidate_id,
                                    const UmiCancellationToken *cancellation,
                                    UmiTestArchiveComparison **out_comparison);
    void UmiTestArchiveComparisonDestroy(UmiTestArchiveComparison *comparison);
    /* These methods copy memory only and remain usable after the database closes.
 * Keep the comparison alive during calls; errors preserve output objects. */
    UmiStatus UmiTestArchiveComparisonRead(const UmiTestArchiveComparison *comparison,
                                           UmiTestArchiveComparisonSummary *out_summary);
    UmiStatus UmiTestArchiveComparisonRowAt(const UmiTestArchiveComparison *comparison, size_t index,
                                            UmiTestArchiveComparisonRow *out_row);
    const char *UmiTestArchiveChangeText(UmiTestArchiveChange change);
#ifdef __cplusplus
}
#endif
#endif
