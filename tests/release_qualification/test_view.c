/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_view.c
 * Purpose: Verify immutable review snapshots, exact pagination and non-passing filters.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/distribution/runtime/qualification_view.h"
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *name = argv[1];
    QualificationFixture f;
    size_t tests = strcmp(name, "large") == 0 ? 5000U : 3U;
    REQUIRE(QualificationStart(&f, tests, false) == 0);
    UmiQualificationView *view = NULL, *later = NULL;
    REQUIRE(UmiQualificationViewCapture(f.run, &view) == UMI_STATUS_OK);
    UmiQualificationViewSummary summary;
    REQUIRE(UmiQualificationViewDescribe(view, &summary) == UMI_STATUS_OK);
    REQUIRE(summary.rows == tests + summary.selection.targets + 2U);
    REQUIRE(summary.results.missing == tests && !summary.results.readyForOwnerReview);
    REQUIRE(strcmp(summary.identity.execution, f.identity.execution) == 0);
    REQUIRE(strcmp(UmiQualificationViewStateText(view), "Run in progress") == 0);
    REQUIRE(strcmp(UmiQualificationViewRowStateText(UmiQualificationViewAt(view, 1U)), "Not supplied") == 0);
    UmiQualificationViewRow rows[UMI_QUALIFICATION_VIEW_PAGE_LIMIT];
    size_t count = 0U, total = 0U;
    REQUIRE(UmiQualificationViewPage(view, UMI_QUALIFICATION_VIEW_MISSING, 0U, rows,
        UMI_QUALIFICATION_VIEW_PAGE_LIMIT, &count, &total) == UMI_STATUS_OK);
    REQUIRE(total == tests + 1U);
    if (strcmp(name, "invalid") == 0) {
        REQUIRE(UmiQualificationViewCapture(f.run, &view) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiQualificationViewCapture(NULL, &later) == UMI_STATUS_INVALID_ARGUMENT && later == NULL);
        memset(rows, 0x5a, sizeof(rows)); count = 999U; total = 998U;
        REQUIRE(UmiQualificationViewPage(view, UMI_QUALIFICATION_VIEW_ALL, SIZE_MAX, rows, 1U, &count, &total) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(count == 999U && total == 998U);
        for (size_t i = 0U; i < sizeof(rows); ++i) REQUIRE(((unsigned char *)rows)[i] == 0x5a);
        REQUIRE(UmiQualificationViewPage(view, (UmiQualificationViewFilter)99, 0U, rows, 1U, &count, &total) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiQualificationViewPage(view, UMI_QUALIFICATION_VIEW_ALL, 0U, rows, UMI_QUALIFICATION_VIEW_PAGE_LIMIT + 1U, &count, &total) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiQualificationViewAt(NULL, 0U) == NULL && UmiQualificationViewAt(view, SIZE_MAX) == NULL);
    } else if (strcmp(name, "large") == 0 || strcmp(name, "pagination") == 0) {
        size_t offset = 0U, previous = 0U;
        do {
            REQUIRE(UmiQualificationViewPage(view, UMI_QUALIFICATION_VIEW_TESTS, offset, rows, 2U, &count, &total) == UMI_STATUS_OK);
            REQUIRE(total == tests && count <= 2U);
            for (size_t i = 0U; i < count; ++i) {
                if (offset + i != 0U) REQUIRE(rows[i].index > previous);
                previous = rows[i].index;
                char expected[32]; snprintf(expected, sizeof(expected), "test.%06zu", offset + i);
                REQUIRE(strcmp(rows[i].name, expected) == 0);
            }
            offset += count;
        } while (offset < total);
        REQUIRE(UmiQualificationViewPage(view, UMI_QUALIFICATION_VIEW_TESTS, total, rows, 2U, &count, &total) == UMI_STATUS_OK && count == 0U);
        REQUIRE(UmiQualificationViewPage(view, UMI_QUALIFICATION_VIEW_TARGETS, 0U, NULL, 0U, &count, &total) == UMI_STATUS_OK);
        REQUIRE(count == 0U && total == summary.selection.targets);
    } else if (strcmp(name, "problems") == 0) {
        UmiReleaseQualificationResult result = QualificationResult(UMI_QUALIFICATION_PASSED);
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &result) == UMI_STATUS_OK);
        const UmiReleaseQualificationOutcome outcomes[] = {UMI_QUALIFICATION_FAILED, UMI_QUALIFICATION_SKIPPED, UMI_QUALIFICATION_DISABLED};
        for (size_t i = 0U; i < 3U; ++i) {
            char test[32]; snprintf(test, sizeof(test), "test.%06zu", i);
            result = QualificationResult(outcomes[i]);
            REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, test, &result) == UMI_STATUS_OK);
        }
        REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
        REQUIRE(UmiQualificationViewCapture(f.run, &later) == UMI_STATUS_OK);
        REQUIRE(UmiQualificationViewPage(later, UMI_QUALIFICATION_VIEW_PROBLEMS, 0U, rows, 3U, &count, &total) == UMI_STATUS_OK && count == 3U && total == 3U);
        for (size_t i = 0U; i < count; ++i) REQUIRE(rows[i].result.outcome == outcomes[i]);
        REQUIRE(strcmp(UmiQualificationViewStateText(later), "Review blocked") == 0);
    } else if (strcmp(name, "ownership") == 0 || strcmp(name, "passed") == 0 || strcmp(name, "source-blocked") == 0) {
        REQUIRE(QualificationRecordAll(&f) == 0);
        REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity,
            strcmp(name, "source-blocked") == 0 ? NULL : f.before, f.toolchain) == UMI_STATUS_OK);
        REQUIRE(UmiQualificationViewCapture(f.run, &later) == UMI_STATUS_OK);
        REQUIRE(UmiQualificationViewDescribe(view, &summary) == UMI_STATUS_OK && !summary.results.closed);
        REQUIRE(UmiQualificationViewPage(later, UMI_QUALIFICATION_VIEW_PROBLEMS, 0U, rows, 3U, &count, &total) == UMI_STATUS_OK && total == 0U);
        REQUIRE(UmiQualificationViewDescribe(later, &summary) == UMI_STATUS_OK);
        REQUIRE(summary.results.readyForOwnerReview == (strcmp(name, "source-blocked") != 0));
        QualificationDestroy(&f); memset(&f, 0, sizeof(f));
        REQUIRE(UmiQualificationViewAt(later, summary.rows - 1U) != NULL);
        REQUIRE(strcmp(UmiQualificationViewAt(later, summary.rows - 1U)->name, "test.000002") == 0);
    } else if (strcmp(name, "missing") != 0) return 2;
    UmiQualificationViewDestroy(later); UmiQualificationViewDestroy(view); QualificationDestroy(&f);
    return 0;
}
