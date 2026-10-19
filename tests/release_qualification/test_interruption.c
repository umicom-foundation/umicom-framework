/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_interruption.c
 * Purpose: Keep cancellation and terminal workflow failure visible after passing test results.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include "umicom/distribution/runtime/qualification_view.h"
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    QualificationFixture f; REQUIRE(QualificationStart(&f, 1U, false) == 0);
    UmiReleaseQualificationResult passed = QualificationResult(UMI_QUALIFICATION_PASSED);
    if (strcmp(argv[1], "after-pass") == 0) REQUIRE(QualificationRecordAll(&f) == 0);
    UmiReleaseQualificationIdentity wrong = f.identity; wrong.execution[0] = wrong.execution[0] == '0' ? '1' : '0';
    REQUIRE(UmiReleaseQualificationInterrupt(f.run, &wrong, UMI_STATUS_CANCELLED) == UMI_STATUS_INVALID_STATE);
    REQUIRE(UmiReleaseQualificationInterrupt(f.run, &f.identity, UMI_STATUS_OK) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiReleaseQualificationInterrupt(f.run, &f.identity, (UmiStatus)999) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiReleaseQualificationInterrupt(f.run, &f.identity, UMI_STATUS_CANCELLED) == UMI_STATUS_OK);
    REQUIRE(UmiReleaseQualificationInterrupt(f.run, &f.identity, UMI_STATUS_IO_ERROR) == UMI_STATUS_ALREADY_EXISTS);
    REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &passed) == UMI_STATUS_INVALID_STATE);
    REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "test.000000", &passed) == UMI_STATUS_INVALID_STATE);
    REQUIRE(UmiReleaseQualificationRecordTests(f.run, &f.identity, NULL, 0U, NULL) == UMI_STATUS_INVALID_STATE);
    REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
    REQUIRE(UmiReleaseQualificationInterrupt(f.run, &f.identity, UMI_STATUS_CANCELLED) == UMI_STATUS_INVALID_STATE);
    UmiReleaseQualificationSummary summary;
    REQUIRE(UmiReleaseQualificationDescribe(f.run, &summary) == UMI_STATUS_OK);
    REQUIRE(summary.closed && !summary.readyForOwnerReview && summary.executionStatus == UMI_STATUS_CANCELLED);
    REQUIRE((summary.blockers & UMI_QUALIFICATION_EXECUTION_INTERRUPTED) != 0U);
    if (strcmp(argv[1], "after-pass") == 0)
        REQUIRE(summary.blockers == UMI_QUALIFICATION_EXECUTION_INTERRUPTED && summary.passed == 1U);
    size_t required = 0U;
    REQUIRE(UmiReleaseQualificationFormatReport(f.run, NULL, 0U, &required) == UMI_STATUS_OK);
    char *report = malloc(required); REQUIRE(report != NULL);
    REQUIRE(UmiReleaseQualificationFormatReport(f.run, report, required, &required) == UMI_STATUS_OK);
    REQUIRE(strstr(report, "\nexecution-status\t13\n") != NULL); free(report);
    UmiQualificationView *view = NULL;
    REQUIRE(UmiQualificationViewCapture(f.run, &view) == UMI_STATUS_OK);
    REQUIRE(strcmp(UmiQualificationViewStateText(view), "Review blocked") == 0);
    UmiQualificationViewDestroy(view); QualificationDestroy(&f);
    return 0;
}
