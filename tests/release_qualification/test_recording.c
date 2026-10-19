/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_recording.c
 * Purpose: Preserve original failures and reject incomplete or inconsistent result attribution.
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
    UmiReleaseQualificationResult result = QualificationResult(UMI_QUALIFICATION_PASSED), out = result;
    out.durationMs = 999U;
    REQUIRE(UmiReleaseQualificationGetBuild(f.run, &out) == UMI_STATUS_NOT_FOUND && out.durationMs == 999U);
    REQUIRE(UmiReleaseQualificationGetTest(f.run, "test.000000", &out) == UMI_STATUS_NOT_FOUND && out.durationMs == 999U);
    if (strcmp(name, "build-first") == 0) {
        REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "test.000000", &result) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "invalid-result") == 0) {
        result.exitCode = 1;
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &result) == UMI_STATUS_INVALID_ARGUMENT);
        result.exitCode = 0; result.status = UMI_STATUS_IO_ERROR;
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &result) == UMI_STATUS_INVALID_ARGUMENT);
        result.status = UMI_STATUS_OK; result.logSha256[0] = 'G';
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &result) == UMI_STATUS_INVALID_ARGUMENT);
        result = QualificationResult((UmiReleaseQualificationOutcome)99);
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &result) == UMI_STATUS_INVALID_ARGUMENT);
    } else {
        bool buildFailed = strcmp(name, "failed-build") == 0;
        result = QualificationResult(buildFailed ? UMI_QUALIFICATION_FAILED : UMI_QUALIFICATION_PASSED);
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &result) == UMI_STATUS_OK);
        if (strcmp(name, "duplicate-build") == 0) {
            REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &result) == UMI_STATUS_ALREADY_EXISTS);
        } else if (strcmp(name, "unknown-test") == 0) {
            REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "Test.000000", &result) == UMI_STATUS_NOT_FOUND);
            REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "", &result) == UMI_STATUS_NOT_FOUND);
        } else if (strcmp(name, "duplicate-test") == 0) {
            result = QualificationResult(UMI_QUALIFICATION_FAILED);
            REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "test.000000", &result) == UMI_STATUS_OK);
            result = QualificationResult(UMI_QUALIFICATION_PASSED);
            REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "test.000000", &result) == UMI_STATUS_ALREADY_EXISTS);
            REQUIRE(UmiReleaseQualificationGetTest(f.run, "test.000000", &out) == UMI_STATUS_OK && out.outcome == UMI_QUALIFICATION_FAILED);
        } else if (strcmp(name, "copy") == 0) {
            REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "test.000000", &result) == UMI_STATUS_OK);
            memset(&result, 0, sizeof(result));
            REQUIRE(UmiReleaseQualificationGetTest(f.run, "test.000000", &out) == UMI_STATUS_OK);
            REQUIRE(out.durationMs == 123U && out.outcome == UMI_QUALIFICATION_PASSED);
        } else if (buildFailed) {
            result = QualificationResult(UMI_QUALIFICATION_PASSED);
            REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "test.000000", &result) == UMI_STATUS_OK);
            REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "test.000001", &result) == UMI_STATUS_OK);
            REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
            UmiReleaseQualificationSummary summary;
            REQUIRE(UmiReleaseQualificationDescribe(f.run, &summary) == UMI_STATUS_OK);
            REQUIRE(summary.passed == 2U && !summary.readyForOwnerReview);
            REQUIRE(summary.blockers == UMI_QUALIFICATION_BUILD_NOT_PASSED);
        } else REQUIRE(false);
    }
    QualificationDestroy(&f);
    return 0;
}
