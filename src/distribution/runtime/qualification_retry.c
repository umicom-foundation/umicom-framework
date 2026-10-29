/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_retry.c
 *
 * PURPOSE:
 *   Retain failed and missing checks independently of source-change detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/qualification_retry.h"
#include "qualification_internal.h"
#include <stdlib.h>
#include <string.h>

static bool KnownPass(const UmiReleaseQualification *run, size_t index,
    uint32_t historyReasons)
{
    if (historyReasons != 0U || run == NULL || index == SIZE_MAX ||
        !run->tests[index].recorded) return false;
    const UmiReleaseQualificationResult *result = &run->tests[index].result;
    return result->outcome == UMI_QUALIFICATION_PASSED &&
        (result->exitCodeKnown || (run->testProcessRecorded &&
            run->testProcess.outcome == UMI_QUALIFICATION_PASSED));
}
static bool HasTest(const UmiReleaseInventory *inventory, size_t first,
    size_t count, const char *name)
{
    size_t low = first, high = first + count;
    while (low < high) {
        size_t middle = low + (high - low) / 2U;
        const UmiReleaseInventoryRecord *row = UmiReleaseInventoryAt(inventory, middle);
        if (strcmp(row->identity, name) < 0) low = middle + 1U;
        else high = middle;
    }
    return low < first + count &&
        strcmp(UmiReleaseInventoryAt(inventory, low)->identity, name) == 0;
}
static uint32_t HistoryReasons(const UmiReleaseQualification *previous,
    const UmiReleaseSourceObservation *baseline, const char *inventory,
    const char *toolchain)
{
    if (previous == NULL) return UMI_QUALIFICATION_RETRY_NO_HISTORY;
    uint32_t reasons = 0U;
    if (!previous->closed) reasons |= UMI_QUALIFICATION_RETRY_OPEN_HISTORY;
    if (baseline == NULL || strcmp(previous->source,
        UmiReleaseSourceObservationFingerprint(baseline)) != 0)
        reasons |= UMI_QUALIFICATION_RETRY_BASELINE_MISMATCH;
    if (strcmp(previous->inventory, inventory) != 0)
        reasons |= UMI_QUALIFICATION_RETRY_INVENTORY_MISMATCH;
    if (toolchain == NULL || strcmp(previous->toolchain, toolchain) != 0)
        reasons |= UMI_QUALIFICATION_RETRY_TOOLCHAIN_MISMATCH;
    if (previous->closeBlockers != 0U)
        reasons |= UMI_QUALIFICATION_RETRY_UNSTABLE_ATTEMPT;
    if (!previous->buildRecorded || previous->build.outcome != UMI_QUALIFICATION_PASSED)
        reasons |= UMI_QUALIFICATION_RETRY_BUILD_UNQUALIFIED;
    if (previous->testProcessRecorded &&
        previous->testProcess.outcome != UMI_QUALIFICATION_PASSED)
        reasons |= UMI_QUALIFICATION_RETRY_RUNNER_UNQUALIFIED;
    if (previous->executionStatus != UMI_STATUS_OK)
        reasons |= UMI_QUALIFICATION_RETRY_INTERRUPTED;
    return reasons;
}
static UmiStatus Require(const char **names, size_t *count, const char *name)
{
    if (*count == UMI_RELEASE_IMPACT_CHANGE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    names[(*count)++] = name;
    return UMI_STATUS_OK;
}
UmiStatus UmiReleaseQualificationPlanNext(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceObservation *baseline,
    const UmiReleaseSourceObservation *current,
    const UmiReleaseQualification *previous, const char *toolchain,
    UmiReleaseImpactPlan **outPlan, UmiQualificationRetrySummary *outSummary)
{
    if (inventory == NULL || current == NULL || outPlan == NULL ||
        *outPlan != NULL || outSummary == NULL ||
        (toolchain != NULL && !UmiQualificationDigestValid(toolchain)))
        return UMI_STATUS_INVALID_ARGUMENT;
    char identity[65];
    UmiStatus status = UmiReleaseInventoryFingerprint(inventory, identity);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(identity, UmiReleaseSourceObservationInventoryFingerprint(current)) != 0)
        return UMI_STATUS_INVALID_STATE;
    UmiReleaseInventorySummary catalogue;
    status = UmiReleaseInventorySummarise(inventory, &catalogue);
    if (status != UMI_STATUS_OK) return status;
    if (catalogue.tests == 0U) return UMI_STATUS_UNAVAILABLE;
    if (catalogue.tests > UMI_RELEASE_IMPACT_CHANGE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const char **names = calloc(UMI_RELEASE_IMPACT_CHANGE_LIMIT, sizeof(*names));
    if (names == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiQualificationRetrySummary summary = {0};
    summary.configuredTests = catalogue.tests;
    summary.historyReasons = HistoryReasons(previous, baseline, identity, toolchain);
    /* Inventory records are sorted by kind/name; tests form the final group.
     * Exact binary lookups avoid scanning an entire report for every test. */
    size_t first = UmiReleaseInventoryCount(inventory) - catalogue.tests;
    for (size_t i = 0U; i < catalogue.tests && status == UMI_STATUS_OK; ++i) {
        const char *name = UmiReleaseInventoryAt(inventory, first + i)->identity;
        size_t old = previous == NULL ? SIZE_MAX : UmiQualificationFindTest(previous, name);
        if (old == SIZE_MAX || !previous->tests[old].recorded) ++summary.missingResults;
        else if (previous->tests[old].result.outcome != UMI_QUALIFICATION_PASSED)
            ++summary.nonpassingResults;
        if (KnownPass(previous, old, summary.historyReasons)) ++summary.historicalPasses;
        else status = Require(names, &summary.requiredTests, name);
    }
    /* Keep unresolved older names visible. A missing current registration may
     * be intentional, but only a host review can decide how to retire its issue. */
    if (previous != NULL) {
        for (size_t i = 0U; i < previous->selection.tests && status == UMI_STATUS_OK; ++i) {
            const char *name = previous->items[previous->testStart + i].name;
            if (HasTest(inventory, first, catalogue.tests, name) ||
                KnownPass(previous, i, summary.historyReasons)) continue;
            status = Require(names, &summary.requiredTests, name);
            if (status == UMI_STATUS_OK) ++summary.removedOutstandingTests;
        }
    }
    UmiReleaseImpactPlan *plan = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiReleaseImpactFromObservationsWithTests(inventory, baseline, current,
            names, summary.requiredTests, &plan, &summary.observation);
    if (status == UMI_STATUS_OK && summary.historyReasons != 0U) {
        /* An incompatible or incomplete attempt cannot restrict target work
         * to test owners: untested artifacts may also need rebuilding. Keep
         * the observed change explanation, but request the full graph. */
        UmiReleaseImpactPlan *full = NULL;
        status = UmiReleaseImpactCreateWithTests(inventory, NULL, 0U, names,
            summary.requiredTests, true, &full);
        if (status == UMI_STATUS_OK) { UmiReleaseImpactDestroy(plan); plan = full; }
    }
    free(names);
    /* Publish the complete selection and its explanation together. No state in
     * the earlier attempt changes; a retry is a new execution with its own ID. */
    if (status == UMI_STATUS_OK) {
        *outPlan = plan;
        *outSummary = summary;
    } else UmiReleaseImpactDestroy(plan);
    return status;
}
