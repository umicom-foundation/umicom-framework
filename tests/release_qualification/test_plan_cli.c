/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_plan_cli.c
 *
 * PURPOSE:
 *   Verify native capture and planning boundaries using isolated files and synthetic outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "fixture.h"
#include "../build_log/fixture.h"
#include "umicom/distribution/runtime/source_observation_archive.h"
#include "umicom/distribution/runtime/qualification_archive.h"
#include "umicom/platform/rooted_files.h"
int umi_cli_command_source_observation(int argc, char **argv);
int umi_cli_command_qualification_plan(int argc, char **argv);

static int WriteNew(const char *name, const void *bytes, size_t length)
{
    UmiOutputFile *file = NULL;
    REQUIRE(UmiOutputFileCreate(name, &file) == UMI_STATUS_OK);
    REQUIRE(UmiOutputFileWrite(file, bytes, length) == UMI_STATUS_OK);
    REQUIRE(UmiOutputFileClose(file) == UMI_STATUS_OK);
    UmiOutputFileDestroy(file);
    return 0;
}
/* These outcome records are test fixtures, not evidence from a compiler. The
 * adapter must still use their exact identities and leave their files intact. */
static int SaveHistory(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceObservation *baseline, const char *path,
    bool failed, const char *toolchain, UmiQualificationArchiveReceipt *receipt)
{
    UmiReleaseImpactPlan *plan = NULL;
    UmiReleaseQualification *run = NULL;
    char execution[65];
    REQUIRE(UmiSha256Buffer("fixture", 7U, execution) == UMI_STATUS_OK);
    REQUIRE(UmiReleaseImpactCreate(inventory, NULL, 0U, true, &plan) == UMI_STATUS_OK);
    REQUIRE(UmiReleaseQualificationCreate(plan, baseline, execution, toolchain, &run) == UMI_STATUS_OK);
    UmiReleaseQualificationIdentity identity;
    REQUIRE(UmiReleaseQualificationGetIdentity(run, &identity) == UMI_STATUS_OK);
    UmiReleaseQualificationResult result = QualificationResult(UMI_QUALIFICATION_PASSED);
    REQUIRE(UmiReleaseQualificationRecordBuild(run, &identity, &result) == UMI_STATUS_OK);
    REQUIRE(UmiReleaseQualificationRecordTest(run, &identity, "test.one", &result) == UMI_STATUS_OK);
    if (failed) result = QualificationResult(UMI_QUALIFICATION_FAILED);
    REQUIRE(UmiReleaseQualificationRecordTest(run, &identity, "test.two", &result) == UMI_STATUS_OK);
    REQUIRE(UmiReleaseQualificationClose(run, &identity, baseline, toolchain) == UMI_STATUS_OK);
    REQUIRE(UmiQualificationArchiveSave(run, path, receipt) == UMI_STATUS_OK);
    UmiReleaseQualificationDestroy(run);
    UmiReleaseImpactDestroy(plan);
    return 0;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    static const char *const modes[] = {"capture", "existing", "passed", "failed", "changed",
        "summary", "no-history", "unknown-tools", "wrong-baseline", "wrong-report",
        "duplicate-option", "duplicate-summary", "unknown-option", "missing-value",
        "baseline-pair", "report-pair", "relative", "no-tests", "capture-relative",
        "capture-duplicate", "capture-missing", "null-arguments"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(modes) / sizeof(modes[0]); ++i)
        if (strcmp(mode, modes[i]) == 0) known = true;
    REQUIRE(known);
    if (strcmp(mode, "null-arguments") == 0) {
        REQUIRE(umi_cli_command_source_observation(1, NULL) == 2);
        REQUIRE(umi_cli_command_qualification_plan(1, NULL) == 2);
        REQUIRE(umi_cli_command_source_observation(-1, NULL) == 2);
        REQUIRE(umi_cli_command_qualification_plan(-1, NULL) == 2);
        return 0;
    }
    char directory[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY], inventoryPath[UMI_PATH_CAPACITY];
    char baselinePath[UMI_PATH_CAPACITY], reportPath[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(source, directory, "source-\xc3\xa9.c");
    FixturePath(inventoryPath, directory, "inventory.tsv");
    FixturePath(baselinePath, directory, "baseline.tsv");
    FixturePath(reportPath, directory, "report.tsv");
    REQUIRE(WriteNew(source, "original\n", 9U) == 0);
    Fixture text;
    REQUIRE(FixtureStart(&text) && FixtureTarget(&text, "library", NULL));
    REQUIRE(FixtureSource(&text, source, "library"));
    bool noTests = strcmp(mode, "no-tests") == 0;
    if (!noTests) REQUIRE(FixtureTest(&text, "test.one", "library", NULL, false) &&
        FixtureTest(&text, "test.two", "library", NULL, false));
    REQUIRE(WriteNew(inventoryPath, text.text, text.used) == 0);
    UmiReleaseInventory *inventory = NULL;
    REQUIRE(FixtureParse(&text, &inventory) == UMI_STATUS_OK);
    char *capture[] = {"--inventory", inventoryPath, "--output", baselinePath, "--output", baselinePath};
    if (strcmp(mode, "capture-relative") == 0 || strcmp(mode, "capture-duplicate") == 0 ||
        strcmp(mode, "capture-missing") == 0) {
        if (strcmp(mode, "capture-relative") == 0) capture[3] = "baseline.tsv";
        REQUIRE(umi_cli_command_source_observation(strcmp(mode, "capture-duplicate") == 0 ? 6 :
            strcmp(mode, "capture-missing") == 0 ? 3 : 4, capture) == 2);
        UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&text); return 0;
    }
    REQUIRE(umi_cli_command_source_observation(4, capture) == 0);
    size_t baselineLength;
    unsigned char *baselineBytes = FixtureRead(baselinePath, &baselineLength);
    char baselineSha[65], toolchain[65];
    REQUIRE(UmiSha256Buffer(baselineBytes, baselineLength, baselineSha) == UMI_STATUS_OK);
    REQUIRE(UmiSha256Buffer("fixture toolchain", 17U, toolchain) == UMI_STATUS_OK);
    UmiReleaseSourceObservation *baseline = NULL;
    REQUIRE(UmiReleaseSourceObservationLoad(inventory, baselinePath, baselineSha, &baseline) == UMI_STATUS_OK);
    if (strcmp(mode, "capture") == 0 || strcmp(mode, "existing") == 0) {
        if (strcmp(mode, "existing") == 0) REQUIRE(umi_cli_command_source_observation(4, capture) == 2);
        goto unchanged;
    }
    if (noTests || strcmp(mode, "no-history") == 0) {
        char *first[] = {"--inventory", inventoryPath};
        REQUIRE(umi_cli_command_qualification_plan(2, first) == (noTests ? 2 : 0));
        goto unchanged;
    }
    UmiQualificationArchiveReceipt receipt;
    REQUIRE(SaveHistory(inventory, baseline, reportPath, strcmp(mode, "failed") == 0, toolchain, &receipt) == 0);
    if (strcmp(mode, "changed") == 0)
        REQUIRE(UmiRootedFileWrite(directory, "source-\xc3\xa9.c", "changed!\n", 9U) == UMI_STATUS_OK);
    char *arguments[18] = {"--inventory", inventoryPath, "--baseline", baselinePath,
        "--baseline-sha256", baselineSha, "--report", reportPath, "--report-sha256", receipt.reportSha256,
        "--toolchain", toolchain};
    int count = 12, expected = 0;
    if (strcmp(mode, "summary") == 0) arguments[count++] = "--summary";
    else if (strcmp(mode, "unknown-tools") == 0) count = 10;
    else if (strcmp(mode, "wrong-baseline") == 0) { baselineSha[0] = baselineSha[0] == '0' ? '1' : '0'; expected = 2; }
    else if (strcmp(mode, "wrong-report") == 0) { receipt.reportSha256[0] = receipt.reportSha256[0] == '0' ? '1' : '0'; expected = 2; }
    else if (strcmp(mode, "duplicate-option") == 0) { arguments[count++] = "--inventory"; arguments[count++] = inventoryPath; expected = 2; }
    else if (strcmp(mode, "duplicate-summary") == 0) { arguments[count++] = "--summary"; arguments[count++] = "--summary"; expected = 2; }
    else if (strcmp(mode, "unknown-option") == 0) { arguments[count++] = "--apply"; expected = 2; }
    else if (strcmp(mode, "missing-value") == 0) { arguments[count++] = "--output"; expected = 2; }
    else if (strcmp(mode, "baseline-pair") == 0) { count = 4; expected = 2; }
    else if (strcmp(mode, "report-pair") == 0) { count = 8; expected = 2; }
    else if (strcmp(mode, "relative") == 0) { arguments[1] = "inventory.tsv"; expected = 2; }
    size_t reportLength;
    unsigned char *reportBefore = FixtureRead(reportPath, &reportLength);
    REQUIRE(umi_cli_command_qualification_plan(count, arguments) == expected);
    size_t afterLength;
    unsigned char *reportAfter = FixtureRead(reportPath, &afterLength);
    REQUIRE(reportLength == afterLength && memcmp(reportBefore, reportAfter, reportLength) == 0);
    free(reportBefore); free(reportAfter);
unchanged:;
    size_t size;
    unsigned char *after = FixtureRead(baselinePath, &size);
    REQUIRE(size == baselineLength && memcmp(after, baselineBytes, size) == 0);
    free(after); free(baselineBytes);
    UmiReleaseSourceObservationDestroy(baseline);
    UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&text);
    return 0;
}
