/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_file_identity.c
 *
 * PURPOSE:
 *   Check that accepted byte identities belong to parsed input and never leak from rejected reads.
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
#include "umicom/testing/ctest_report_file.h"

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
    const char *cases[] = {"known", "empty-suite", "line-endings", "malformed", "empty-file",
        "missing", "relative", "null-output", "null-identity", "ownership", "unicode"};
    CHECK(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *name = argv[1];
    /* Independent fixture digests cover raw UTF-8 bytes, not reconstructed XML. */
    const char *xml = "<testsuite tests='1'><testcase name='notes' status='run' time='0'/></testsuite>";
    const char *expected = "52a616306fa80189deeb4100d9ed96827c90da4937ea56f1056b63e0cfd854ff";
    if (!strcmp(name, "empty-suite")) {
        xml = "<testsuite tests='0'></testsuite>";
        expected = "9ebc712562a6b9b5fca2777ce9d7c3ff17a8b9dd0daaa03ce3d1c57d704cf5f6";
    } else if (!strcmp(name, "line-endings")) {
        xml = "<testsuite tests='1'>\r\n<testcase name='notes' status='run' time='0'/>\r\n</testsuite>";
        expected = "2382d58a0a25e789b4e96c17e155a735dba14e5cd4bd4080650f8f767cfcfe2c";
    } else if (!strcmp(name, "unicode")) {
        xml = "<testsuite tests='1'><testcase name='caf\xc3\xa9' status='run' time='0'/></testsuite>";
        expected = "9a573f1edb52ab4385284930f8a6ca61d09ba090825bb3a84350290a19179905";
    }
    char directory[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(file, directory, "report.xml");
    if (strcmp(name, "missing"))
        Write(file, !strcmp(name, "empty-file") ? "" : !strcmp(name, "malformed") ? "<testsuite>" : xml);
    UmiCtestReportFileIdentity identity;
    memset(&identity, 0xa5, sizeof identity);
    unsigned char original[sizeof identity];
    memcpy(original, &identity, sizeof original);
    UmiCtestReport *report = NULL;
    if (!strcmp(name, "null-output")) {
        CHECK(UmiCtestReportReadFileWithIdentity(file, NULL, &identity) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(original, &identity, sizeof identity));
        return 0;
    }
    UmiStatus status = UmiCtestReportReadFileWithIdentity(
        !strcmp(name, "relative") ? "report.xml" : file, &report,
        !strcmp(name, "null-identity") ? NULL : &identity);
    if (!strcmp(name, "empty-file") || !strcmp(name, "malformed")) {
        CHECK(status == UMI_STATUS_PARSE_ERROR && !report);
        CHECK(!memcmp(original, &identity, sizeof identity));
    } else if (!strcmp(name, "missing") || !strcmp(name, "relative")) {
        CHECK(status != UMI_STATUS_OK && !report);
        if (!strcmp(name, "relative")) CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(original, &identity, sizeof identity));
    } else {
        OK(status);
        UmiCtestReportSummary summary;
        OK(UmiCtestReportReadSummary(report, &summary));
        CHECK(summary.tests == (!strcmp(name, "empty-suite") ? 0U : 1U));
        UmiCtestReportDestroy(report);
        report = NULL;
        if (!strcmp(name, "ownership")) memset(file, 'x', 5U);
        if (!strcmp(name, "null-identity"))
            CHECK(!memcmp(original, &identity, sizeof identity));
        else CHECK(identity.bytes == strlen(xml) && !strcmp(identity.sha256, expected));
    }
    UmiCtestReportDestroy(report);
    return 0;
}
