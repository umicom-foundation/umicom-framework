/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_comparison_file.c
 *
 * PURPOSE:
 *   Verify both file selections are admitted and independently owned before returning a saved comparison.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../build_log/fixture.h"
#undef CHECK
#include "fixture.h"
#include "umicom/testing/ctest_comparison_file.h"

static void Write(const char *path, const char *text)
{
    UmiOutputFile *file = NULL;
    OK(UmiOutputFileCreate(path, &file));
    OK(UmiOutputFileWrite(file, text, strlen(text)));
    OK(UmiOutputFileClose(file));
    UmiOutputFileDestroy(file);
}
int main(int argc, char **argv)
{
    const char *cases[] = {"valid", "same", "empty", "malformed-before", "malformed-after",
        "missing-before", "missing-after", "relative-before", "relative-after", "null", "ownership"};
    CHECK(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *name = argv[1];
    char directory[UMI_PATH_CAPACITY], before[UMI_PATH_CAPACITY], after[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(before, directory, "before.xml");
    FixturePath(after, directory, "after.xml");
    const char *first = "<testsuite tests='1'><testcase name='notes' status='fail' time='0'><failure message='Failed'/></testcase></testsuite>";
    const char *second = "<testsuite tests='1'><testcase name='notes' status='run' time='0'/></testsuite>";
    if (!strcmp(name, "empty")) first = second = "<testsuite tests='0'></testsuite>";
    if (strcmp(name, "missing-before")) Write(before, !strcmp(name, "malformed-before") ? "<testsuite>" : first);
    if (strcmp(name, "missing-after")) Write(after, !strcmp(name, "malformed-after") ? "<testsuite>" : second);
    UmiCtestReportComparison *comparison = NULL;
    if (!strcmp(name, "null")) {
        CHECK(UmiCtestReportCompareFiles(NULL, after, &comparison) == UMI_STATUS_INVALID_ARGUMENT && !comparison);
        CHECK(UmiCtestReportCompareFiles(before, NULL, &comparison) == UMI_STATUS_INVALID_ARGUMENT && !comparison);
        CHECK(UmiCtestReportCompareFiles(before, after, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    UmiStatus status = UmiCtestReportCompareFiles(
        !strcmp(name, "relative-before") ? "before.xml" : before,
        !strcmp(name, "relative-after") ? "after.xml" : !strcmp(name, "same") ? before : after,
        &comparison);
    if (strstr(name, "relative")) CHECK(status == UMI_STATUS_INVALID_ARGUMENT && !comparison);
    else if (strstr(name, "malformed")) CHECK(status == UMI_STATUS_PARSE_ERROR && !comparison);
    else if (strstr(name, "missing")) CHECK(status != UMI_STATUS_OK && !comparison);
    else {
        OK(status);
        if (!strcmp(name, "ownership")) { memset(before, 'x', 5); memset(after, 'y', 5); }
        UmiCtestReportComparisonSummary summary;
        OK(UmiCtestReportComparisonReadSummary(comparison, &summary));
        if (!strcmp(name, "empty")) CHECK(!summary.before_tests && !summary.after_tests && !summary.changed);
        else if (!strcmp(name, "same")) CHECK(summary.unchanged == 1 && !summary.recovered && !summary.changed);
        else {
            CHECK(summary.recovered == 1 && summary.changed == 1);
            CHECK(!strcmp(UmiCtestReportComparisonAt(comparison, 0)->name, "notes"));
        }
    }
    UmiCtestReportComparisonDestroy(comparison);
    return 0;
}
