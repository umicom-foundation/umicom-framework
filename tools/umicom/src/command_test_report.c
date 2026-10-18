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

int umi_cli_command_test_report(int argc, char **argv)
{
    if (argc == 1 && (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "help") == 0)) {
        (void)puts("Usage: umicom test-report CTEST_JUNIT_FILE [--all]\n"
            "Reads saved XML only; it does not run tests or verify the current checkout.\n"
            "Default output lists non-passed tests. --all includes passed records.\n"
            "Create a fresh report with ctest --output-junit FILE --output-log LOG.\n"
            "Exit 0: saved nonempty report is all passed; 1: other outcomes; 2: input error.");
        return 0;
    }
    if (argc < 1 || argc > 2 || (argc == 2 && strcmp(argv[1], "--all") != 0)) {
        (void)fputs("Use umicom test-report CTEST_JUNIT_FILE [--all].\n", stderr);
        return 2;
    }
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
