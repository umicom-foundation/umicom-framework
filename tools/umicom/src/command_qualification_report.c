/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_qualification_report.c
 * Purpose: Display saved qualification evidence through the shared validated archive reader.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"
#include "umicom/distribution/runtime/qualification_archive.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include "umicom/distribution/runtime/qualification_compare.h"
#include <stdio.h>
#include <string.h>
static void SafeText(const char *text)
{
    /* Evidence names are data. Escape controls, non-ASCII bytes and slashes used
     * by escape notation so terminal output cannot impersonate another row. */
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p) {
        if (*p < 32U || *p >= 127U || *p == '\\') (void)printf("\\x%02x", (unsigned)*p);
        else (void)putchar((int)*p);
    }
}
int umi_cli_command_qualification_report(int argc, char **argv)
{
    if (argc == 1 && argv != NULL && argv[0] != NULL && strcmp(argv[0], "--help") == 0) {
        (void)puts("Usage: umicom qualification-report ABSOLUTE.tsv [--expect-report SHA256] [--all] [--compare BEFORE.tsv]\n"
            "Read a saved qualification report. No build, test, install or repository operation runs.\n"
            "Default: show missing and non-passing results; --all includes selected targets and passing rows.\n"
            "An expected digest must come from a separately retained receipt; it is not an author signature.\n"
            "Saved evidence does not validate current files, tools or logs. Relative paths are refused.\n"
            "--compare compares two closed historical reports; different plan identities are shown explicitly.\n"
            "Comparison also returns 1 for previously passing evidence now missing/non-passing or removed test coverage.\n"
            "Exit 0: saved scoped result ready for review; 1: saved record incomplete or blocked; 2: invalid input/read/output.");
        return fflush(stdout) == 0 && ferror(stdout) == 0 ? 0 : 2;
    }
    if (argc < 1 || argv == NULL || argv[0] == NULL || argv[0][0] == '\0') return 2;
    const char *expected = NULL, *comparePath = NULL; bool all = false;
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == NULL) return 2;
        if (strcmp(argv[i], "--all") == 0 && !all) all = true;
        else if (strcmp(argv[i], "--expect-report") == 0 && expected == NULL &&
                 i + 1 < argc && argv[i + 1] != NULL && argv[i + 1][0] != '\0') expected = argv[++i];
        else if (strcmp(argv[i], "--compare") == 0 && comparePath == NULL &&
                 i + 1 < argc && argv[i + 1] != NULL && argv[i + 1][0] != '\0') comparePath = argv[++i];
        else return 2;
    }
    UmiQualificationView *view = NULL;
    UmiQualificationArchiveReceipt receipt;
    UmiStatus status = UmiQualificationArchiveLoad(argv[0], expected, &view, &receipt);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Saved qualification refused: %s. Use qualification-report --help.\n", umi_status_text(status));
        return 2;
    }
    UmiQualificationComparison *comparison = NULL;
    if (comparePath != NULL) {
        UmiQualificationView *before = NULL; UmiQualificationArchiveReceipt prior;
        status = UmiQualificationArchiveLoad(comparePath, NULL, &before, &prior);
        if (status == UMI_STATUS_OK) status = UmiQualificationCompare(before, view, &comparison);
        UmiQualificationViewDestroy(before);
        if (status != UMI_STATUS_OK) {
            UmiQualificationViewDestroy(view);
            (void)fprintf(stderr, "Saved comparison refused: %s.\n", umi_status_text(status)); return 2;
        }
    }
    UmiQualificationViewSummary summary;
    status = UmiQualificationViewDescribe(view, &summary);
    if (status != UMI_STATUS_OK) { UmiQualificationComparisonDestroy(comparison); UmiQualificationViewDestroy(view); return 2; }
    (void)printf("%s\nHistorical evidence only; current source, tools and logs have not been validated.\n"
        "Execution: %s\nPlan: %s\nObserved source: %s\nReport SHA-256: %s\n"
        "Tests: %zu selected, %zu recorded, %zu missing, %zu passed, %zu failed, %zu skipped, "
        "%zu not-run, %zu cancelled, %zu timed-out, %zu disabled.\n",
        UmiQualificationViewStateText(view), summary.identity.execution, summary.identity.plan,
        summary.source, receipt.reportSha256, summary.results.tests, summary.results.recorded,
        summary.results.missing, summary.results.passed, summary.results.failed, summary.results.skipped,
        summary.results.notRun, summary.results.cancelled, summary.results.timedOut, summary.results.disabled);
    for (uint32_t blocker = 1U; blocker <= UMI_QUALIFICATION_EXECUTION_INTERRUPTED; blocker <<= 1U)
        if ((summary.results.blockers & blocker) != 0U)
            (void)printf("Blocker: %s\n", UmiReleaseQualificationBlockerText((UmiReleaseQualificationBlocker)blocker));
    if (summary.results.executionStatus != UMI_STATUS_OK)
        (void)printf("Execution status: %s\n", umi_status_text(summary.results.executionStatus));
    if (summary.selection.fallbackReasons != 0U)
        (void)printf("Coverage fallback reasons: 0x%x\n", (unsigned)summary.selection.fallbackReasons);
    for (size_t i = 0U; i < summary.rows; ++i) {
        const UmiQualificationViewRow *row = UmiQualificationViewAt(view, i);
        if (!all && (row->kind == UMI_QUALIFICATION_VIEW_TARGET ||
            (row->kind == UMI_QUALIFICATION_VIEW_TEST_PROCESS && !row->recorded) ||
            (row->recorded && row->result.outcome == UMI_QUALIFICATION_PASSED))) continue;
        (void)printf("%s\t", UmiQualificationViewRowStateText(row)); SafeText(row->name); (void)putchar('\n');
    }
    int result = summary.results.readyForOwnerReview ? 0 : 1;
    if (comparison != NULL) {
        UmiQualificationComparisonSummary changes;
        if (UmiQualificationComparisonDescribe(comparison, &changes) != UMI_STATUS_OK) result = 2;
        else {
            (void)printf("Comparison: %zu newly non-passing/missing, %zu recovered, %zu added, %zu removed, %zu other changes.\n",
                changes.regressed, changes.recovered, changes.added, changes.removed, changes.resultChanged);
            (void)puts(changes.sameRecordedPlan ? "Both reports describe the same recorded plan." :
                "Different recorded plans: review changed inputs and coverage before attributing a cause.");
            if (changes.sameExecution)
                (void)puts("Both files claim the same execution identity; they do not establish two independent runs.");
            if (changes.regressed != 0U || changes.removed != 0U) result = 1;
            for (size_t i = 0U; i < changes.rows; ++i) {
                const UmiQualificationDifference *row = UmiQualificationComparisonAt(comparison, i);
                if (!all && row->change == UMI_QUALIFICATION_UNCHANGED) continue;
                (void)printf("%s\t", UmiQualificationChangeText(row->change)); SafeText(row->name); (void)putchar('\n');
            }
        }
    }
    UmiQualificationComparisonDestroy(comparison);
    UmiQualificationViewDestroy(view);
    return fflush(stdout) == 0 && ferror(stdout) == 0 ? result : 2;
}
