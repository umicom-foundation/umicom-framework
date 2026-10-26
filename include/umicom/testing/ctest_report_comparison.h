/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_report_comparison.h
 * Purpose: Compare saved test outcomes without mistaking missing tests for repairs.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TESTING_CTEST_REPORT_COMPARISON_H
#define UMICOM_TESTING_CTEST_REPORT_COMPARISON_H
#include "umicom/testing/ctest_report.h"
#ifdef __cplusplus
extern "C" {
#endif

/** A comparison owns its changed-name records independently of both inputs. */
typedef struct UmiCtestReportComparison UmiCtestReportComparison;

/** Classification uses test names and outcomes only, never timings or messages. */
typedef enum UmiCtestReportChangeKind {
    UMI_CTEST_CHANGE_ADDED,
    UMI_CTEST_CHANGE_REMOVED,
    UMI_CTEST_CHANGE_RECOVERED,
    UMI_CTEST_CHANGE_REGRESSED,
    UMI_CTEST_CHANGE_OUTCOME
} UmiCtestReportChangeKind;

/** Missing sides use NOT_RUN with before_present/after_present false. A removed
 * failure is REMOVED, never RECOVERED. Disabled and runtime skipped differ. */
typedef struct UmiCtestReportChange {
    char name[UMI_TEST_NAME_CAPACITY];
    UmiCtestReportChangeKind kind;
    UmiTestState before, after;
    bool before_present, after_present, before_disabled, after_disabled;
} UmiCtestReportChange;

/** Counts describe two saved observations, not current source qualification.
 * changed = added + removed + recovered + regressed + outcome_changed.
 * before_tests = unchanged + recovered + regressed + outcome_changed + removed;
 * after_tests substitutes added for removed in the same identity. */
typedef struct UmiCtestReportComparisonSummary {
    size_t before_tests, after_tests, unchanged, changed;
    size_t added, removed, recovered, regressed, outcome_changed;
} UmiCtestReportComparisonSummary;

/** Match names byte-for-byte after report import's XML decoding. Input order,
 * duration and diagnostic wording do not affect outcome classification.
 * Runs no process and reads no files. Inputs are borrowed only for this call;
 * concurrent access must not destroy either input. On failure outComparison is
 * NULL. Empty inputs are valid comparisons, not successful qualification. */
UmiStatus UmiCtestReportCompare(const UmiCtestReport *before, const UmiCtestReport *after,
    UmiCtestReportComparison **outComparison);

/** Copy counts; invalid arguments leave outSummary unchanged. */
UmiStatus UmiCtestReportComparisonReadSummary(const UmiCtestReportComparison *comparison,
    UmiCtestReportComparisonSummary *outSummary);

/** Borrow a change in bytewise name order; NULL for an invalid index/owner.
 * The pointer remains valid until comparison destruction. */
const UmiCtestReportChange *UmiCtestReportComparisonAt(const UmiCtestReportComparison *comparison,
    size_t index);

/** Format a plain escaped page of outcome changes in exact-name order.
 * limit is 1..128; an empty comparison accepts offset zero. Query bytes with
 * NULL,0; required excludes NUL. Invalid arguments/offsets leave required
 * unchanged. A short buffer stays unchanged and returns its required length.
 * Inputs/output must not overlap. This reports observations, not qualification. */
UmiStatus UmiCtestReportComparisonFormat(const UmiCtestReportComparison *comparison,
    size_t offset, size_t limit, char *out, size_t capacity, size_t *required);

/** Release owned names and records. NULL is accepted. */
void UmiCtestReportComparisonDestroy(UmiCtestReportComparison *comparison);
#ifdef __cplusplus
}
#endif
#endif
