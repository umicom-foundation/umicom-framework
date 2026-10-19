/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_compare.c
 * Purpose: Distinguish recovered outcomes, lost coverage and changed plans without borrowing worker state.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/distribution/runtime/qualification_compare.h"
#include "umicom/distribution/runtime/qualification_archive.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include <stdlib.h>
#include <string.h>
static int Record(QualificationFixture *f, UmiReleaseQualificationOutcome outcome, bool missing, bool noisy)
{
    UmiReleaseQualificationResult result = QualificationResult(UMI_QUALIFICATION_PASSED);
    REQUIRE(UmiReleaseQualificationRecordBuild(f->run, &f->identity, &result) == UMI_STATUS_OK);
    if (!missing) for (size_t i = 0U; i < UmiReleaseQualificationCount(f->run); ++i) {
        const UmiReleaseImpactItem *item = UmiReleaseQualificationAt(f->run, i);
        if (item->kind != UMI_RELEASE_INVENTORY_TEST) continue;
        result = QualificationResult(outcome);
        if (noisy) { result.durationMs += 1000U; result.logSha256[0] = result.logSha256[0] == '0' ? '1' : '0'; }
        REQUIRE(UmiReleaseQualificationRecordTest(f->run, &f->identity, item->name, &result) == UMI_STATUS_OK);
    }
    return 0;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    QualificationFixture a, b;
    size_t oldCount = strcmp(name, "removed") == 0 ? 2U : strcmp(name, "large") == 0 ? 4000U : 1U;
    size_t newCount = strcmp(name, "added") == 0 ? 2U : strcmp(name, "large") == 0 ? 4000U : 1U;
    REQUIRE(QualificationStart(&a, oldCount, false) == 0);
    REQUIRE(QualificationStart(&b, newCount, false) == 0);
    UmiReleaseQualificationOutcome old = UMI_QUALIFICATION_PASSED, current = UMI_QUALIFICATION_PASSED;
    if (strcmp(name, "regressed") == 0 || strcmp(name, "large") == 0) current = UMI_QUALIFICATION_FAILED;
    else if (strcmp(name, "recovered") == 0) old = UMI_QUALIFICATION_SKIPPED;
    else if (strcmp(name, "changed") == 0) { old = UMI_QUALIFICATION_SKIPPED; current = UMI_QUALIFICATION_DISABLED; }
    bool missing = strcmp(name, "missing") == 0;
    bool sourceChanged = strcmp(name, "source-changed") == 0;
    if (sourceChanged) {
        UmiReleaseQualificationDestroy(b.run); b.run = NULL;
        UmiReleaseSourceObservationDestroy(b.before); b.before = NULL;
        UmiReleaseSourceOptions options = {.read = QualificationRead, .context = "different",
            .maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT, .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT};
        REQUIRE(UmiReleaseSourceObserve(b.inventory, &options, &b.before) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationCreate(b.plan, b.before, b.execution, b.toolchain, &b.run) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationGetIdentity(b.run, &b.identity) == UMI_STATUS_OK);
        current = UMI_QUALIFICATION_FAILED;
    }
    REQUIRE(Record(&a, old, false, false) == 0);
    REQUIRE(Record(&b, current, missing, strcmp(name, "log-duration") == 0) == 0);
    REQUIRE(UmiReleaseQualificationClose(a.run, &a.identity, a.before, a.toolchain) == UMI_STATUS_OK);
    bool open = strcmp(name, "open") == 0;
    if (!open) REQUIRE(UmiReleaseQualificationClose(b.run, &b.identity, b.before, b.toolchain) == UMI_STATUS_OK);
    UmiQualificationView *before = NULL, *after = NULL;
    REQUIRE(UmiQualificationViewCapture(a.run, &before) == UMI_STATUS_OK);
    if (strcmp(name, "saved") == 0) {
        size_t capacity = 0U;
        REQUIRE(UmiReleaseQualificationFormatReport(b.run, NULL, 0U, &capacity) == UMI_STATUS_OK);
        char *text = malloc(capacity); REQUIRE(text != NULL);
        REQUIRE(UmiReleaseQualificationFormatReport(b.run, text, capacity, &capacity) == UMI_STATUS_OK);
        UmiQualificationArchiveReceipt receipt;
        REQUIRE(UmiQualificationArchiveParse(text, capacity - 1U, &after, &receipt) == UMI_STATUS_OK);
        free(text);
    } else REQUIRE(UmiQualificationViewCapture(b.run, &after) == UMI_STATUS_OK);
    UmiQualificationComparison *comparison = NULL;
    UmiStatus status = UmiQualificationCompare(strcmp(name, "invalid") == 0 ? NULL : before, after, &comparison);
    if (open || strcmp(name, "invalid") == 0) {
        REQUIRE(status == (open ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_INVALID_ARGUMENT) && comparison == NULL);
    } else {
        REQUIRE(status == UMI_STATUS_OK && comparison != NULL);
        UmiQualificationComparisonSummary summary;
        REQUIRE(UmiQualificationComparisonDescribe(comparison, &summary) == UMI_STATUS_OK);
        REQUIRE(summary.rows == (oldCount > newCount ? oldCount : newCount) + 2U);
        REQUIRE(summary.sameRecordedPlan == (!sourceChanged && oldCount == newCount));
        if (strcmp(name, "added") == 0) REQUIRE(summary.added == 1U && summary.removed == 0U);
        else if (strcmp(name, "removed") == 0) REQUIRE(summary.removed == 1U && summary.added == 0U);
        else if (strcmp(name, "recovered") == 0) REQUIRE(summary.recovered == 1U);
        else if (strcmp(name, "regressed") == 0 || missing || sourceChanged) REQUIRE(summary.regressed == 1U);
        else if (strcmp(name, "large") == 0) REQUIRE(summary.regressed == 4000U);
        else if (strcmp(name, "changed") == 0) REQUIRE(summary.resultChanged == 1U);
        else {
            REQUIRE(strcmp(name, "same") == 0 || strcmp(name, "log-duration") == 0 || strcmp(name, "saved") == 0);
            REQUIRE(summary.unchanged == summary.rows);
        }
        REQUIRE(summary.after.savedReport == (strcmp(name, "saved") == 0));
        REQUIRE(UmiQualificationComparisonAt(comparison, summary.rows) == NULL);
        size_t counts = summary.unchanged + summary.added + summary.removed + summary.regressed + summary.recovered + summary.resultChanged;
        REQUIRE(counts == summary.rows);
    }
    UmiQualificationViewDestroy(before); UmiQualificationViewDestroy(after);
    QualificationDestroy(&a); QualificationDestroy(&b);
    if (comparison != NULL) {
        const UmiQualificationDifference *row = UmiQualificationComparisonAt(comparison, 2U);
        REQUIRE(row != NULL && strcmp(row->name, "test.000000") == 0);
        REQUIRE(row->beforePresent && row->afterPresent);
        REQUIRE(strcmp(UmiQualificationChangeText(row->change), "Unknown") != 0);
    }
    REQUIRE(UmiQualificationComparisonCount(NULL) == 0U);
    REQUIRE(strcmp(UmiQualificationChangeText((UmiQualificationChange)99), "Unknown") == 0);
    UmiQualificationComparisonDestroy(comparison); return 0;
}
