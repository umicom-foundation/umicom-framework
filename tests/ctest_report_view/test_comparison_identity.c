/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_comparison_identity.c
 *
 * PURPOSE:
 *   Check atomic identity publication for two independently admitted saved reports.
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
    const char *cases[] = {"known", "same", "semantic", "malformed-before", "malformed-after",
        "missing-before", "missing-after", "relative-before", "relative-after",
        "null-before", "null-after", "null-output", "null-identity", "ownership"};
    CHECK(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *name = argv[1];
    const char *first = "<testsuite tests='1'><testcase name='notes' status='run' time='0'/></testsuite>";
    const char *second = "<testsuite tests='1'>\r\n<testcase name='notes' status='run' time='0'/>\r\n</testsuite>";
    char directory[UMI_PATH_CAPACITY], before[UMI_PATH_CAPACITY], after[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(before, directory, "before.xml"); FixturePath(after, directory, "after.xml");
    if (strcmp(name, "missing-before"))
        Write(before, !strcmp(name, "malformed-before") ? "<testsuite>" : first);
    if (strcmp(name, "missing-after"))
        Write(after, !strcmp(name, "malformed-after") ? "<testsuite>" : second);
    UmiCtestReportComparisonIdentity identity;
    memset(&identity, 0xa5, sizeof identity);
    unsigned char original[sizeof identity]; memcpy(original, &identity, sizeof original);
    UmiCtestReportComparison *comparison = NULL;
    const char *left = !strcmp(name, "relative-before") ? "before.xml" : !strcmp(name, "null-before") ? NULL : before;
    const char *right = !strcmp(name, "relative-after") ? "after.xml" :
        !strcmp(name, "null-after") ? NULL : !strcmp(name, "same") ? before : after;
    UmiStatus status = UmiCtestReportCompareFilesWithIdentity(left, right,
        !strcmp(name, "null-output") ? NULL : &comparison,
        !strcmp(name, "null-identity") ? NULL : &identity);
    if (strstr(name, "malformed") || strstr(name, "missing") || strstr(name, "relative") ||
        !strcmp(name, "null-before") || !strcmp(name, "null-after") || !strcmp(name, "null-output")) {
        CHECK(status != UMI_STATUS_OK && !comparison);
        if (strstr(name, "malformed")) CHECK(status == UMI_STATUS_PARSE_ERROR);
        if (strstr(name, "relative") || !strncmp(name, "null-", 5U)) CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(original, &identity, sizeof identity));
    } else {
        OK(status);
        UmiCtestReportComparisonSummary summary;
        OK(UmiCtestReportComparisonReadSummary(comparison, &summary));
        CHECK(summary.unchanged == 1U && !summary.changed);
        UmiCtestReportComparisonDestroy(comparison); comparison = NULL;
        if (!strcmp(name, "ownership")) { memset(before, 'x', 5U); memset(after, 'y', 5U); }
        if (!strcmp(name, "null-identity")) CHECK(!memcmp(original, &identity, sizeof identity));
        else {
            CHECK(identity.before.bytes == strlen(first));
            CHECK(!strcmp(identity.before.sha256, "52a616306fa80189deeb4100d9ed96827c90da4937ea56f1056b63e0cfd854ff"));
            CHECK(identity.after.bytes == strlen(!strcmp(name, "same") ? first : second));
            CHECK(!strcmp(identity.after.sha256, !strcmp(name, "same") ? "52a616306fa80189deeb4100d9ed96827c90da4937ea56f1056b63e0cfd854ff" : "2382d58a0a25e789b4e96c17e155a735dba14e5cd4bd4080650f8f767cfcfe2c"));
            if (!strcmp(name, "semantic")) CHECK(strcmp(identity.before.sha256, identity.after.sha256));
        }
    }
    UmiCtestReportComparisonDestroy(comparison);
    return 0;
}
