/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_comparison_text.c
 *
 * PURPOSE:
 *   Check bounded saved-comparison text, ownership and honest outcome classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include "umicom/testing/ctest_report_comparison.h"

static UmiCtestReport *Import(const char *xml)
{
    UmiCtestReport *report = NULL;
    OK(UmiCtestReportImport(xml, strlen(xml), &report));
    return report;
}
static char *Format(const UmiCtestReportComparison *comparison, size_t offset, size_t limit)
{
    size_t required = 0;
    OK(UmiCtestReportComparisonFormat(comparison, offset, limit, NULL, 0, &required));
    char *text = malloc(required + 1U);
    CHECK(text);
    OK(UmiCtestReportComparisonFormat(comparison, offset, limit, text, required + 1U, &required));
    CHECK(strlen(text) == required);
    return text;
}
int main(int argc, char **argv)
{
    const char *cases[] = {"outcomes", "query", "short", "exact", "offset", "limit", "null",
        "unicode", "escape", "ownership", "first", "last", "empty", "unchanged-failure"};
    CHECK(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *name = argv[1];
    UmiCtestReport *before = Import("<testsuite tests='0'></testsuite>");
    UmiCtestReport *after = NULL;
    if (!strcmp(name, "outcomes")) {
        UmiCtestReportDestroy(before);
        before = Import("<testsuite tests='6'>"
            "<testcase name='removed' status='fail' time='0'><failure message='Failed'/></testcase>"
            "<testcase name='recover' status='fail' time='0'><failure message='Failed'/></testcase>"
            "<testcase name='regress' status='run' time='0'/>"
            "<testcase name='outcome' status='fail' time='0'><failure message='Failed'/></testcase>"
            "<testcase name='disabled' status='disabled' time='0'/>"
            "<testcase name='unchanged' status='fail' time='0'><failure message='Failed'/></testcase></testsuite>");
        after = Import("<testsuite tests='6'>"
            "<testcase name='added' status='run' time='0'/>"
            "<testcase name='recover' status='run' time='0'/>"
            "<testcase name='regress' status='notrun' time='0'><skipped message='SKIP_RETURN_CODE=77'/></testcase>"
            "<testcase name='outcome' status='notrun' time='0'><skipped message='SKIP_RETURN_CODE=77'/></testcase>"
            "<testcase name='disabled' status='notrun' time='0'><skipped message='SKIP_RETURN_CODE=77'/></testcase>"
            "<testcase name='unchanged' status='fail' time='0'><failure message='Failed'/></testcase></testsuite>");
    } else if (!strcmp(name, "unicode")) {
        after = Import("<testsuite tests='1'><testcase name='caf\xc3\xa9' status='run' time='0'/></testsuite>");
    } else if (!strcmp(name, "escape")) {
        /* Test names reject control characters at import. Retain the former
         * invalid fixture for review, then check that refusal separately from
         * formatting a valid name containing literal backslashes. */
#if 0
        after = Import("<testsuite tests='1'><testcase name='line&#10;slash\\tail' status='run' time='0'/></testsuite>");
#endif
        const char *invalid = "<testsuite tests='1'><testcase name='line&#10;slash\\tail' status='run' time='0'/></testsuite>";
        UmiCtestReport *rejected = NULL;
        CHECK(UmiCtestReportImport(invalid, strlen(invalid), &rejected) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(rejected == NULL);
        after = Import("<testsuite tests='1'><testcase name='line\\slash\\tail' status='run' time='0'/></testsuite>");
    } else if (!strcmp(name, "empty")) {
        after = Import("<testsuite tests='0'></testsuite>");
    } else if (!strcmp(name, "unchanged-failure")) {
        UmiCtestReportDestroy(before);
        before = Mixed();
        after = Mixed();
    } else {
        after = Many(130);
    }
    UmiCtestReportComparison *comparison = NULL;
    OK(UmiCtestReportCompare(before, after, &comparison));
    /* The rendered page must depend only on the comparison's owned records. */
    UmiCtestReportDestroy(before);
    UmiCtestReportDestroy(after);
    size_t offset = !strcmp(name, "last") ? 128U : 0;
    char *text = Format(comparison, offset, 64U);
    CHECK(strstr(text, "Removed tests are not repairs") &&
        strstr(text, "no tests were run or source identities verified"));
    if (!strcmp(name, "outcomes")) {
        CHECK(strstr(text, "Before: 6 | After: 6 | Unchanged outcomes: 1 | Changed: 6"));
        CHECK(strstr(text, "Added: 1 | Removed: 1 | Recovered: 1 | Regressed: 1 | Other outcome changes: 2"));
        CHECK(strstr(text, "[removed] removed\n  Before: failed -> After: absent"));
        CHECK(strstr(text, "[recovered] recover\n  Before: failed -> After: passed"));
        CHECK(strstr(text, "[regressed] regress\n  Before: passed -> After: skipped"));
        CHECK(strstr(text, "[outcome changed] disabled\n  Before: disabled -> After: skipped"));
    } else if (!strcmp(name, "unicode")) {
        CHECK(strstr(text, "[added] caf\xc3\xa9"));
    } else if (!strcmp(name, "escape")) {
        /* The positive formatting case must be reachable through the public
         * importer. Control-name rejection is asserted above. */
#if 0
        CHECK(strstr(text, "line\\x0Aslash\\x5Ctail") && !strstr(text, "line\nslash"));
#endif
        CHECK(strstr(text, "line\\x5Cslash\\x5Ctail") && !strstr(text, "line\\slash"));
    } else if (!strcmp(name, "empty") || !strcmp(name, "unchanged-failure")) {
        CHECK(strstr(text, "Showing 0..0 of 0") && strstr(text, "does not establish an all-passed result"));
        if (!strcmp(name, "unchanged-failure")) CHECK(strstr(text, "Unchanged outcomes: 6"));
    } else if (!strcmp(name, "last")) {
        CHECK(strstr(text, "Showing 129..130 of 130") && strstr(text, "row-000129") &&
            !strstr(text, "row-000127"));
    } else {
        CHECK(strstr(text, "Showing 1..64 of 130") && strstr(text, "row-000063") &&
            !strstr(text, "row-000064"));
    }
    size_t required = 777U;
    char untouched[16] = "unchanged";
    if (!strcmp(name, "query")) {
        OK(UmiCtestReportComparisonFormat(comparison, 0, 64, NULL, 0, &required));
        CHECK(required == strlen(text));
    } else if (!strcmp(name, "short")) {
        CHECK(UmiCtestReportComparisonFormat(comparison, 0, 64, untouched, sizeof untouched,
            &required) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!strcmp(untouched, "unchanged") && required == strlen(text));
    } else if (!strcmp(name, "exact")) {
        char *output = malloc(strlen(text) + 1U);
        CHECK(output);
        memset(output, '!', strlen(text) + 1U);
        CHECK(UmiCtestReportComparisonFormat(comparison, 0, 64, output, strlen(text), &required)
            == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(output[0] == '!' && output[strlen(text)] == '!');
        OK(UmiCtestReportComparisonFormat(comparison, 0, 64, output, strlen(text) + 1U, &required));
        CHECK(!strcmp(output, text));
        free(output);
    } else if (!strcmp(name, "offset")) {
        CHECK(UmiCtestReportComparisonFormat(comparison, 130, 64, untouched, sizeof untouched,
            &required) == UMI_STATUS_NOT_FOUND);
        CHECK(required == 777U && !strcmp(untouched, "unchanged"));
    } else if (!strcmp(name, "limit")) {
        CHECK(UmiCtestReportComparisonFormat(comparison, 0, 0, untouched, sizeof untouched,
            &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestReportComparisonFormat(comparison, 0, 129, untouched, sizeof untouched,
            &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(required == 777U && !strcmp(untouched, "unchanged"));
    } else if (!strcmp(name, "null")) {
        CHECK(UmiCtestReportComparisonFormat(NULL, 0, 64, untouched, sizeof untouched,
            &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestReportComparisonFormat(comparison, 0, 64, NULL, 1, &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestReportComparisonFormat(comparison, 0, 64, untouched, sizeof untouched, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(required == 777U && !strcmp(untouched, "unchanged"));
    }
    free(text);
    UmiCtestReportComparisonDestroy(comparison);
    return 0;
}
