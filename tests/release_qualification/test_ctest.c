/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_ctest.c
 * Purpose: Keep JUnit import exact, atomic and separate from runner exit status.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/distribution/runtime/qualification_ctest.h"
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *name = argv[1];
    QualificationFixture f;
    REQUIRE(QualificationStart(&f, 2U, false) == 0);
    UmiReleaseQualificationResult passed = QualificationResult(UMI_QUALIFICATION_PASSED), process = passed;
    REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &passed) == UMI_STATUS_OK);
    const char *second = "<testcase name='test.000000' status='run' time='0.2'/>";
    const char *first = "<testcase name='test.000001' status='run' time='0.1'/>";
    unsigned count = 2U;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "unknown") == 0) {
        second = "<testcase name='wrong' status='run' time='0'/>";
        expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(name, "missing") == 0) {
        count = 1U; second = ""; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "duplicate") == 0) {
        second = first; expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "skipped") == 0) {
        second = "<testcase name='test.000000' status='notrun' time='0'><skipped message='SKIP_RETURN_CODE=77'/></testcase>";
    } else if (strcmp(name, "disabled") == 0) {
        second = "<testcase name='test.000000' status='disabled' time='0'/>";
    } else if (strcmp(name, "not-run") == 0) {
        second = "<testcase name='test.000000' status='notrun' time='0'><skipped message='Unable to find executable'/></testcase>";
        process = QualificationResult(UMI_QUALIFICATION_FAILED);
    } else if (strcmp(name, "failed") == 0) {
        second = "<testcase name='test.000000' status='fail' time='0'><failure message='Failed'/></testcase>";
        process = QualificationResult(UMI_QUALIFICATION_FAILED);
    } else if (strcmp(name, "timed-out") == 0) {
        second = "<testcase name='test.000000' status='fail' time='0'><failure message='Timeout'/></testcase>";
        process = QualificationResult(UMI_QUALIFICATION_FAILED);
    } else if (strcmp(name, "process-failed") == 0) process = QualificationResult(UMI_QUALIFICATION_FAILED);
    else if (strcmp(name, "existing") == 0) {
        REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "test.000000", &passed) == UMI_STATUS_OK);
        expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(name, "empty") == 0) {
        first = ""; second = ""; count = 0U; expected = UMI_STATUS_UNAVAILABLE;
    } else REQUIRE(strcmp(name, "passed") == 0 || strcmp(name, "malformed") == 0 || strcmp(name, "identity") == 0);
    char xml[2048];
    int written = snprintf(xml, sizeof(xml), "<testsuite tests='%u'>%s%s</testsuite>", count, first, second);
    REQUIRE(written > 0 && (size_t)written < sizeof(xml));
    size_t length = (size_t)written;
    UmiReleaseQualificationIdentity identity = f.identity;
    if (strcmp(name, "malformed") == 0) { --length; expected = UMI_STATUS_PARSE_ERROR; }
    if (strcmp(name, "identity") == 0) {
        identity.execution[0] = identity.execution[0] == '0' ? '1' : '0';
        expected = UMI_STATUS_INVALID_STATE;
    }
    REQUIRE(UmiReleaseQualificationImportCtest(f.run, &identity, xml, length, &process) == expected);
    UmiReleaseQualificationSummary summary;
    REQUIRE(UmiReleaseQualificationDescribe(f.run, &summary) == UMI_STATUS_OK);
    if (expected != UMI_STATUS_OK) {
        REQUIRE(summary.recorded == (strcmp(name, "existing") == 0 ? 1U : 0U));
        REQUIRE(!summary.testProcessRecorded);
    } else {
        REQUIRE(summary.recorded == 2U && summary.testProcessRecorded);
        UmiReleaseQualificationResult observed;
        REQUIRE(UmiReleaseQualificationGetTest(f.run, "test.000001", &observed) == UMI_STATUS_OK);
        REQUIRE(!observed.exitCodeKnown && observed.durationMs == 100U);
        char digest[65];
        REQUIRE(UmiSha256Buffer(xml, length, digest) == UMI_STATUS_OK);
        REQUIRE(strcmp(observed.logSha256, digest) == 0);
        REQUIRE(UmiReleaseQualificationGetTestProcess(f.run, &observed) == UMI_STATUS_OK);
        REQUIRE(observed.exitCodeKnown && observed.exitCode == process.exitCode);
        REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationDescribe(f.run, &summary) == UMI_STATUS_OK);
        REQUIRE(summary.readyForOwnerReview == (strcmp(name, "passed") == 0));
        if (strcmp(name, "disabled") == 0) REQUIRE(summary.disabled == 1U && summary.skipped == 0U);
        if (strcmp(name, "skipped") == 0) REQUIRE(summary.skipped == 1U && summary.disabled == 0U);
        if (strcmp(name, "not-run") == 0) REQUIRE(summary.notRun == 1U);
        if (strcmp(name, "failed") == 0) REQUIRE(summary.failed == 1U);
        if (strcmp(name, "timed-out") == 0) REQUIRE(summary.timedOut == 1U);
        if (strcmp(name, "process-failed") == 0)
            REQUIRE(summary.passed == 2U && summary.blockers == UMI_QUALIFICATION_TEST_PROCESS_NOT_PASSED);
    }
    QualificationDestroy(&f);
    return 0;
}
