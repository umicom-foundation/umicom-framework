/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_report_comparison.c
 * Purpose: Verify saved test identity, inventory changes and result ownership.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/testing/ctest_report_comparison.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Line %d: %s\n", __LINE__, #condition); return 1; } } while (0)

/* These are the six observable outcomes emitted by CTest, including tests that
 * never executed. Keeping disabled separate prevents accidental credit as a fix. */
static const char *outcomes[] = {
    "status='run' time='0'/>",
    "status='fail' time='0'><failure message='Failed'/></testcase>",
    "status='fail' time='1'><failure message='Timeout'/></testcase>",
    "status='notrun' time='0'><skipped message='SKIP_RETURN_CODE=77'/></testcase>",
    "status='disabled' time='0'/>",
    "status='notrun' time='0'><skipped message='Unable to find executable'/></testcase>"
};
static const UmiTestState states[] = {
    UMI_TEST_STATE_PASSED, UMI_TEST_STATE_FAILED, UMI_TEST_STATE_TIMED_OUT,
    UMI_TEST_STATE_SKIPPED, UMI_TEST_STATE_SKIPPED, UMI_TEST_STATE_NOT_RUN
};

static UmiStatus Single(const char *name, size_t outcome, UmiCtestReport **report)
{
    char xml[1200];
    int size = snprintf(xml, sizeof xml, "<testsuite tests='1'><testcase name='%s' %s</testsuite>",
        name, outcomes[outcome]);
    if (size <= 0 || (size_t)size >= sizeof xml) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiCtestReportImport(xml, (size_t)size, report);
}

static int VerifyTransitions(size_t before_kind)
{
    for (size_t after_kind = 0U; after_kind < 6U; ++after_kind) {
        UmiCtestReport *before = NULL, *after = NULL;
        UmiCtestReportComparison *comparison = NULL;
        CHECK(Single("named.case", before_kind, &before) == UMI_STATUS_OK);
        CHECK(Single("named.case", after_kind, &after) == UMI_STATUS_OK);
        CHECK(UmiCtestReportCompare(before, after, &comparison) == UMI_STATUS_OK);
        UmiCtestReportComparisonSummary summary;
        CHECK(UmiCtestReportComparisonReadSummary(comparison, &summary) == UMI_STATUS_OK);
        CHECK(summary.before_tests == 1U && summary.after_tests == 1U);
        CHECK(summary.added == 0U && summary.removed == 0U);
        if (before_kind == after_kind) {
            CHECK(summary.unchanged == 1U && summary.changed == 0U);
            CHECK(UmiCtestReportComparisonAt(comparison, 0U) == NULL);
        } else {
            const UmiCtestReportChange *change = UmiCtestReportComparisonAt(comparison, 0U);
            CHECK(summary.unchanged == 0U && summary.changed == 1U && change != NULL);
            CHECK(strcmp(change->name, "named.case") == 0);
            CHECK(change->before_present && change->after_present);
            CHECK(change->before == states[before_kind] && change->after == states[after_kind]);
            CHECK(change->before_disabled == (before_kind == 4U));
            CHECK(change->after_disabled == (after_kind == 4U));
            if (after_kind == 0U) {
                CHECK(summary.recovered == 1U && change->kind == UMI_CTEST_CHANGE_RECOVERED);
            } else if (before_kind == 0U) {
                CHECK(summary.regressed == 1U && change->kind == UMI_CTEST_CHANGE_REGRESSED);
            } else {
                CHECK(summary.outcome_changed == 1U && change->kind == UMI_CTEST_CHANGE_OUTCOME);
            }
            CHECK(UmiCtestReportComparisonAt(comparison, 1U) == NULL);
        }
        UmiCtestReportComparisonDestroy(comparison);
        UmiCtestReportDestroy(after);
        UmiCtestReportDestroy(before);
    }
    return 0;
}

static int VerifyInventory(void)
{
    UmiCtestReport *before = NULL, *after = NULL;
    UmiCtestReportComparison *comparison = NULL;
    CHECK(Single("old.failure", 1U, &before) == UMI_STATUS_OK);
    CHECK(Single("new.success", 0U, &after) == UMI_STATUS_OK);
    CHECK(UmiCtestReportCompare(before, after, &comparison) == UMI_STATUS_OK);
    UmiCtestReportComparisonSummary summary;
    CHECK(UmiCtestReportComparisonReadSummary(comparison, &summary) == UMI_STATUS_OK);
    CHECK(summary.before_tests == 1U && summary.after_tests == 1U && summary.changed == 2U);
    CHECK(summary.added == 1U && summary.removed == 1U && summary.recovered == 0U);
    const UmiCtestReportChange *added = UmiCtestReportComparisonAt(comparison, 0U);
    const UmiCtestReportChange *removed = UmiCtestReportComparisonAt(comparison, 1U);
    CHECK(added != NULL && removed != NULL);
    CHECK(added->kind == UMI_CTEST_CHANGE_ADDED && !added->before_present && added->after_present);
    CHECK(removed->kind == UMI_CTEST_CHANGE_REMOVED && removed->before_present && !removed->after_present);
    CHECK(removed->before == UMI_TEST_STATE_FAILED && removed->after == UMI_TEST_STATE_NOT_RUN);
    CHECK(strcmp(added->name, "new.success") == 0 && strcmp(removed->name, "old.failure") == 0);
    UmiCtestReportComparisonDestroy(comparison);
    UmiCtestReportDestroy(after);
    UmiCtestReportDestroy(before);
    return 0;
}

static int VerifyOrderAndDetails(void)
{
    const char *left = "<testsuite tests='2'>"
        "<testcase name='z' status='fail' time='1'><failure message='First diagnosis'/></testcase>"
        "<testcase name='a' status='run' time='3'/></testsuite>";
    const char *right = "<testsuite tests='2'>"
        "<testcase name='a' status='run' time='2'/>"
        "<testcase name='z' status='fail' time='0'><failure message='Different detail'/></testcase></testsuite>";
    UmiCtestReport *before = NULL, *after = NULL;
    UmiCtestReportComparison *comparison = NULL;
    CHECK(UmiCtestReportImport(left, strlen(left), &before) == UMI_STATUS_OK);
    CHECK(UmiCtestReportImport(right, strlen(right), &after) == UMI_STATUS_OK);
    CHECK(UmiCtestReportCompare(before, after, &comparison) == UMI_STATUS_OK);
    UmiCtestReportComparisonSummary summary;
    CHECK(UmiCtestReportComparisonReadSummary(comparison, &summary) == UMI_STATUS_OK);
    CHECK(summary.unchanged == 2U && summary.changed == 0U);
    CHECK(strcmp(UmiCtestReportEntryAt(before, 0U)->name, "z") == 0);
    CHECK(strcmp(UmiCtestReportEntryAt(after, 0U)->name, "a") == 0);
    UmiCtestReportComparisonDestroy(comparison);
    UmiCtestReportDestroy(after);
    UmiCtestReportDestroy(before);
    return 0;
}

static int VerifyNamesAndOwnership(void)
{
    UmiCtestReport *before = NULL, *after = NULL;
    UmiCtestReportComparison *comparison = NULL;
    CHECK(Single("caf&#233;.[a]+&amp;save", 1U, &before) == UMI_STATUS_OK);
    CHECK(Single("caf\xC3\xA9.[a]+&amp;save", 0U, &after) == UMI_STATUS_OK);
    CHECK(UmiCtestReportCompare(before, after, &comparison) == UMI_STATUS_OK);
    UmiCtestReportDestroy(before);
    UmiCtestReportDestroy(after);
    const UmiCtestReportChange *change = UmiCtestReportComparisonAt(comparison, 0U);
    CHECK(change != NULL && change->kind == UMI_CTEST_CHANGE_RECOVERED);
    CHECK(strcmp(change->name, "caf\xC3\xA9.[a]+&save") == 0);
    CHECK(change->before == UMI_TEST_STATE_FAILED && change->after == UMI_TEST_STATE_PASSED);
    UmiCtestReportComparisonDestroy(comparison);
    return 0;
}

static int VerifyEmpty(void)
{
    const char *xml = "<testsuite tests='0'/>";
    UmiCtestReport *empty = NULL, *one = NULL;
    CHECK(UmiCtestReportImport(xml, strlen(xml), &empty) == UMI_STATUS_OK);
    CHECK(Single("retained", 0U, &one) == UMI_STATUS_OK);
    const UmiCtestReport *left[] = {empty, empty, one};
    const UmiCtestReport *right[] = {empty, one, empty};
    for (size_t index = 0U; index < 3U; ++index) {
        UmiCtestReportComparison *comparison = NULL;
        CHECK(UmiCtestReportCompare(left[index], right[index], &comparison) == UMI_STATUS_OK);
        UmiCtestReportComparisonSummary summary;
        CHECK(UmiCtestReportComparisonReadSummary(comparison, &summary) == UMI_STATUS_OK);
        CHECK(summary.changed == (index == 0U ? 0U : 1U));
        CHECK(summary.added == (index == 1U ? 1U : 0U));
        CHECK(summary.removed == (index == 2U ? 1U : 0U));
        CHECK(summary.recovered == 0U && summary.regressed == 0U);
        UmiCtestReportComparisonDestroy(comparison);
    }
    CHECK(UmiCtestReportRequirePassed(empty) == UMI_STATUS_INVALID_STATE);
    UmiCtestReportDestroy(empty);
    UmiCtestReportDestroy(one);
    return 0;
}

static int VerifyInvalid(void)
{
    UmiCtestReport *one = NULL;
    CHECK(Single("valid", 0U, &one) == UMI_STATUS_OK);
    UmiCtestReportComparison *comparison = (UmiCtestReportComparison *)(uintptr_t)1U;
    CHECK(UmiCtestReportCompare(NULL, one, &comparison) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(comparison == NULL);
    comparison = (UmiCtestReportComparison *)(uintptr_t)1U;
    CHECK(UmiCtestReportCompare(one, NULL, &comparison) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(comparison == NULL);
    CHECK(UmiCtestReportCompare(one, one, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiCtestReportComparisonSummary summary = {0};
    summary.before_tests = 17U;
    CHECK(UmiCtestReportComparisonReadSummary(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(summary.before_tests == 17U);
    CHECK(UmiCtestReportCompare(one, one, &comparison) == UMI_STATUS_OK);
    CHECK(UmiCtestReportComparisonReadSummary(comparison, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCtestReportComparisonAt(NULL, 0U) == NULL);
    CHECK(UmiCtestReportComparisonAt(comparison, (size_t)-1) == NULL);
    UmiCtestReportComparisonDestroy(comparison);
    UmiCtestReportComparisonDestroy(NULL);
    UmiCtestReportDestroy(one);
    return 0;
}

static int VerifyLarge(void)
{
    const size_t count = 25018U, capacity = count * 100U + 100U;
    char *xml = malloc(capacity);
    CHECK(xml != NULL);
    UmiCtestReport *reports[2] = {NULL, NULL};
    for (size_t side = 0U; side < 2U; ++side) {
        int written = snprintf(xml, capacity, "<testsuite tests='%zu'>", count);
        CHECK(written > 0);
        size_t used = (size_t)written;
        for (size_t index = 0U; index < count; ++index) {
            size_t name = side == 0U ? index : count - 1U - index;
            written = snprintf(xml + used, capacity - used,
                "<testcase name='case.%05zu' status='run' time='0'/>", name);
            CHECK(written > 0 && (size_t)written < capacity - used);
            used += (size_t)written;
        }
        const char closing[] = "</testsuite>";
        CHECK(sizeof closing < capacity - used);
        memcpy(xml + used, closing, sizeof closing);
        used += sizeof closing - 1U;
        CHECK(UmiCtestReportImport(xml, used, &reports[side]) == UMI_STATUS_OK);
    }
    free(xml);
    UmiCtestReportComparison *comparison = NULL;
    CHECK(UmiCtestReportCompare(reports[0], reports[1], &comparison) == UMI_STATUS_OK);
    UmiCtestReportComparisonSummary summary;
    CHECK(UmiCtestReportComparisonReadSummary(comparison, &summary) == UMI_STATUS_OK);
    CHECK(summary.before_tests == count && summary.after_tests == count);
    CHECK(summary.unchanged == count && summary.changed == 0U);
    CHECK(strcmp(UmiCtestReportEntryAt(reports[1], 0U)->name, "case.25017") == 0);
    UmiCtestReportComparisonDestroy(comparison);
    UmiCtestReportDestroy(reports[0]);
    UmiCtestReportDestroy(reports[1]);
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *transition_names[] = {"from-passed", "from-failed", "from-timeout",
        "from-skipped", "from-disabled", "from-not-run"};
    for (size_t index = 0U; index < 6U; ++index)
        if (strcmp(argv[1], transition_names[index]) == 0) return VerifyTransitions(index);
    if (strcmp(argv[1], "inventory") == 0) return VerifyInventory();
    if (strcmp(argv[1], "order-and-details") == 0) return VerifyOrderAndDetails();
    if (strcmp(argv[1], "names-and-ownership") == 0) return VerifyNamesAndOwnership();
    if (strcmp(argv[1], "empty") == 0) return VerifyEmpty();
    if (strcmp(argv[1], "invalid") == 0) return VerifyInvalid();
    if (strcmp(argv[1], "large") == 0) return VerifyLarge();
    fprintf(stderr, "Unknown scenario: %s\n", argv[1]);
    return 1;
}
