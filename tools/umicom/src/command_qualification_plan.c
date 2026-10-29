/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_qualification_plan.c
 *
 * PURPOSE:
 *   Connect private source archives and retry planning to the native command line.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"
#include "umicom/distribution/runtime/source_observation_archive.h"
#include "umicom/distribution/runtime/qualification_retry.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include "umicom/platform/input_file.h"
#include <stdio.h>
#include <string.h>

static void SafeText(const char *text)
{
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p) {
        if (*p < 32U || *p >= 127U || *p == '\\') (void)printf("\\x%02x", (unsigned)*p);
        else (void)putchar((int)*p);
    }
}
static void SourceProblem(void *context, UmiStatus status, const char *identity)
{
    (void)context;
    (void)printf("Source observation refused: %s; input: ", umi_status_text(status));
    SafeText(identity); (void)putchar('\n');
}
static UmiStatus Inventory(const char *filename, UmiReleaseInventory **out)
{
    unsigned char *bytes = NULL;
    size_t length = 0U;
    UmiStatus status = UmiInputFileRead(filename, UMI_RELEASE_INVENTORY_TEXT_LIMIT, &bytes, &length);
    if (status == UMI_STATUS_OK)
        status = UmiReleaseInventoryParse((const char *)bytes, length, out);
    UmiInputFileFree(bytes);
    return status;
}
static UmiStatus Observe(const UmiReleaseInventory *inventory, UmiReleaseSourceObservation **out)
{
    UmiReleaseSourceOptions options = {.maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT,
        .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT, .diagnostic = SourceProblem};
    return UmiReleaseSourceObserve(inventory, &options, out);
}
static bool Help(int argc, char **argv)
{
    return argc == 1 && argv != NULL && argv[0] != NULL && strcmp(argv[0], "--help") == 0;
}
static int Finish(UmiStatus status, const char *command)
{
    if (status != UMI_STATUS_OK)
        (void)fprintf(stderr, "%s refused: %s. Use --help.\n", command, umi_status_text(status));
    return status == UMI_STATUS_OK && fflush(stdout) == 0 && ferror(stdout) == 0 ? 0 : 2;
}
int umi_cli_command_source_observation(int argc, char **argv)
{
    if (Help(argc, argv)) {
        (void)puts("Usage: umicom source-observation --inventory ABSOLUTE.tsv --output NEW-ABSOLUTE.tsv\n"
            "Capture declared current inputs and save a private historical baseline by exclusive creation.\n"
            "Choose an existing private directory; existing files are never overwritten or removed.\n"
            "Retain the printed archive SHA-256 separately. Paths can reveal private workstation details.\n"
            "Reconfigure after input membership or build configuration changes. No build or test is run.\n"
            "A baseline contains recorded hashes, not source contents or an atomic filesystem snapshot.\n"
            "Exit 0: capture/write completed; 2: invalid input, observation, write or output failure.");
        return Finish(UMI_STATUS_OK, "Source observation");
    }
    if (argc < 0 || (argc > 0 && argv == NULL)) return 2;
    const char *inventoryPath = NULL, *output = NULL;
    for (int i = 0; i < argc; ++i) {
        const char *key = argv[i];
        if (key == NULL || i + 1 >= argc || argv[i + 1] == NULL || argv[i + 1][0] == '\0') return 2;
        const char *value = argv[++i];
        if (strcmp(key, "--inventory") == 0 && inventoryPath == NULL) inventoryPath = value;
        else if (strcmp(key, "--output") == 0 && output == NULL) output = value;
        else return 2;
    }
    if (inventoryPath == NULL || output == NULL) return 2;
    UmiReleaseInventory *inventory = NULL;
    UmiReleaseSourceObservation *observation = NULL;
    UmiStatus status = Inventory(inventoryPath, &inventory);
    if (status == UMI_STATUS_OK) status = Observe(inventory, &observation);
    UmiSourceObservationReceipt receipt;
    if (status == UMI_STATUS_OK) status = UmiReleaseSourceObservationSave(observation, output, &receipt);
    if (status == UMI_STATUS_OK)
        (void)printf("Historical baseline saved: %zu bytes.\nArchive SHA-256: %s\nSource identity: %s\n"
            "Retain the receipt separately. This is an observation, not build/test success.\n",
            receipt.bytes, receipt.archiveSha256, receipt.sourceSha256);
    UmiReleaseSourceObservationDestroy(observation);
    UmiReleaseInventoryDestroy(inventory);
    return Finish(status, "Source observation");
}
int umi_cli_command_qualification_plan(int argc, char **argv)
{
    if (Help(argc, argv)) {
        (void)puts("Usage: umicom qualification-plan --inventory ABSOLUTE.tsv\n"
            "  [--baseline ABSOLUTE.tsv --baseline-sha256 SHA256]\n"
            "  [--report ABSOLUTE.tsv --report-sha256 SHA256] [--toolchain SHA256] [--summary]\n"
            "Observe current declared bytes, reopen retained evidence and derive changed work plus due tests.\n"
            "Use separately retained archive/report receipts and the current toolchain identity.\n"
            "Missing history, baseline or toolchain widens selection. Invalid evidence is an error.\n"
            "An old baseline must match the configured inventory exactly; retain mismatched evidence for review.\n"
            "No compiler, tests, install, Git operation or file write runs. This command only previews work.\n"
            "--summary omits selected rows without changing the plan. Empty selection is not a new passing result.\n"
            "Input files are absolute, bounded regular files; paths and commands inside reports are never executed.\n"
            "Exit 0: plan produced; 2: invalid input, incomplete inventory, read or output failure.");
        return Finish(UMI_STATUS_OK, "Qualification planning");
    }
    if (argc < 0 || (argc > 0 && argv == NULL)) return 2;
    const char *inventoryPath = NULL, *baselinePath = NULL, *baselineSha = NULL;
    const char *reportPath = NULL, *reportSha = NULL, *toolchain = NULL;
    bool summaryOnly = false;
    for (int i = 0; i < argc; ++i) {
        const char *key = argv[i];
        if (key == NULL) return 2;
        if (strcmp(key, "--summary") == 0 && !summaryOnly) { summaryOnly = true; continue; }
        if (i + 1 >= argc || argv[i + 1] == NULL || argv[i + 1][0] == '\0') return 2;
        const char *value = argv[++i];
        if (strcmp(key, "--inventory") == 0 && inventoryPath == NULL) inventoryPath = value;
        else if (strcmp(key, "--baseline") == 0 && baselinePath == NULL) baselinePath = value;
        else if (strcmp(key, "--baseline-sha256") == 0 && baselineSha == NULL) baselineSha = value;
        else if (strcmp(key, "--report") == 0 && reportPath == NULL) reportPath = value;
        else if (strcmp(key, "--report-sha256") == 0 && reportSha == NULL) reportSha = value;
        else if (strcmp(key, "--toolchain") == 0 && toolchain == NULL) toolchain = value;
        else return 2;
    }
    if (inventoryPath == NULL || (baselinePath == NULL) != (baselineSha == NULL) ||
        (reportPath == NULL) != (reportSha == NULL)) return 2;
    UmiReleaseInventory *inventory = NULL;
    UmiReleaseSourceObservation *baseline = NULL, *current = NULL;
    UmiReleaseImpactPlan *plan = NULL;
    UmiQualificationRetrySummary history;
    UmiStatus status = Inventory(inventoryPath, &inventory);
    if (status == UMI_STATUS_OK && baselinePath != NULL)
        status = UmiReleaseSourceObservationLoad(inventory, baselinePath, baselineSha, &baseline);
    if (status == UMI_STATUS_OK) status = Observe(inventory, &current);
    unsigned char *report = NULL;
    size_t reportLength = 0U;
    if (status == UMI_STATUS_OK && reportPath != NULL) {
        status = UmiInputFileRead(reportPath, UMI_QUALIFICATION_REPORT_LIMIT - 1U, &report, &reportLength);
        if (status == UMI_STATUS_OK)
            status = UmiReleaseQualificationPlanFromReport(inventory, baseline, current,
                (const char *)report, reportLength, reportSha, toolchain, &plan, &history);
    } else if (status == UMI_STATUS_OK)
        status = UmiReleaseQualificationPlanNext(inventory, baseline, current, NULL,
            toolchain, &plan, &history);
    UmiInputFileFree(report);
    UmiReleaseImpactSummary selection;
    if (status == UMI_STATUS_OK) status = UmiReleaseImpactDescribe(plan, &selection);
    if (status == UMI_STATUS_OK) {
        (void)printf("Selected: %zu targets; %zu tests.\n"
            "History: %zu considered passes; %zu required tests; %zu missing; %zu nonpassing; %zu removed outstanding.\n"
            "History reasons: 0x%x; observation reasons: 0x%x; graph reasons: 0x%x.\n"
            "Changes known: %s; changed inputs: %zu.\nCurrent source identity: %s\n",
            selection.targets, selection.tests, history.historicalPasses, history.requiredTests,
            history.missingResults, history.nonpassingResults, history.removedOutstandingTests,
            (unsigned)history.historyReasons, (unsigned)history.observation.fallbackReasons,
            (unsigned)selection.fallbackReasons, history.observation.changesKnown ? "yes" : "no",
            history.observation.changedFiles, UmiReleaseSourceObservationFingerprint(current));
        if (!summaryOnly) for (size_t i = 0U; i < UmiReleaseImpactCount(plan); ++i) {
            const UmiReleaseImpactItem *item = UmiReleaseImpactAt(plan, i);
            (void)fputs(item->kind == UMI_RELEASE_INVENTORY_TEST ? "test\t" : "target\t", stdout);
            SafeText(item->name); (void)printf("\t0x%x\n", (unsigned)item->reasons);
        }
        (void)puts("PREVIEW ONLY. No build or test executed; no evidence accepted for publication.");
        if (selection.noWork)
            (void)puts("No selected work under this contract. This is not a new passing execution.");
    }
    UmiReleaseImpactDestroy(plan);
    UmiReleaseSourceObservationDestroy(current);
    UmiReleaseSourceObservationDestroy(baseline);
    UmiReleaseInventoryDestroy(inventory);
    return Finish(status, "Qualification planning");
}
