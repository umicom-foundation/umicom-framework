/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_assessment.c
 * Purpose: Require complete passing outcomes and matching observations before review.
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
    REQUIRE(QualificationStart(&f, strcmp(name, "no-tests") == 0 ? 0U : 1U, false) == 0);
    UmiReleaseQualificationSummary summary;
    REQUIRE(UmiReleaseQualificationDescribe(f.run, &summary) == UMI_STATUS_OK);
    REQUIRE(!summary.readyForOwnerReview && !summary.closed && !summary.buildRecorded);
    uint32_t expected = 0U;
    bool missingBuild = strcmp(name, "missing-build") == 0;
    bool missingTest = strcmp(name, "missing-test") == 0 || missingBuild;
    if (!missingBuild) {
        UmiReleaseQualificationResult result = QualificationResult(UMI_QUALIFICATION_PASSED);
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &result) == UMI_STATUS_OK);
        if (!missingTest && strcmp(name, "no-tests") != 0) {
            if (strcmp(name, "failed") == 0) result = QualificationResult(UMI_QUALIFICATION_FAILED);
            else if (strcmp(name, "skipped") == 0) result = QualificationResult(UMI_QUALIFICATION_SKIPPED);
            else if (strcmp(name, "not-run") == 0) result = QualificationResult(UMI_QUALIFICATION_NOT_RUN);
            else if (strcmp(name, "cancelled") == 0) result = QualificationResult(UMI_QUALIFICATION_CANCELLED);
            else if (strcmp(name, "timed-out") == 0) result = QualificationResult(UMI_QUALIFICATION_TIMED_OUT);
            REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "test.000000", &result) == UMI_STATUS_OK);
            if (result.outcome != UMI_QUALIFICATION_PASSED) expected |= UMI_QUALIFICATION_TESTS_NOT_PASSED;
        }
    }
    if (missingBuild) expected |= UMI_QUALIFICATION_BUILD_MISSING;
    if (missingTest) expected |= UMI_QUALIFICATION_TESTS_MISSING;
    if (strcmp(name, "no-tests") == 0) expected |= UMI_QUALIFICATION_NO_TEST_COVERAGE;
    const UmiReleaseSourceObservation *closeSource = f.before;
    UmiReleaseSourceObservation *other = NULL;
    UmiReleaseInventory *otherInventory = NULL;
    UmiReleaseSourceOptions options = {.read = QualificationRead,
        .maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT, .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT};
    if (strcmp(name, "source-changed") == 0) {
        options.context = "after!";
        REQUIRE(UmiReleaseSourceObserve(f.inventory, &options, &other) == UMI_STATUS_OK);
        closeSource = other; expected |= UMI_QUALIFICATION_SOURCE_CHANGED;
    } else if (strcmp(name, "context-changed") == 0) {
        char *generation = strstr(f.text.text, "0123456789abcdef0123456789abcdef");
        REQUIRE(generation != NULL); generation[0] = 'a';
        REQUIRE(FixtureParse(&f.text, &otherInventory) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseSourceObserve(otherInventory, &options, &other) == UMI_STATUS_OK);
        closeSource = other; expected |= UMI_QUALIFICATION_CONTEXT_CHANGED;
    } else if (strcmp(name, "unobserved-source") == 0) {
        closeSource = NULL; expected |= UMI_QUALIFICATION_SOURCE_UNOBSERVED;
    }
    char changedToolchain[65];
    REQUIRE(UmiSha256Buffer("changed", 7U, changedToolchain) == UMI_STATUS_OK);
    const char *toolchain = f.toolchain;
    if (strcmp(name, "toolchain-changed") == 0) {
        toolchain = changedToolchain; expected |= UMI_QUALIFICATION_TOOLCHAIN_CHANGED;
    } else if (strcmp(name, "unobserved-toolchain") == 0) {
        toolchain = NULL; expected |= UMI_QUALIFICATION_TOOLCHAIN_UNOBSERVED;
    }
    if (strcmp(name, "invalid-close") == 0) {
        REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, closeSource, "invalid") == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiReleaseQualificationDescribe(f.run, &summary) == UMI_STATUS_OK && !summary.closed);
    }
    REQUIRE(UmiReleaseQualificationDescribe(f.run, &summary) == UMI_STATUS_OK && !summary.readyForOwnerReview);
    REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, closeSource, toolchain) == UMI_STATUS_OK);
    REQUIRE(UmiReleaseQualificationDescribe(f.run, &summary) == UMI_STATUS_OK);
    REQUIRE(summary.closed && summary.blockers == expected);
    REQUIRE(summary.readyForOwnerReview == (expected == 0U));
    REQUIRE(summary.recorded + summary.missing == summary.tests);
    REQUIRE(summary.passed + summary.failed + summary.skipped + summary.notRun + summary.cancelled + summary.timedOut + summary.disabled == summary.recorded);
    if (strcmp(name, "skipped") == 0) REQUIRE(summary.skipped == 1U && summary.passed == 0U);
    if (strcmp(name, "not-run") == 0) REQUIRE(summary.notRun == 1U && summary.missing == 0U);
    if (strcmp(name, "failed") == 0) REQUIRE(summary.failed == 1U);
    if (strcmp(name, "cancelled") == 0) REQUIRE(summary.cancelled == 1U);
    if (strcmp(name, "timed-out") == 0) REQUIRE(summary.timedOut == 1U);
    if (strcmp(name, "sealed") == 0) {
        UmiReleaseQualificationResult result = QualificationResult(UMI_QUALIFICATION_FAILED);
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &result) == UMI_STATUS_INVALID_STATE);
        REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "test.000000", &result) == UMI_STATUS_INVALID_STATE);
        REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, NULL, NULL) == UMI_STATUS_INVALID_STATE);
    }
    UmiReleaseSourceObservationDestroy(other);
    UmiReleaseInventoryDestroy(otherInventory);
    QualificationDestroy(&f);
    return 0;
}
