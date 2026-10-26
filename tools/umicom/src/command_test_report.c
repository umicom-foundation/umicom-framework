/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_test_report.c
 * PURPOSE: Present Framework-owned saved CTest report results without execution.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"
#include "umicom/platform/input_file.h"
#include "umicom/testing/ctest_report.h"
#include "umicom/testing/ctest_report_file.h"
#include "umicom/testing/ctest_report_comparison.h"
#include "umicom/testing/ctest_coverage_manifest.h"
#include "umicom/base/csv_document.h"
#include <stdio.h>
#include <string.h>

static const char *ReportOutcome(const UmiCtestReportEntry *entry)
{
    if (entry->disabled) return "disabled";
    switch (entry->result.state) {
        case UMI_TEST_STATE_PASSED: return "passed";
        case UMI_TEST_STATE_FAILED: return "failed";
        case UMI_TEST_STATE_TIMED_OUT: return "timed out";
        case UMI_TEST_STATE_SKIPPED: return "skipped";
        case UMI_TEST_STATE_NOT_RUN: return "not run";
        default: return "unknown";
    }
}

/* A report is external input. Keep control bytes from changing terminal state
 * or injecting extra result lines, while retaining printable UTF-8 names. */
static void ReportText(const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    while (*cursor != 0U) {
        unsigned char value = *cursor++;
        if (value < 32U || value == 127U) (void)printf("\\x%02X", (unsigned)value);
        else (void)putchar((int)value);
    }
}


/* Each input is resolved before its bounded read and shared XML import.
 * The CLI owns paths; the Framework comparison service only owns outcomes. */
/* Bounded file admission now belongs to Framework so command-line and native
 * hosts share the same parser boundary. Retain the former composition for review. */
#if 0
static UmiStatus ReportReadSaved(const char *file, UmiCtestReport **out)
{
    *out = NULL;
    char current[UMI_PATH_CAPACITY], absolute[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_is_absolute(file)
        ? umi_path_copy(absolute, sizeof absolute, file)
        : umi_fs_current_directory(current, sizeof current);
    if (status == UMI_STATUS_OK && !umi_path_is_absolute(file))
        status = umi_path_absolute(file, current, absolute, sizeof absolute);
    unsigned char *bytes = NULL;
    size_t size = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(absolute, UMI_CTEST_COLLECTION_MAX_BYTES, &bytes, &size);
    if (status == UMI_STATUS_OK) status = UmiCtestReportImport((const char *)bytes, size, out);
    UmiInputFileFree(bytes);
    return status;
}

#endif
static UmiStatus ReportReadSaved(const char *file, UmiCtestReport **out)
{
    *out = NULL;
    char current[UMI_PATH_CAPACITY], absolute[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_is_absolute(file)
        ? umi_path_copy(absolute, sizeof absolute, file)
        : umi_fs_current_directory(current, sizeof current);
    if (status == UMI_STATUS_OK && !umi_path_is_absolute(file))
        status = umi_path_absolute(file, current, absolute, sizeof absolute);
    if (status == UMI_STATUS_OK) status = UmiCtestReportReadFile(absolute, out);
    return status;
}

static const char *ReportChangeKind(UmiCtestReportChangeKind kind)
{
    switch (kind) {
        case UMI_CTEST_CHANGE_ADDED: return "added";
        case UMI_CTEST_CHANGE_REMOVED: return "removed";
        case UMI_CTEST_CHANGE_RECOVERED: return "recovered";
        case UMI_CTEST_CHANGE_REGRESSED: return "regressed";
        case UMI_CTEST_CHANGE_OUTCOME: return "outcome changed";
        default: return "unknown";
    }
}

static int ReportCompareFiles(const char *before_file, const char *after_file)
{
    UmiCtestReport *before = NULL, *after = NULL;
    UmiCtestReportComparison *comparison = NULL;
    UmiStatus status = ReportReadSaved(before_file, &before);
    if (status == UMI_STATUS_OK) status = ReportReadSaved(after_file, &after);
    if (status == UMI_STATUS_OK) status = UmiCtestReportCompare(before, after, &comparison);
    int exit_code = 2;
    if (status == UMI_STATUS_OK) {
        UmiCtestReportComparisonSummary summary;
        status = UmiCtestReportComparisonReadSummary(comparison, &summary);
        if (status == UMI_STATUS_OK) {
            puts("Saved CTest comparison; this command did not run tests or verify source revisions.");
            printf("Before: %zu | After: %zu | Unchanged outcomes: %zu\n"
                   "Added: %zu | Removed: %zu | Recovered: %zu | Regressed: %zu | Other outcome changes: %zu\n",
                summary.before_tests, summary.after_tests, summary.unchanged, summary.added,
                summary.removed, summary.recovered, summary.regressed, summary.outcome_changed);
            for (size_t index = 0U; index < summary.changed; ++index) {
                const UmiCtestReportChange *change = UmiCtestReportComparisonAt(comparison, index);
                UmiCtestReportEntry previous = {0}, current = {0};
                previous.result.state = change->before; previous.disabled = change->before_disabled;
                current.result.state = change->after; current.disabled = change->after_disabled;
                printf("[%s] ", ReportChangeKind(change->kind));
                ReportText(change->name);
                printf(" : %s -> %s\n", change->before_present ? ReportOutcome(&previous) : "absent",
                    change->after_present ? ReportOutcome(&current) : "absent");
            }
            if (summary.added != 0U || summary.removed != 0U)
                puts("Test inventory changed. Removed tests are not verified repairs.");
            bool passed = UmiCtestReportRequirePassed(after) == UMI_STATUS_OK;
            puts(passed ? "The current saved report contains only passed tests."
                        : "The current saved report is empty or contains non-passed tests.");
            exit_code = passed && summary.added == 0U && summary.removed == 0U ? 0 : 1;
        }
    }
    if (status != UMI_STATUS_OK)
        fprintf(stderr, "Cannot compare saved CTest reports (status %d). Supply two complete JUnit XML files.\n", (int)status);
    UmiCtestReportComparisonDestroy(comparison);
    UmiCtestReportDestroy(after);
    UmiCtestReportDestroy(before);
    return exit_code;
}


/* Framework owns manifest grammar, file admission and host/source policy. The
 * command resolves one user path and presents the immutable review. */
static int ReportCoverageFiles(const char *identity, const char *manifest_file)
{
    char current[UMI_PATH_CAPACITY], absolute[UMI_PATH_CAPACITY], directory[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_is_absolute(manifest_file)
        ? umi_path_copy(absolute, sizeof absolute, manifest_file)
        : umi_fs_current_directory(current, sizeof current);
    if (status == UMI_STATUS_OK && !umi_path_is_absolute(manifest_file))
        status = umi_path_absolute(manifest_file, current, absolute, sizeof absolute);
    if (status == UMI_STATUS_OK) status = umi_path_parent(absolute, directory, sizeof directory);
    unsigned char *bytes = NULL;
    size_t length = 0;
    UmiCtestCoverageManifest *manifest = NULL;
    UmiCtestCoverageReview *review = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(absolute, UMI_CSV_MAX_BYTES, &bytes, &length);
    if (status == UMI_STATUS_OK) status = UmiCtestCoverageManifestImport(bytes, length, &manifest);
    UmiInputFileFree(bytes);
    if (status == UMI_STATUS_OK)
        status = UmiCtestCoverageManifestReviewFiles(manifest, directory, identity, &review);
    UmiCtestCoverageSummary summary;
    if (status == UMI_STATUS_OK) status = UmiCtestCoverageReviewRead(review, &summary);
    int exit_code = 2;
    if (status == UMI_STATUS_OK) {
        puts("Saved host coverage; this command did not run tests or verify source revisions.");
        fputs("Requested source identity: ", stdout); ReportText(UmiCtestCoverageReviewSourceIdentity(review));
        printf("\nRequirements: %zu | Passed: %zu | Outstanding: %zu\n",
            summary.requirements, summary.outcomes[UMI_CTEST_COVERAGE_PASSED], summary.outstanding);
        for (size_t i = 0; i < summary.requirements; ++i) {
            const UmiCtestCoverageRow *row = UmiCtestCoverageReviewAt(review, i);
            printf("[%s] ", UmiCtestCoverageOutcomeText(row->outcome));
            ReportText(row->requirement.host); fputs(" / ", stdout);
            ReportText(row->requirement.configuration); fputs(" : ", stdout);
            ReportText(row->requirement.name);
            if (row->requirement.capability[0]) {
                fputs(" | requires ", stdout); ReportText(row->requirement.capability);
            }
            if (row->outcome == UMI_CTEST_COVERAGE_SOURCE_MISMATCH) {
                fputs(" | observed source ", stdout); ReportText(row->observed_source_identity);
            }
            putchar('\n');
        }
        puts("Only listed requirements are reviewed. Unlisted tests and hosts are outside this gate.");
        if (!summary.requirements) puts("No requirements: empty coverage cannot pass.");
        exit_code = UmiCtestCoverageRequirePassed(review) == UMI_STATUS_OK ? 0 : 1;
    } else {
        fprintf(stderr, "Cannot review saved host coverage (status %d). Check the seven-column "
            "CSV manifest, unique hosts/requirements, relative report paths and complete JUnit XML.\n",
            (int)status);
    }
    UmiCtestCoverageReviewDestroy(review);
    UmiCtestCoverageManifestDestroy(manifest);
    return exit_code;
}

int umi_cli_command_test_report(int argc, char **argv)
{
    if (argc == 1 && (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "help") == 0)) {
        (void)puts("Usage: umicom test-report CTEST_JUNIT_FILE [--all]\n"
            "Reads saved XML only; it does not run tests or verify the current checkout.\n"
            "Default output lists non-passed tests. --all includes passed records.\n"
            "Create a fresh report with ctest --output-junit FILE --output-log LOG.\n"
            "Exit 0: saved nonempty report is all passed; 1: other outcomes; 2: input error.");
        (void)puts("Comparison: umicom test-report --compare BEFORE_XML AFTER_XML\n"
            "Comparison exit 0 requires unchanged test inventory and a nonempty all-passed current report.\n"
            "Removed, skipped and disabled tests are not repaired tests.");
        (void)puts("Coverage: umicom test-report --coverage SOURCE_ID MANIFEST.csv\n"
            "CSV paths are relative to the manifest directory. No files are written.\n"
            "Coverage exit 0 requires nonempty listed requirements all passed on the exact host/configuration/source.\n"
            "Exit 1: outstanding or empty requirements; 2: manifest or report input error.");
        return 0;
    }
    if (argc == 3 && strcmp(argv[0], "--coverage") == 0)
        return ReportCoverageFiles(argv[1], argv[2]);
    if (argc == 3 && strcmp(argv[0], "--compare") == 0)
        return ReportCompareFiles(argv[1], argv[2]);
    if (argc < 1 || argc > 2 || (argc == 2 && strcmp(argv[1], "--all") != 0)) {
        (void)fputs("Use umicom test-report CTEST_JUNIT_FILE [--all].\n", stderr);
        return 2;
    }
    /* The shared bounded reader now serves both command variants. Retain the
     * former inline admission for engineering review of compatible behaviour. */
#if 0
    char current[UMI_PATH_CAPACITY], absolute[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (umi_path_is_absolute(argv[0])) status = umi_path_copy(absolute, sizeof absolute, argv[0]);
    else {
        status = umi_fs_current_directory(current, sizeof current);
        if (status == UMI_STATUS_OK)
            status = umi_path_absolute(argv[0], current, absolute, sizeof absolute);
    }
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiCtestReport *report = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(absolute, UMI_CTEST_COLLECTION_MAX_BYTES, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiCtestReportImport((const char *)bytes, size, &report);
    UmiInputFileFree(bytes);
#endif
    UmiCtestReport *report = NULL;
    UmiStatus status = ReportReadSaved(argv[0], &report);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot inspect saved CTest report (status %d). "
            "Use a complete CTest JUnit XML file, not a console transcript. "
            "Check the path, byte limit, format and suite counts.\n", (int)status);
        return 2;
    }
    UmiCtestReportSummary summary;
    status = UmiCtestReportReadSummary(report, &summary);
    if (status != UMI_STATUS_OK) {
        UmiCtestReportDestroy(report);
        return 2;
    }
    (void)puts("Saved CTest report; this command did not run tests or verify source revisions.");
    (void)printf("Tests: %zu | Passed: %zu | Failed: %zu | Timed out: %zu\n"
        "Skipped: %zu | Disabled: %zu | Not run: %zu\n",
        summary.tests, summary.passed, summary.failed, summary.timed_out,
        summary.skipped, summary.disabled, summary.not_run);
    for (size_t index = 0U; index < summary.tests; ++index) {
        const UmiCtestReportEntry *entry = UmiCtestReportEntryAt(report, index);
        if (argc == 1 && entry->result.state == UMI_TEST_STATE_PASSED) continue;
        (void)printf("[%s] ", ReportOutcome(entry));
        ReportText(entry->name);
        if (entry->result.message[0] != '\0') {
            (void)fputs(" : ", stdout);
            ReportText(entry->result.message);
        }
        (void)putchar('\n');
    }
    int result = UmiCtestReportRequirePassed(report) == UMI_STATUS_OK ? 0 : 1;
    if (summary.tests == 0U) (void)puts("No test records: an empty report is not an all-passed result.");
    UmiCtestReportDestroy(report);
    return result;
}
