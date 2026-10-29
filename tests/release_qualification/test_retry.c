/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_retry.c
 *
 * PURPOSE:
 *   Verify next-attempt planning retains incomplete evidence and current source changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include "umicom/distribution/runtime/qualification_retry.h"
#include "umicom/distribution/runtime/qualification_archive.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include <stdlib.h>
#include <string.h>

static bool Is(const char *mode, const char *value) { return strcmp(mode, value) == 0; }
static int NewInventory(QualificationFixture *f, bool remove, bool add,
    UmiReleaseInventory **out)
{
    Fixture text;
    REQUIRE(FixtureStart(&text));
    REQUIRE(FixtureTarget(&text, "library", NULL));
    REQUIRE(FixtureSource(&text, "/repo/library.c", "library"));
    for (size_t i = 0U; i < (add ? 4U : 3U); ++i) {
        if (remove && i == 1U) continue;
        char name[32];
        (void)snprintf(name, sizeof(name), "test.%06zu", i);
        REQUIRE(FixtureTest(&text, name, "library", NULL, false));
    }
    if (!remove && !add) {
        char *generation = strstr(text.text, "0123456789abcdef0123456789abcdef");
        REQUIRE(generation != NULL);
        generation[0] = '1';
    }
    REQUIRE(FixtureParse(&text, out) == UMI_STATUS_OK);
    FixtureDestroy(&text);
    (void)f;
    return 0;
}
/* Reopen the exact selected reasons as historical evidence. This couples the
 * planner regression to the saved-report reader, not just an in-memory count. */
static int CheckArchive(const UmiReleaseImpactPlan *plan,
    const UmiReleaseSourceObservation *source, const char *execution, const char *toolchain)
{
    UmiReleaseQualification *attempt = NULL;
    REQUIRE(UmiReleaseQualificationCreate(plan, source, execution, toolchain, &attempt) == UMI_STATUS_OK);
    size_t length = 0U;
    REQUIRE(UmiReleaseQualificationFormatReport(attempt, NULL, 0U, &length) == UMI_STATUS_OK);
    char *text = malloc(length);
    REQUIRE(text != NULL);
    REQUIRE(UmiReleaseQualificationFormatReport(attempt, text, length, &length) == UMI_STATUS_OK);
    UmiQualificationView *view = NULL;
    UmiQualificationArchiveReceipt receipt;
    REQUIRE(UmiQualificationArchiveParse(text, length - 1U, &view, &receipt) == UMI_STATUS_OK);
    UmiReleaseImpactSummary expected;
    UmiQualificationViewSummary actual;
    REQUIRE(UmiReleaseImpactDescribe(plan, &expected) == UMI_STATUS_OK);
    REQUIRE(UmiQualificationViewDescribe(view, &actual) == UMI_STATUS_OK);
    REQUIRE(actual.savedReport && !actual.results.closed && !actual.results.readyForOwnerReview);
    REQUIRE(actual.selection.fallbackReasons == expected.fallbackReasons);
    for (size_t i = 0U; i < UmiReleaseImpactCount(plan); ++i) {
        const UmiReleaseImpactItem *item = UmiReleaseImpactAt(plan, i);
        const UmiQualificationViewRow *row = UmiQualificationViewAt(view, i + 2U);
        REQUIRE(row != NULL && strcmp(row->name, item->name) == 0 && row->reasons == item->reasons);
    }
    free(text);
    UmiQualificationViewDestroy(view);
    UmiReleaseQualificationDestroy(attempt);
    return 0;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    const char *modes[] = {"passed", "failed", "skipped", "disabled", "cancelled",
        "timed-out", "not-run", "missing-test", "missing-history", "open", "missing-build",
        "failed-build", "process-failed", "process-passed", "interrupted",
        "source-changed", "source-unstable", "toolchain-changed", "toolchain-unstable",
        "unknown-toolchain", "invalid-toolchain", "baseline-mismatch", "no-baseline",
        "inventory-changed", "current-stale", "new-test", "removed-test", "scoped-history",
        "no-tests", "null-inventory", "null-current", "null-output", "null-summary",
        "occupied-output", "ownership", "scale", "untested-target"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(modes) / sizeof(modes[0]); ++i)
        if (Is(mode, modes[i])) known = true;
    REQUIRE(known);
    size_t tests = Is(mode, "no-tests") ? 0U : Is(mode, "scale") ? 4096U : 3U;
    QualificationFixture f;
    REQUIRE(QualificationStart(&f, tests, false) == 0);
    if (Is(mode, "untested-target")) {
        /* A toolchain change can affect a configured target even when no test
         * owns it. Rebuild a synthetic attempt including that uncovered target. */
        REQUIRE(FixtureTarget(&f.text, "uncovered", NULL));
        UmiReleaseQualificationDestroy(f.run); f.run = NULL;
        UmiReleaseImpactDestroy(f.plan); f.plan = NULL;
        UmiReleaseSourceObservationDestroy(f.before); f.before = NULL;
        UmiReleaseInventoryDestroy(f.inventory); f.inventory = NULL;
        REQUIRE(FixtureParse(&f.text, &f.inventory) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseImpactCreate(f.inventory, NULL, 0U, true, &f.plan) == UMI_STATUS_OK);
        UmiReleaseSourceOptions observe = {.read = QualificationRead,
            .maximumFileBytes = 1024U, .maximumTotalBytes = 8192U};
        REQUIRE(UmiReleaseSourceObserve(f.inventory, &observe, &f.before) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationCreate(f.plan, f.before, f.execution, f.toolchain,
            &f.run) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationGetIdentity(f.run, &f.identity) == UMI_STATUS_OK);
    }
    if (Is(mode, "scoped-history")) {
        UmiReleaseQualificationDestroy(f.run); f.run = NULL;
        UmiReleaseImpactPlan *partial = NULL;
        const char *names[] = {"test.000000"};
        REQUIRE(UmiReleaseImpactCreateWithTests(f.inventory, NULL, 0U, names, 1U,
            false, &partial) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationCreate(partial, f.before, f.execution,
            f.toolchain, &f.run) == UMI_STATUS_OK);
        UmiReleaseImpactDestroy(partial);
        REQUIRE(UmiReleaseQualificationGetIdentity(f.run, &f.identity) == UMI_STATUS_OK);
    }
    UmiReleaseQualificationResult pass = QualificationResult(UMI_QUALIFICATION_PASSED);
    if (!Is(mode, "missing-build")) {
        UmiReleaseQualificationResult build = QualificationResult(Is(mode, "failed-build") ?
            UMI_QUALIFICATION_FAILED : UMI_QUALIFICATION_PASSED);
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &build) == UMI_STATUS_OK);
        if (Is(mode, "process-failed") || Is(mode, "process-passed")) {
            UmiReleaseQualificationNamedResult rows[3] = {
                {.name = "test.000000", .result = pass},
                {.name = "test.000001", .result = pass},
                {.name = "test.000002", .result = pass}
            };
            for (size_t i = 0U; i < 3U; ++i) rows[i].result.exitCodeKnown = false;
            UmiReleaseQualificationResult process = QualificationResult(Is(mode, "process-failed") ?
                UMI_QUALIFICATION_FAILED : UMI_QUALIFICATION_PASSED);
            REQUIRE(UmiReleaseQualificationRecordTests(f.run, &f.identity, rows, 3U,
                &process) == UMI_STATUS_OK);
        } else {
            for (size_t i = 0U; i < UmiReleaseQualificationCount(f.run); ++i) {
                const UmiReleaseImpactItem *item = UmiReleaseQualificationAt(f.run, i);
                if (item->kind != UMI_RELEASE_INVENTORY_TEST) continue;
                if (Is(mode, "missing-test") && strcmp(item->name, "test.000001") == 0) continue;
                UmiReleaseQualificationOutcome outcome = UMI_QUALIFICATION_PASSED;
                if (strcmp(item->name, "test.000001") == 0) {
                    if (Is(mode, "failed") || Is(mode, "removed-test") || Is(mode, "ownership")) outcome = UMI_QUALIFICATION_FAILED;
                    else if (Is(mode, "skipped")) outcome = UMI_QUALIFICATION_SKIPPED;
                    else if (Is(mode, "disabled")) outcome = UMI_QUALIFICATION_DISABLED;
                    else if (Is(mode, "cancelled")) outcome = UMI_QUALIFICATION_CANCELLED;
                    else if (Is(mode, "timed-out")) outcome = UMI_QUALIFICATION_TIMED_OUT;
                    else if (Is(mode, "not-run")) outcome = UMI_QUALIFICATION_NOT_RUN;
                }
                UmiReleaseQualificationResult result = QualificationResult(outcome);
                REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, item->name,
                    &result) == UMI_STATUS_OK);
            }
        }
    }
    UmiReleaseSourceOptions options = {.read = QualificationRead,
        .maximumFileBytes = 1024U, .maximumTotalBytes = 8192U};
    UmiReleaseSourceObservation *changed = NULL, *otherObservation = NULL;
    options.context = "different source";
    REQUIRE(UmiReleaseSourceObserve(f.inventory, &options, &changed) == UMI_STATUS_OK);
    char otherToolchain[65];
    REQUIRE(UmiSha256Buffer("another compiler", 16U, otherToolchain) == UMI_STATUS_OK);
    if (Is(mode, "interrupted"))
        REQUIRE(UmiReleaseQualificationInterrupt(f.run, &f.identity, UMI_STATUS_IO_ERROR) == UMI_STATUS_OK);
    if (!Is(mode, "open"))
        REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity,
            Is(mode, "source-unstable") ? changed : f.before,
            Is(mode, "toolchain-unstable") ? otherToolchain : f.toolchain) == UMI_STATUS_OK);
    UmiReleaseQualificationSummary oldSummary;
    REQUIRE(UmiReleaseQualificationDescribe(f.run, &oldSummary) == UMI_STATUS_OK);

    UmiReleaseInventory *otherInventory = NULL;
    bool newInventory = Is(mode, "inventory-changed") || Is(mode, "current-stale") ||
        Is(mode, "new-test") || Is(mode, "removed-test");
    if (newInventory) {
        REQUIRE(NewInventory(&f, Is(mode, "removed-test"), Is(mode, "new-test"), &otherInventory) == 0);
        options.context = NULL;
        REQUIRE(UmiReleaseSourceObserve(otherInventory, &options, &otherObservation) == UMI_STATUS_OK);
    }
    const UmiReleaseInventory *inventory = newInventory ? otherInventory : f.inventory;
    const UmiReleaseSourceObservation *current = Is(mode, "source-changed") ? changed :
        newInventory && !Is(mode, "current-stale") ? otherObservation : f.before;
    const UmiReleaseSourceObservation *baseline = Is(mode, "no-baseline") ? NULL :
        Is(mode, "baseline-mismatch") ? changed : f.before;
    const char *toolchain = Is(mode, "toolchain-changed") ? otherToolchain :
        Is(mode, "unknown-toolchain") || Is(mode, "untested-target") ? NULL : Is(mode, "invalid-toolchain") ? "invalid" : f.toolchain;
    UmiReleaseImpactPlan *plan = NULL, *original = NULL;
    if (Is(mode, "occupied-output")) {
        REQUIRE(UmiReleaseImpactCreate(f.inventory, NULL, 0U, false, &plan) == UMI_STATUS_OK);
        original = plan;
    }
    UmiQualificationRetrySummary summary;
    memset(&summary, 0x35, sizeof(summary));
    UmiQualificationRetrySummary sentinel;
    memcpy(&sentinel, &summary, sizeof(summary));
    UmiStatus status = UmiReleaseQualificationPlanNext(
        Is(mode, "null-inventory") ? NULL : inventory, baseline,
        Is(mode, "null-current") ? NULL : current,
        Is(mode, "missing-history") ? NULL : f.run, toolchain,
        Is(mode, "null-output") ? NULL : &plan,
        Is(mode, "null-summary") ? NULL : &summary);
    bool invalid = strncmp(mode, "null-", 5U) == 0 || Is(mode, "invalid-toolchain") ||
        Is(mode, "occupied-output");
    UmiStatus expected = invalid ? UMI_STATUS_INVALID_ARGUMENT : Is(mode, "current-stale") ?
        UMI_STATUS_INVALID_STATE : Is(mode, "no-tests") ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK;
    REQUIRE(status == expected);
    UmiReleaseQualificationSummary afterSummary;
    REQUIRE(UmiReleaseQualificationDescribe(f.run, &afterSummary) == UMI_STATUS_OK);
    REQUIRE(afterSummary.closed == oldSummary.closed && afterSummary.recorded == oldSummary.recorded &&
        afterSummary.blockers == oldSummary.blockers && afterSummary.passed == oldSummary.passed);
    if (status != UMI_STATUS_OK) {
        REQUIRE(plan == original && memcmp(&summary, &sentinel, sizeof(summary)) == 0);
    } else {
        uint32_t reasons = Is(mode, "missing-history") ? UMI_QUALIFICATION_RETRY_NO_HISTORY :
            Is(mode, "open") ? UMI_QUALIFICATION_RETRY_OPEN_HISTORY :
            Is(mode, "missing-build") || Is(mode, "failed-build") ? UMI_QUALIFICATION_RETRY_BUILD_UNQUALIFIED :
            Is(mode, "process-failed") ? UMI_QUALIFICATION_RETRY_RUNNER_UNQUALIFIED :
            Is(mode, "interrupted") ? UMI_QUALIFICATION_RETRY_INTERRUPTED :
            Is(mode, "source-unstable") || Is(mode, "toolchain-unstable") ? UMI_QUALIFICATION_RETRY_UNSTABLE_ATTEMPT :
            Is(mode, "toolchain-changed") || Is(mode, "unknown-toolchain") || Is(mode, "untested-target") ? UMI_QUALIFICATION_RETRY_TOOLCHAIN_MISMATCH :
            Is(mode, "baseline-mismatch") || Is(mode, "no-baseline") ? UMI_QUALIFICATION_RETRY_BASELINE_MISMATCH :
            newInventory ? UMI_QUALIFICATION_RETRY_INVENTORY_MISMATCH : 0U;
        REQUIRE(summary.historyReasons == reasons);
        size_t configured = Is(mode, "new-test") ? 4U : Is(mode, "removed-test") ? 2U : tests;
        REQUIRE(summary.configuredTests == configured);
        UmiReleaseImpactSummary selection;
        REQUIRE(UmiReleaseImpactDescribe(plan, &selection) == UMI_STATUS_OK);
        bool oneRetry = Is(mode, "failed") || Is(mode, "skipped") || Is(mode, "disabled") ||
            Is(mode, "cancelled") || Is(mode, "timed-out") || Is(mode, "not-run") || Is(mode, "missing-test") || Is(mode, "ownership");
        size_t required = reasons != 0U ? configured + (Is(mode, "removed-test") ? 1U : 0U) :
            oneRetry ? 1U : Is(mode, "scoped-history") ? 2U : 0U;
        REQUIRE(summary.requiredTests == required);
        REQUIRE(summary.removedOutstandingTests == (Is(mode, "removed-test") ? 1U : 0U));
        size_t selected = reasons != 0U || Is(mode, "source-changed") ? configured : required;
        REQUIRE(selection.tests == selected && selection.targets ==
            (selected != 0U ? (Is(mode, "untested-target") ? 2U : 1U) : 0U));
        if (Is(mode, "untested-target"))
            REQUIRE(PlanContains(plan, "uncovered", UMI_RELEASE_IMPACT_FULL));
        REQUIRE(selection.noWork == (selected == 0U));
        if (reasons != 0U) REQUIRE((selection.fallbackReasons & UMI_RELEASE_IMPACT_REQUESTED_FULL) != 0U);
        else REQUIRE(selection.fallbackReasons == 0U);
        if (oneRetry) {
            REQUIRE(summary.historicalPasses == 2U);
            REQUIRE(PlanContains(plan, "test.000001", UMI_RELEASE_IMPACT_REQUIRED_TEST));
            REQUIRE(!PlanContains(plan, "test.000000", 0U));
            REQUIRE(summary.missingResults == (Is(mode, "missing-test") ? 1U : 0U));
            REQUIRE(summary.nonpassingResults == (Is(mode, "missing-test") ? 0U : 1U));
        }
        if (Is(mode, "source-changed")) REQUIRE(summary.historicalPasses == tests);
        if (!selection.noWork)
            REQUIRE(CheckArchive(plan, current, f.execution, f.toolchain) == 0);
        if (Is(mode, "ownership")) {
            QualificationDestroy(&f);
            REQUIRE(UmiReleaseImpactCount(plan) == 2U);
            REQUIRE(PlanContains(plan, "test.000001", UMI_RELEASE_IMPACT_REQUIRED_TEST));
            REQUIRE(UmiReleaseImpactInventoryFingerprint(plan) != NULL);
        }
    }
    UmiReleaseImpactDestroy(plan);
    UmiReleaseSourceObservationDestroy(changed);
    UmiReleaseSourceObservationDestroy(otherObservation);
    UmiReleaseInventoryDestroy(otherInventory);
    QualificationDestroy(&f);
    return 0;
}
