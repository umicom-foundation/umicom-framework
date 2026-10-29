/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/observed_impact/main.c
 *
 * PURPOSE:
 *   Demonstrate source-driven build selection using installed public contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <umicom/distribution/runtime/observed_impact.h>
#include <umicom/distribution/runtime/qualification_retry.h>
#include <umicom/distribution/runtime/qualification_report.h>
#include <umicom/distribution/runtime/source_observation_archive.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* This deliberately small inventory is synthetic: /demo is an identity,
 * never a directory opened by the example. Hex fields preserve exact strings.
 * The graph is core -> view -> view.check, with other -> other.check separate.
 * Real hosts obtain inventories from configured project metadata. */
static const char inventory_text[] =
    "UMICOM-RELEASE-INVENTORY\t1\n"
    "context\tcmake\t0123456789abcdef0123456789abcdef\t2f64656d6f\t2f6275696c64\t4465627567\n"
    "target\t636f7265\t2f64656d6f\tconfigured\t696d706163742d76657273696f6e3d310a636f6d706c6574653d310a696d706163742d656e640a\n"
    "target\t76696577\t2f64656d6f\tconfigured\t696d706163742d76657273696f6e3d310a636f6d706c6574653d310a646570656e64656e63793d36333666373236350a696d706163742d656e640a\n"
    "target\t6f74686572\t2f64656d6f\tconfigured\t696d706163742d76657273696f6e3d310a636f6d706c6574653d310a696d706163742d656e640a\n"
    "source\t2f64656d6f2f636f72652e63\t636f7265\tpresent\t7368613235363d73796e746865746963\n"
    "source\t2f64656d6f2f6f746865722e63\t6f74686572\tpresent\t7368613235363d73796e746865746963\n"
    "test\t766965772e636865636b\t2f64656d6f\tregistered\t696d706163742d76657273696f6e3d310a636f6d706c6574653d310a7461726765743d37363639363537370a696d706163742d656e640a\n"
    "test\t6f746865722e636865636b\t2f64656d6f\tregistered\t696d706163742d76657273696f6e3d310a636f6d706c6574653d310a7461726765743d366637343638363537320a696d706163742d656e640a\n";

typedef struct ExampleSource { bool edited; } ExampleSource;
static UmiStatus ReadSource(void *context, const char *path, size_t maximumBytes,
    size_t *outSize, char outSha256[65])
{
    const ExampleSource *source = context;
    const char *bytes = source->edited && strcmp(path, "/demo/core.c") == 0 ?
        "int answer(void) { return 43; }\n" : "int answer(void) { return 42; }\n";
    size_t size = strlen(bytes);
    if (size > maximumBytes) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiSha256Buffer(bytes, size, outSha256);
    if (status == UMI_STATUS_OK) *outSize = size;
    return status;
}

/* Retain the source-only example for review. The active entry also shows
 * explicitly required retries using the same installed Framework service. */
#if 0
int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "same") != 0 &&
        strcmp(argv[1], "source") != 0 && strcmp(argv[1], "first") != 0)) {
        fprintf(stderr, "Usage: umicom-observed-impact <same|source|first>\n");
        return 2;
    }
    UmiReleaseInventory *inventory = NULL;
    UmiReleaseSourceObservation *baseline = NULL, *current = NULL;
    UmiReleaseImpactPlan *plan = NULL;
    UmiObservedImpactSummary observed;
    UmiReleaseImpactSummary selection;
    ExampleSource source = {false};
    UmiReleaseSourceOptions options = {
        .maximumFileBytes = 1024U, .maximumTotalBytes = 4096U,
        .read = ReadSource, .context = &source
    };
    UmiStatus status = UmiReleaseInventoryParse(inventory_text,
        sizeof(inventory_text) - 1U, &inventory);
    if (status == UMI_STATUS_OK && strcmp(argv[1], "first") != 0)
        status = UmiReleaseSourceObserve(inventory, &options, &baseline);
    source.edited = strcmp(argv[1], "source") == 0;
    if (status == UMI_STATUS_OK)
        status = UmiReleaseSourceObserve(inventory, &options, &current);
    if (status == UMI_STATUS_OK)
        status = UmiReleaseImpactFromObservations(inventory, baseline, current, &plan, &observed);
    if (status == UMI_STATUS_OK) status = UmiReleaseImpactDescribe(plan, &selection);
    if (status == UMI_STATUS_OK) {
        if (printf("Synthetic observation: %zu files | changes known: %s | changed: %zu\n"
            "Selected: %zu targets | %zu tests | observation fallback: %u\n"
            "This is a selection, not a build or test result.\n",
            observed.observedFiles, observed.changesKnown ? "yes" : "no", observed.changedFiles,
            selection.targets, selection.tests, (unsigned)observed.fallbackReasons) < 0)
            status = UMI_STATUS_IO_ERROR;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < UmiReleaseImpactCount(plan); ++i) {
            const UmiReleaseImpactItem *item = UmiReleaseImpactAt(plan, i);
            if (printf("%s: %s\n", item->kind == UMI_RELEASE_INVENTORY_TEST ? "test" : "target",
                item->name) < 0) status = UMI_STATUS_IO_ERROR;
        }
        if (status == UMI_STATUS_OK && fflush(stdout) != 0) status = UMI_STATUS_IO_ERROR;
    }
    if (status != UMI_STATUS_OK) fprintf(stderr, "Selection refused: %s\n", umi_status_text(status));
    UmiReleaseImpactDestroy(plan);
    UmiReleaseSourceObservationDestroy(current);
    UmiReleaseSourceObservationDestroy(baseline);
    UmiReleaseInventoryDestroy(inventory);
    return status == UMI_STATUS_OK ? 0 : 2;
}
#endif

/* These deliberately synthetic records teach the host contract. They do not
 * certify a real build: the example labels its source and output accordingly. */
static UmiStatus ExampleHistory(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceObservation *before, bool failed, char toolchain[65],
    UmiReleaseQualification **out)
{
    char execution[65];
    UmiStatus status = UmiSha256Buffer("example-attempt", 15U, execution);
    if (status == UMI_STATUS_OK)
        status = UmiSha256Buffer("example-toolchain", 17U, toolchain);
    UmiReleaseImpactPlan *full = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiReleaseImpactCreate(inventory, NULL, 0U, true, &full);
    if (status == UMI_STATUS_OK)
        status = UmiReleaseQualificationCreate(full, before, execution, toolchain, out);
    UmiReleaseImpactDestroy(full);
    UmiReleaseQualificationIdentity identity;
    if (status == UMI_STATUS_OK) status = UmiReleaseQualificationGetIdentity(*out, &identity);
    UmiReleaseQualificationResult result = {
        .outcome = UMI_QUALIFICATION_PASSED, .status = UMI_STATUS_OK,
        .exitCodeKnown = true
    };
    if (status == UMI_STATUS_OK)
        status = UmiSha256Buffer("synthetic log", 13U, result.logSha256);
    if (status == UMI_STATUS_OK)
        status = UmiReleaseQualificationRecordBuild(*out, &identity, &result);
    if (status == UMI_STATUS_OK)
        status = UmiReleaseQualificationRecordTest(*out, &identity, "view.check", &result);
    if (failed) { result.outcome = UMI_QUALIFICATION_FAILED; result.exitCode = 1; }
    if (status == UMI_STATUS_OK)
        status = UmiReleaseQualificationRecordTest(*out, &identity, "other.check", &result);
    if (status == UMI_STATUS_OK)
        status = UmiReleaseQualificationClose(*out, &identity, before, toolchain);
    return status;
}

/* A restart cannot borrow the earlier worker. Format and hash teaching bytes,
 * destroy that worker, then invoke the public reader with a separate receipt. */
static UmiStatus ExampleSavedPlan(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceObservation *baseline, const UmiReleaseSourceObservation *current,
    UmiReleaseQualification **previous, const char *toolchain,
    UmiReleaseImpactPlan **plan, UmiQualificationRetrySummary *summary)
{
    size_t length = 0U;
    UmiStatus status = UmiReleaseQualificationFormatReport(*previous, NULL, 0U, &length);
    char *report = NULL;
    if (status == UMI_STATUS_OK) {
        report = malloc(length);
        if (report == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK)
        status = UmiReleaseQualificationFormatReport(*previous, report, length, &length);
    char digest[65];
    if (status == UMI_STATUS_OK) status = UmiSha256Buffer(report, length - 1U, digest);
    if (status == UMI_STATUS_OK) {
        UmiReleaseQualificationDestroy(*previous);
        *previous = NULL;
        status = UmiReleaseQualificationPlanFromReport(inventory, baseline, current,
            report, length - 1U, digest, toolchain, plan, summary);
    }
    free(report);
    return status;
}

/* Reconstruct the baseline from data after releasing its original allocation.
 * A real host stores these bytes and the receipt privately between executions. */
static UmiStatus ExampleRestoreBaseline(const UmiReleaseInventory *inventory,
    UmiReleaseSourceObservation **baseline)
{
    char *bytes = NULL, digest[65];
    size_t length = 0U;
    UmiStatus status = UmiReleaseSourceObservationEncode(*baseline, &bytes, &length, digest);
    if (status == UMI_STATUS_OK) {
        UmiReleaseSourceObservationDestroy(*baseline);
        *baseline = NULL;
        status = UmiReleaseSourceObservationDecode(inventory, bytes, length, digest, baseline);
    }
    UmiReleaseSourceObservationFreeEncoded(bytes);
    return status;
}

int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "same") != 0 &&
        strcmp(argv[1], "source") != 0 && strcmp(argv[1], "first") != 0 &&
        strcmp(argv[1], "retry") != 0 && strcmp(argv[1], "union") != 0 &&
        strcmp(argv[1], "unknown-retry") != 0 && strcmp(argv[1], "history-pass") != 0 &&
        strcmp(argv[1], "history-failed") != 0 && strcmp(argv[1], "history-unknown-tools") != 0 &&
        strcmp(argv[1], "saved-pass") != 0 && strcmp(argv[1], "saved-failed") != 0 &&
        strcmp(argv[1], "saved-unknown-tools") != 0 && strcmp(argv[1], "restored-pass") != 0 &&
        strcmp(argv[1], "restored-failed") != 0 && strcmp(argv[1], "restored-edited") != 0)) {
        fprintf(stderr, "Usage: umicom-observed-impact <same|source|first|retry|union|unknown-retry|history-pass|history-failed|history-unknown-tools|saved-pass|saved-failed|saved-unknown-tools|restored-pass|restored-failed|restored-edited>\n");
        return 2;
    }
    UmiReleaseInventory *inventory = NULL;
    UmiReleaseSourceObservation *baseline = NULL, *current = NULL;
    UmiReleaseImpactPlan *plan = NULL;
    UmiObservedImpactSummary observed;
    UmiReleaseQualification *previous = NULL;
    UmiQualificationRetrySummary historySummary = {0};
    bool restoredMode = strncmp(argv[1], "restored-", 9U) == 0;
    bool savedMode = restoredMode || strncmp(argv[1], "saved-", 6U) == 0;
    bool historyMode = savedMode || strncmp(argv[1], "history-", 8U) == 0;
    UmiReleaseImpactSummary selection;
    ExampleSource source = {false};
    UmiReleaseSourceOptions options = {
        .maximumFileBytes = 1024U, .maximumTotalBytes = 4096U,
        .read = ReadSource, .context = &source
    };
    UmiStatus status = UmiReleaseInventoryParse(inventory_text,
        sizeof(inventory_text) - 1U, &inventory);
    if (status == UMI_STATUS_OK && strcmp(argv[1], "first") != 0)
        status = UmiReleaseSourceObserve(inventory, &options, &baseline);
    source.edited = strcmp(argv[1], "source") == 0 || strcmp(argv[1], "union") == 0 ||
        strcmp(argv[1], "restored-edited") == 0;
    if (status == UMI_STATUS_OK)
        status = UmiReleaseSourceObserve(inventory, &options, &current);
    /* A real host obtains these exact names from checked failure or coverage
     * evidence. This synthetic example deliberately performs no test execution. */
    const char *retries[] = {strcmp(argv[1], "unknown-retry") == 0 ? "missing.check" : "other.check"};
    bool retry = strcmp(argv[1], "retry") == 0 || strcmp(argv[1], "union") == 0 ||
        strcmp(argv[1], "unknown-retry") == 0;
    if (status == UMI_STATUS_OK && historyMode) {
        char toolchain[65];
        status = ExampleHistory(inventory, baseline, strcmp(argv[1], "history-failed") == 0 ||
            strcmp(argv[1], "saved-failed") == 0 || strcmp(argv[1], "restored-failed") == 0,
            toolchain, &previous);
        if (status == UMI_STATUS_OK && restoredMode)
            status = ExampleRestoreBaseline(inventory, &baseline);
        const char *knownTools = strcmp(argv[1], "history-unknown-tools") == 0 ||
            strcmp(argv[1], "saved-unknown-tools") == 0 ? NULL : toolchain;
        if (status == UMI_STATUS_OK && savedMode)
            status = ExampleSavedPlan(inventory, baseline, current, &previous,
                knownTools, &plan, &historySummary);
        else if (status == UMI_STATUS_OK)
            status = UmiReleaseQualificationPlanNext(inventory, baseline, current, previous,
                knownTools, &plan, &historySummary);
        if (status == UMI_STATUS_OK) observed = historySummary.observation;
    } else if (status == UMI_STATUS_OK)
        status = UmiReleaseImpactFromObservationsWithTests(inventory, baseline, current,
            retries, retry ? 1U : 0U, &plan, &observed);
    if (status == UMI_STATUS_OK) status = UmiReleaseImpactDescribe(plan, &selection);
    if (status == UMI_STATUS_OK) {
        if (printf("Synthetic observation: %zu files | changes known: %s | changed: %zu\n"
            "Selected: %zu targets | %zu tests | observation fallback: %u\n"
            "Graph fallback: %u\n"
            "This is a selection, not a build or test result.\n",
            observed.observedFiles, observed.changesKnown ? "yes" : "no", observed.changedFiles,
            selection.targets, selection.tests, (unsigned)observed.fallbackReasons,
            (unsigned)selection.fallbackReasons) < 0)
            status = UMI_STATUS_IO_ERROR;
        if (status == UMI_STATUS_OK && historyMode &&
            printf("Synthetic history: %zu considered passes | %zu required tests | reasons: %u\n",
                historySummary.historicalPasses, historySummary.requiredTests,
                (unsigned)historySummary.historyReasons) < 0) status = UMI_STATUS_IO_ERROR;
        if (status == UMI_STATUS_OK && restoredMode &&
            printf("Restored source baseline: historical observation, not current source proof.\n") < 0)
            status = UMI_STATUS_IO_ERROR;
        if (status == UMI_STATUS_OK && savedMode &&
            printf("Synthetic saved report: reopened after original attempt destruction.\n") < 0)
            status = UMI_STATUS_IO_ERROR;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < UmiReleaseImpactCount(plan); ++i) {
            const UmiReleaseImpactItem *item = UmiReleaseImpactAt(plan, i);
            if (printf("%s: %s\n", item->kind == UMI_RELEASE_INVENTORY_TEST ? "test" : "target",
                item->name) < 0) status = UMI_STATUS_IO_ERROR;
        }
        if (status == UMI_STATUS_OK && fflush(stdout) != 0) status = UMI_STATUS_IO_ERROR;
    }
    if (status != UMI_STATUS_OK) fprintf(stderr, "Selection refused: %s\n", umi_status_text(status));
    UmiReleaseQualificationDestroy(previous);
    UmiReleaseImpactDestroy(plan);
    UmiReleaseSourceObservationDestroy(current);
    UmiReleaseSourceObservationDestroy(baseline);
    UmiReleaseInventoryDestroy(inventory);
    return status == UMI_STATUS_OK ? 0 : 2;
}
