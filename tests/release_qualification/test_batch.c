/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_batch.c
 * Purpose: Ensure batch preflight never publishes partial results or hides process failure.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *name = argv[1];
    QualificationFixture f;
    REQUIRE(QualificationStart(&f, 2U, false) == 0);
    UmiReleaseQualificationResult passed = QualificationResult(UMI_QUALIFICATION_PASSED);
    REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &passed) == UMI_STATUS_OK);
    UmiReleaseQualificationNamedResult rows[2] = {
        {"test.000001", passed}, {"test.000000", passed}};
    UmiReleaseQualificationResult process = passed;
    const UmiReleaseQualificationResult *processPointer = NULL;
    UmiStatus expected = UMI_STATUS_OK;
    size_t count = 2U;
    if (strcmp(name, "unknown-last") == 0) {
        rows[1].name = "unknown"; expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(name, "duplicate") == 0) {
        rows[1].name = rows[0].name; expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(name, "invalid-last") == 0) {
        rows[1].result.logSha256[0] = 'z'; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "existing") == 0) {
        REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, rows[1].name, &passed) == UMI_STATUS_OK);
        expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(name, "empty") == 0) count = 0U;
    else if (strcmp(name, "unknown-exit") == 0) {
        rows[1].result.exitCodeKnown = false; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "process") == 0 || strcmp(name, "process-failed") == 0) {
        processPointer = &process;
        rows[0].result.exitCodeKnown = false;
        rows[1].result.exitCodeKnown = false;
        if (strcmp(name, "process-failed") == 0) process = QualificationResult(UMI_QUALIFICATION_FAILED);
    } else if (strcmp(name, "partial-process") == 0) {
        count = 1U; processPointer = &process; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "unknown-process-exit") == 0) {
        processPointer = &process; process.exitCodeKnown = false; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "disabled") == 0) {
        rows[1].result.outcome = UMI_QUALIFICATION_DISABLED;
    } else REQUIRE(strcmp(name, "order") == 0);
    REQUIRE(UmiReleaseQualificationRecordTests(f.run, &f.identity, rows, count, processPointer) == expected);
    UmiReleaseQualificationSummary summary;
    REQUIRE(UmiReleaseQualificationDescribe(f.run, &summary) == UMI_STATUS_OK);
    if (expected != UMI_STATUS_OK) {
        REQUIRE(summary.recorded == (strcmp(name, "existing") == 0 ? 1U : 0U));
        REQUIRE(!summary.testProcessRecorded);
    } else if (count == 0U) REQUIRE(summary.recorded == 0U && summary.missing == 2U);
    else {
        REQUIRE(summary.recorded == 2U && summary.missing == 0U);
        REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationDescribe(f.run, &summary) == UMI_STATUS_OK);
        if (strcmp(name, "process-failed") == 0) {
            REQUIRE(summary.passed == 2U && !summary.readyForOwnerReview);
            REQUIRE(summary.blockers == UMI_QUALIFICATION_TEST_PROCESS_NOT_PASSED);
        } else if (strcmp(name, "disabled") == 0) {
            REQUIRE(summary.disabled == 1U && summary.skipped == 0U && !summary.readyForOwnerReview);
        } else REQUIRE(summary.readyForOwnerReview);
    }
    QualificationDestroy(&f);
    return 0;
}
