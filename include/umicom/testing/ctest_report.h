/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_report.h
 * PURPOSE: Inspect an owned, complete saved CTest JUnit report.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TESTING_CTEST_REPORT_H
#define UMICOM_TESTING_CTEST_REPORT_H

#include <stdbool.h>
#include "umicom/testing/ctest_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CTEST_COLLECTION_MAX_BYTES (64U * 1024U * 1024U)
#define UMI_CTEST_COLLECTION_MAX_TESTS 100000U

/** An immutable report owns all imported names and result records. It contains
 * saved observations, not proof of a current run, build revision or process
 * exit. The caller must keep the report alive while reading borrowed records.
 */
typedef struct UmiCtestReport UmiCtestReport;

/** Mutually exclusive outcome counts. Disabled tests are separate from runtime
 * skips. failed excludes timed_out. measured_test_ms sums testcase times; it
 * is not elapsed wall time because tests may have executed concurrently.
 */
typedef struct UmiCtestReportSummary {
    size_t tests;
    size_t passed;
    size_t failed;
    size_t timed_out;
    size_t skipped;
    size_t disabled;
    size_t not_run;
    uint64_t measured_test_ms;
} UmiCtestReportSummary;

/** One copied record in document order. disabled distinguishes CTest's disabled
 * state from an executed test returning a configured skip status. The legacy
 * snapshot still represents both as SKIPPED for compatibility.
 */
typedef struct UmiCtestReportEntry {
    char name[UMI_TEST_NAME_CAPACITY];
    UmiCtestReportSnapshot result;
    bool disabled;
} UmiCtestReportEntry;

/** Import CTest's bounded JUnit subset through the same parser as selected-test
 * execution. Reject incomplete XML, duplicate decoded names, mismatched suite
 * counts, unknown outcomes, invalid UTF-8, external entities and capacity
 * exhaustion. Optional failures/disabled/skipped totals must match testcase
 * records; CTest's skipped total includes runtime skips and not-run records.
 * Names must fit UMI_TEST_NAME_CAPACITY. Decoded XML attributes are limited to
 * 1023 bytes and outcome messages to 255 bytes, matching the shared grammar.
 * Existing single-test APIs retain their
 * own smaller byte limit and compatibility rules. No file or process is read.
 * outReport is set to NULL on failure; release an earlier report separately.
 * Input storage must not overlap the output pointer. Empty reports may import
 * successfully, but cannot satisfy RequirePassed.
 */
UmiStatus UmiCtestReportImport(const char *xml, size_t length,
    UmiCtestReport **outReport);

/** Copy summary counts. Failure leaves the caller's output unchanged. */
UmiStatus UmiCtestReportReadSummary(const UmiCtestReport *report,
    UmiCtestReportSummary *outSummary);

/** Borrow a record by zero-based document index; NULL for invalid arguments. */
const UmiCtestReportEntry *UmiCtestReportEntryAt(const UmiCtestReport *report,
    size_t index);

/** Accept a nonempty report containing only passed testcase records. This is
 * a saved-result check, not execution or freshness verification.
 */
UmiStatus UmiCtestReportRequirePassed(const UmiCtestReport *report);

/** Release report-owned records. NULL is accepted; borrowed pointers expire. */
void UmiCtestReportDestroy(UmiCtestReport *report);

#ifdef __cplusplus
}
#endif
#endif
