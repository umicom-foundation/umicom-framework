/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_archive_semantics.c
 * Purpose: Preserve every non-passing outcome and exact name across historical report reopening.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/distribution/runtime/qualification_archive.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    QualificationFixture f; REQUIRE(QualificationStart(&f, 7U, false) == 0);
    bool names = strcmp(name, "names") == 0;
    if (names) {
        UmiReleaseQualificationDestroy(f.run); f.run = NULL;
        UmiReleaseImpactDestroy(f.plan); f.plan = NULL;
        UmiReleaseInventoryDestroy(f.inventory); f.inventory = NULL;
        UmiReleaseSourceObservationDestroy(f.before); f.before = NULL;
        REQUIRE(FixtureTest(&f.text, "odd\tname\nend\\\xc3\xa9", "library", NULL, false));
        REQUIRE(FixtureParse(&f.text, &f.inventory) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseImpactCreate(f.inventory, NULL, 0U, true, &f.plan) == UMI_STATUS_OK);
        UmiReleaseSourceOptions options = {.read = QualificationRead,
            .maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT, .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT};
        REQUIRE(UmiReleaseSourceObserve(f.inventory, &options, &f.before) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationCreate(f.plan, f.before, f.execution, f.toolchain, &f.run) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationGetIdentity(f.run, &f.identity) == UMI_STATUS_OK);
    }
    UmiReleaseQualificationResult build = QualificationResult(UMI_QUALIFICATION_PASSED);
    bool failedBuild = strcmp(name, "failed-build") == 0;
    if (failedBuild) build = QualificationResult(UMI_QUALIFICATION_FAILED);
    REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &build) == UMI_STATUS_OK);
    bool partial = strcmp(name, "partial") == 0, unknown = strcmp(name, "unknown-process") == 0;
    if (unknown) {
        UmiReleaseQualificationNamedResult rows[7];
        char testNames[7][32];
        for (size_t i = 0U; i < 7U; ++i) {
            (void)snprintf(testNames[i], sizeof(testNames[i]), "test.%06zu", i);
            rows[i].name = testNames[i]; rows[i].result = build; rows[i].result.exitCodeKnown = false;
        }
        UmiReleaseQualificationResult process = QualificationResult(UMI_QUALIFICATION_FAILED);
        process.exitCode = 0; process.exitCodeKnown = false;
        REQUIRE(UmiReleaseQualificationRecordTests(f.run, &f.identity, rows, 7U, &process) == UMI_STATUS_OK);
    } else {
        size_t test = 0U;
        for (size_t i = 0U; i < UmiReleaseQualificationCount(f.run); ++i) {
            const UmiReleaseImpactItem *item = UmiReleaseQualificationAt(f.run, i);
            if (item->kind != UMI_RELEASE_INVENTORY_TEST) continue;
            UmiReleaseQualificationResult result = QualificationResult((UmiReleaseQualificationOutcome)(test % 7U));
            if (strcmp(name, "integer-boundaries") == 0 && result.outcome != UMI_QUALIFICATION_PASSED) {
                result.exitCode = test % 2U == 0U ? INT_MIN : INT_MAX;
                result.durationMs = UINT64_MAX;
            }
            if (!partial || test == 0U)
                REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, item->name, &result) == UMI_STATUS_OK);
            ++test;
        }
    }
    bool changed = strcmp(name, "changed-tools") == 0;
    char toolchain[65]; memcpy(toolchain, f.toolchain, sizeof(toolchain));
    if (changed) toolchain[0] = toolchain[0] == '0' ? '1' : '0';
    REQUIRE(names || partial || unknown || failedBuild || changed ||
        strcmp(name, "mixed") == 0 || strcmp(name, "integer-boundaries") == 0);
    REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, toolchain) == UMI_STATUS_OK);
    UmiReleaseQualificationSummary original;
    REQUIRE(UmiReleaseQualificationDescribe(f.run, &original) == UMI_STATUS_OK && !original.readyForOwnerReview);
    size_t capacity = 0U; REQUIRE(UmiReleaseQualificationFormatReport(f.run, NULL, 0U, &capacity) == UMI_STATUS_OK);
    char *text = malloc(capacity); REQUIRE(text != NULL);
    REQUIRE(UmiReleaseQualificationFormatReport(f.run, text, capacity, &capacity) == UMI_STATUS_OK);
    UmiQualificationView *view = NULL; UmiQualificationArchiveReceipt receipt;
    REQUIRE(UmiQualificationArchiveParse(text, capacity - 1U, &view, &receipt) == UMI_STATUS_OK);
    UmiQualificationViewSummary saved; REQUIRE(UmiQualificationViewDescribe(view, &saved) == UMI_STATUS_OK);
    REQUIRE(saved.savedReport && saved.results.blockers == original.blockers);
    REQUIRE(saved.results.recorded == original.recorded && saved.results.missing == original.missing);
    REQUIRE(saved.results.passed == original.passed && saved.results.failed == original.failed &&
        saved.results.skipped == original.skipped && saved.results.notRun == original.notRun &&
        saved.results.cancelled == original.cancelled && saved.results.timedOut == original.timedOut &&
        saved.results.disabled == original.disabled);
    for (size_t i = 0U; i < UmiReleaseQualificationCount(f.run); ++i) {
        const UmiReleaseImpactItem *item = UmiReleaseQualificationAt(f.run, i);
        const UmiQualificationViewRow *row = UmiQualificationViewAt(view, i + 2U);
        REQUIRE(row != NULL && strcmp(item->name, row->name) == 0);
        if (item->kind == UMI_RELEASE_INVENTORY_TEST && row->recorded) {
            UmiReleaseQualificationResult result;
            REQUIRE(UmiReleaseQualificationGetTest(f.run, item->name, &result) == UMI_STATUS_OK);
            REQUIRE(row->result.outcome == result.outcome && row->result.status == result.status &&
                row->result.exitCode == result.exitCode && row->result.exitCodeKnown == result.exitCodeKnown &&
                row->result.durationMs == result.durationMs && strcmp(row->result.logSha256, result.logSha256) == 0);
        }
    }
    free(text); UmiQualificationViewDestroy(view); QualificationDestroy(&f); return 0;
}
