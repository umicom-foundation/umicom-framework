/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_retry_report.c
 *
 * PURPOSE:
 *   Verify saved evidence can plan retries without trusting corrupt or stale records.
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
#include "umicom/distribution/runtime/qualification_report.h"
#include <stdlib.h>
#include <string.h>

static bool Is(const char *a, const char *b) { return strcmp(a, b) == 0; }
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    const char *modes[] = {"passed", "failed", "skipped", "partial", "open", "changed",
        "unknown-tools", "no-baseline", "wrong-digest", "invalid-digest", "null-digest",
        "null-report", "empty", "truncated", "embedded-nul", "oversized", "unknown-reason",
        "unknown-fallback", "null-inventory", "null-current", "null-output", "null-summary",
        "occupied-output", "invalid-tools", "ownership", "retry-reason", "unmapped-retry"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(modes) / sizeof(modes[0]); ++i)
        if (Is(mode, modes[i])) known = true;
    REQUIRE(known);
    QualificationFixture f;
    REQUIRE(QualificationStart(&f, 3U, false) == 0);
    if (Is(mode, "retry-reason") || Is(mode, "unmapped-retry")) {
        UmiReleaseQualificationDestroy(f.run); f.run = NULL;
        UmiReleaseImpactDestroy(f.plan); f.plan = NULL;
        const char *names[] = {Is(mode, "unmapped-retry") ? "absent.test" : "test.000001"};
        REQUIRE(UmiReleaseImpactCreateWithTests(f.inventory, NULL, 0U, names, 1U, false, &f.plan) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationCreate(f.plan, f.before, f.execution, f.toolchain, &f.run) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationGetIdentity(f.run, &f.identity) == UMI_STATUS_OK);
    }
    UmiReleaseQualificationResult pass = QualificationResult(UMI_QUALIFICATION_PASSED);
    REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &pass) == UMI_STATUS_OK);
    for (size_t i = 0U; i < UmiReleaseQualificationCount(f.run); ++i) {
        const UmiReleaseImpactItem *item = UmiReleaseQualificationAt(f.run, i);
        if (item->kind != UMI_RELEASE_INVENTORY_TEST) continue;
        if (Is(mode, "partial") && strcmp(item->name, "test.000001") == 0) continue;
        UmiReleaseQualificationResult result = pass;
        if (strcmp(item->name, "test.000001") == 0) {
            if (Is(mode, "failed") || Is(mode, "ownership") || Is(mode, "retry-reason") || Is(mode, "unmapped-retry"))
                result = QualificationResult(UMI_QUALIFICATION_FAILED);
            if (Is(mode, "skipped")) result = QualificationResult(UMI_QUALIFICATION_SKIPPED);
        }
        REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, item->name, &result) == UMI_STATUS_OK);
    }
    if (!Is(mode, "open"))
        REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
    size_t capacity = 0U;
    REQUIRE(UmiReleaseQualificationFormatReport(f.run, NULL, 0U, &capacity) == UMI_STATUS_OK);
    /* Extra room is only for malformed-field fixtures, never used as report length. */
    char *report = malloc(capacity + 32U);
    REQUIRE(report != NULL);
    REQUIRE(UmiReleaseQualificationFormatReport(f.run, report, capacity, &capacity) == UMI_STATUS_OK);
    size_t length = capacity - 1U;
    UmiReleaseQualificationDestroy(f.run); f.run = NULL;
    if (Is(mode, "unknown-reason") || Is(mode, "unknown-fallback")) {
        char *field = strstr(report, Is(mode, "unknown-reason") ? "\ntarget\t" : "\nselection\t");
        REQUIRE(field != NULL);
        field = strchr(field + 1, '\t'); REQUIRE(field != NULL);
        field = strchr(field + 1, '\t'); REQUIRE(field != NULL);
        ++field;
        char *end = strpbrk(field, "\t\n"); REQUIRE(end != NULL);
        const char *value = Is(mode, "unknown-reason") ? "256" : "128";
        size_t digits = strlen(value), remaining = strlen(end) + 1U;
        memmove(field + digits, end, remaining);
        memcpy(field, value, digits);
        length = strlen(report);
    }
    if (Is(mode, "empty")) length = 0U;
    if (Is(mode, "truncated")) --length;
    if (Is(mode, "embedded-nul")) report[length / 2U] = '\0';
    char digest[65];
    REQUIRE(UmiSha256Buffer(report, length, digest) == UMI_STATUS_OK);
    if (Is(mode, "wrong-digest")) digest[0] = digest[0] == '0' ? '1' : '0';
    UmiReleaseSourceObservation *changed = NULL;
    UmiReleaseSourceOptions options = {.read = QualificationRead, .context = "changed",
        .maximumFileBytes = 1024U, .maximumTotalBytes = 8192U};
    REQUIRE(UmiReleaseSourceObserve(f.inventory, &options, &changed) == UMI_STATUS_OK);
    UmiReleaseImpactPlan *plan = NULL, *original = NULL;
    if (Is(mode, "occupied-output")) {
        REQUIRE(UmiReleaseImpactCreate(f.inventory, NULL, 0U, false, &plan) == UMI_STATUS_OK);
        original = plan;
    }
    UmiQualificationRetrySummary summary, sentinel;
    memset(&summary, 0x35, sizeof(summary)); memcpy(&sentinel, &summary, sizeof(summary));
    UmiStatus status = UmiReleaseQualificationPlanFromReport(
        Is(mode, "null-inventory") ? NULL : f.inventory,
        Is(mode, "no-baseline") ? NULL : f.before,
        Is(mode, "null-current") ? NULL : Is(mode, "changed") ? changed : f.before,
        Is(mode, "null-report") ? NULL : report,
        Is(mode, "oversized") ? UMI_QUALIFICATION_REPORT_LIMIT : length,
        Is(mode, "null-digest") ? NULL : Is(mode, "invalid-digest") ? "invalid" : digest,
        Is(mode, "unknown-tools") ? NULL : Is(mode, "invalid-tools") ? "invalid" : f.toolchain,
        Is(mode, "null-output") ? NULL : &plan,
        Is(mode, "null-summary") ? NULL : &summary);
    bool invalid = strncmp(mode, "null-", 5U) == 0 || Is(mode, "occupied-output") ||
        Is(mode, "invalid-digest") || Is(mode, "invalid-tools");
    bool corrupt = Is(mode, "empty") || Is(mode, "truncated") || Is(mode, "embedded-nul") ||
        Is(mode, "unknown-reason") || Is(mode, "unknown-fallback");
    UmiStatus expected = invalid ? UMI_STATUS_INVALID_ARGUMENT : Is(mode, "wrong-digest") ?
        UMI_STATUS_INVALID_STATE : Is(mode, "oversized") ? UMI_STATUS_CAPACITY_EXCEEDED :
        corrupt ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    REQUIRE(status == expected);
    if (status != UMI_STATUS_OK) {
        REQUIRE(plan == original && memcmp(&summary, &sentinel, sizeof(summary)) == 0);
    } else {
        UmiReleaseImpactSummary selected;
        REQUIRE(UmiReleaseImpactDescribe(plan, &selected) == UMI_STATUS_OK);
        bool full = Is(mode, "open") || Is(mode, "unknown-tools") || Is(mode, "no-baseline");
        size_t due = full || Is(mode, "retry-reason") ? 3U : Is(mode, "passed") || Is(mode, "changed") ? 0U : 1U;
        REQUIRE(summary.requiredTests == due);
        REQUIRE(selected.tests == (Is(mode, "changed") ? 3U : due));
        if (Is(mode, "retry-reason")) REQUIRE(summary.missingResults == 2U);
        if (full) REQUIRE((selected.fallbackReasons & UMI_RELEASE_IMPACT_REQUESTED_FULL) != 0U);
        if (Is(mode, "ownership")) {
            free(report); report = NULL;
            QualificationDestroy(&f);
            REQUIRE(PlanContains(plan, "test.000001", UMI_RELEASE_IMPACT_REQUIRED_TEST));
            REQUIRE(UmiReleaseImpactCount(plan) == 2U);
        }
    }
    free(report);
    UmiReleaseImpactDestroy(plan);
    UmiReleaseSourceObservationDestroy(changed);
    QualificationDestroy(&f);
    return 0;
}
