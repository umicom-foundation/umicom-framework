/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_coverage_files.c
 *
 * PURPOSE:
 *   Qualify explicit host report loading, input refusal and independent ownership.
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
#include "umicom/testing/ctest_coverage_manifest.h"

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
    const char *names[] = {"passed", "skipped", "missing", "malformed", "empty-report", "stale",
        "missing-host", "missing-test", "configuration", "ownership", "relative-base", "null", "empty-plan",
        "second-invalid", "unicode", "subdirectory"};
    CHECK(argc == 2 && Known(argv[1], names, sizeof names / sizeof names[0]));
    const char *name = argv[1];
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], child[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    const char *leaf = !strcmp(name, "unicode") ? "r\xc3\xa9sults.xml" : "report.xml";
    const char *xml = !strcmp(name, "skipped")
        ? "<testsuite tests='1'><testcase name='z.pass' status='notrun' time='0'><skipped message='SKIP_RETURN_CODE=77'/></testcase></testsuite>"
        : "<testsuite tests='1'><testcase name='z.pass' status='run' time='1'/></testsuite>";
    if (!strcmp(name, "malformed")) xml = "<testsuite>";
    if (!strcmp(name, "empty-report")) xml = "";
    const char *report_directory = directory;
    if (!strcmp(name, "subdirectory")) {
        FixturePath(child, directory, "nested");
#ifdef _WIN32
        wchar_t wide[UMI_PATH_CAPACITY];
        CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, child, -1, wide, (int)UMI_PATH_CAPACITY) != 0);
        CHECK(CreateDirectoryW(wide, NULL));
#else
        CHECK(mkdir(child, 0700) == 0);
#endif
        report_directory = child;
    }
    FixturePath(path, report_directory, leaf);
    if (strcmp(name, "missing")) Write(path, xml);
    char relative[UMI_PATH_CAPACITY];
    if (!strcmp(name, "subdirectory")) {
        const char *last = strrchr(child, '/');
        CHECK(last && snprintf(relative, sizeof relative, "%s\\%s", last + 1, leaf) > 0);
    } else strcpy(relative, leaf);
    char csv[UMI_PATH_CAPACITY + 512U];
    int length = snprintf(csv, sizeof csv,
        "kind,host,configuration,source_identity,test,capability,report\n"
        "host,windows,Debug,source-A,,,%s\n"
        "require,%s,%s,,%s,GTK display,\n%s",
        relative, !strcmp(name, "missing-host") ? "linux" : "windows",
        !strcmp(name, "configuration") ? "Release" : "Debug",
        !strcmp(name, "missing-test") ? "other" : "z.pass",
        !strcmp(name, "second-invalid") ? "host,linux,Debug,source-A,,,absent.xml\n" : "");
    CHECK(length > 0 && (size_t)length < sizeof csv);
    if (!strcmp(name, "empty-plan")) {
        strcpy(csv, "kind,host,configuration,source_identity,test,capability,report\n");
        length = (int)strlen(csv);
    }
    UmiCtestCoverageManifest *manifest = NULL;
    OK(UmiCtestCoverageManifestImport(csv, (size_t)length, &manifest));
    UmiCtestCoverageReview *review = NULL;
    if (!strcmp(name, "null")) {
        CHECK(UmiCtestCoverageManifestReviewFiles(NULL, directory, "source-A", &review) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestCoverageManifestReviewFiles(manifest, NULL, "source-A", &review) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestCoverageManifestReviewFiles(manifest, directory, NULL, &review) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestCoverageManifestReviewFiles(manifest, directory, "source-A", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!review);
    } else {
        UmiStatus status = UmiCtestCoverageManifestReviewFiles(manifest,
            !strcmp(name, "relative-base") ? "." : directory,
            !strcmp(name, "stale") ? "source-B" : "source-A", &review);
        bool failure = !strcmp(name, "relative-base") || !strcmp(name, "missing") ||
            !strcmp(name, "second-invalid") || !strcmp(name, "malformed") || !strcmp(name, "empty-report");
        if (failure) CHECK(status != UMI_STATUS_OK && !review);
        else {
            OK(status);
            UmiCtestCoverageManifestDestroy(manifest); manifest = NULL;
            UmiCtestCoverageSummary summary;
            OK(UmiCtestCoverageReviewRead(review, &summary));
            if (!strcmp(name, "empty-plan")) CHECK(summary.requirements == 0);
            else {
                UmiCtestCoverageOutcome outcome = !strcmp(name, "skipped") ? UMI_CTEST_COVERAGE_SKIPPED :
                    !strcmp(name, "stale") ? UMI_CTEST_COVERAGE_SOURCE_MISMATCH :
                    (!strcmp(name, "missing-host") || !strcmp(name, "configuration")) ? UMI_CTEST_COVERAGE_MISSING_HOST :
                    !strcmp(name, "missing-test") ? UMI_CTEST_COVERAGE_MISSING_TEST : UMI_CTEST_COVERAGE_PASSED;
                CHECK(summary.requirements == 1 && summary.outcomes[outcome] == 1);
                CHECK(UmiCtestCoverageReviewAt(review, 0)->outcome == outcome);
                CHECK(strcmp(UmiCtestCoverageOutcomeText(outcome), "unknown") != 0);
            }
            CHECK(UmiCtestCoverageRequirePassed(review) ==
                (summary.requirements && !summary.outstanding ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE));
        }
    }
    if (strcmp(name, "missing")) {
        size_t size = 0; unsigned char *bytes = FixtureRead(path, &size);
        CHECK(size == strlen(xml) && !memcmp(bytes, xml, size)); free(bytes);
    }
    UmiCtestCoverageReviewDestroy(review);
    UmiCtestCoverageManifestDestroy(manifest);
    return 0;
}
