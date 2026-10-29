/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/observed_impact.c
 *
 * PURPOSE:
 *   Reuse exact inventory identities and dependency ownership for observed edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/observed_impact.h"
#include "impact_internal.h"
#include <stdlib.h>
#include <string.h>

/* The original observation-only bridge is retained for review. Required test
 * identities now flow to the same graph constructor, preserving retries even
 * when source observation finds no byte changes. Source-only callers keep the
 * compatibility entry below and receive the same selection as before. */
#if 0
UmiStatus UmiReleaseImpactFromObservations(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceObservation *baseline,
    const UmiReleaseSourceObservation *current,
    UmiReleaseImpactPlan **outPlan, UmiObservedImpactSummary *outSummary)
{
    if (inventory == NULL || current == NULL || outPlan == NULL ||
        *outPlan != NULL || outSummary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    char identity[65];
    UmiStatus status = UmiReleaseInventoryFingerprint(inventory, identity);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(identity, UmiReleaseSourceObservationInventoryFingerprint(current)) != 0)
        return UMI_STATUS_INVALID_STATE;

    UmiObservedImpactSummary summary = {
        .observedFiles = UmiReleaseSourceObservationCount(current)
    };
    UmiReleaseSourceComparison comparison = {0};
    if (baseline == NULL)
        summary.fallbackReasons = UMI_OBSERVED_IMPACT_BASELINE_MISSING;
    else {
        status = UmiReleaseSourceObservationCompare(baseline, current, &comparison);
        if (status == UMI_STATUS_INVALID_STATE) {
            summary.fallbackReasons = UMI_OBSERVED_IMPACT_INVENTORY_CHANGED;
            status = UMI_STATUS_OK;
        } else if (status == UMI_STATUS_OK) {
            summary.changesKnown = true;
            summary.changedFiles = comparison.changedFiles;
        }
        if (status != UMI_STATUS_OK) return status;
    }

    if (summary.changedFiles > UMI_RELEASE_IMPACT_CHANGE_LIMIT ||
        summary.changedFiles > SIZE_MAX / sizeof(char *))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep normalized path storage until the existing planner copies its
     * result. This bridge does not duplicate dependency traversal or CTest
     * fixture closure, and never infers owners from executable names. */
    char **paths = summary.changedFiles != 0U ?
        calloc(summary.changedFiles, sizeof(*paths)) : NULL;
    if (summary.changedFiles != 0U && paths == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t mapped = 0U;
    if (summary.changesKnown) {
        for (size_t i = 0U; i < summary.observedFiles && status == UMI_STATUS_OK; ++i) {
            const UmiReleaseSourceItem *a = UmiReleaseSourceObservationAt(baseline, i);
            const UmiReleaseSourceItem *b = UmiReleaseSourceObservationAt(current, i);
            if (a == NULL || b == NULL) { status = UMI_STATUS_INVALID_STATE; break; }
            if (a->bytes == b->bytes && strcmp(a->sha256, b->sha256) == 0) continue;
            char *relative = NULL;
            status = UmiImpactRelativePath(UmiReleaseInventorySourceRoot(inventory),
                b->path, &relative);
            if (status == UMI_STATUS_NOT_FOUND) {
                summary.fallbackReasons |= UMI_OBSERVED_IMPACT_OUTSIDE_ROOT;
                status = UMI_STATUS_OK;
            } else if (status == UMI_STATUS_OK) {
                if (mapped == summary.changedFiles) {
                    free(relative);
                    status = UMI_STATUS_INVALID_STATE;
                } else paths[mapped++] = relative;
            }
        }
    }
    UmiReleaseImpactPlan *plan = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiReleaseImpactCreate(inventory, (const char *const *)paths,
            mapped, summary.fallbackReasons != 0U, &plan);
    for (size_t i = 0U; i < mapped; ++i) free(paths[i]);
    free(paths);
    /* Publish the selection and explanation together. A malformed graph must
     * not leave a plausible changed-file count beside a missing or stale plan. */
    if (status == UMI_STATUS_OK) {
        *outPlan = plan;
        *outSummary = summary;
    } else UmiReleaseImpactDestroy(plan);
    return status;
}
#endif
UmiStatus UmiReleaseImpactFromObservationsWithTests(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceObservation *baseline,
    const UmiReleaseSourceObservation *current,
    const char *const *requiredTests, size_t requiredCount,
    UmiReleaseImpactPlan **outPlan, UmiObservedImpactSummary *outSummary)
{
    if (inventory == NULL || current == NULL || outPlan == NULL ||
        *outPlan != NULL || outSummary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    char identity[65];
    UmiStatus status = UmiReleaseInventoryFingerprint(inventory, identity);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(identity, UmiReleaseSourceObservationInventoryFingerprint(current)) != 0)
        return UMI_STATUS_INVALID_STATE;

    UmiObservedImpactSummary summary = {
        .observedFiles = UmiReleaseSourceObservationCount(current)
    };
    UmiReleaseSourceComparison comparison = {0};
    if (baseline == NULL)
        summary.fallbackReasons = UMI_OBSERVED_IMPACT_BASELINE_MISSING;
    else {
        status = UmiReleaseSourceObservationCompare(baseline, current, &comparison);
        if (status == UMI_STATUS_INVALID_STATE) {
            summary.fallbackReasons = UMI_OBSERVED_IMPACT_INVENTORY_CHANGED;
            status = UMI_STATUS_OK;
        } else if (status == UMI_STATUS_OK) {
            summary.changesKnown = true;
            summary.changedFiles = comparison.changedFiles;
        }
        if (status != UMI_STATUS_OK) return status;
    }

    if (summary.changedFiles > UMI_RELEASE_IMPACT_CHANGE_LIMIT ||
        summary.changedFiles > SIZE_MAX / sizeof(char *))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep normalized path storage until the existing planner copies its
     * result. This bridge does not duplicate dependency traversal or CTest
     * fixture closure, and never infers owners from executable names. */
    char **paths = summary.changedFiles != 0U ?
        calloc(summary.changedFiles, sizeof(*paths)) : NULL;
    if (summary.changedFiles != 0U && paths == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t mapped = 0U;
    if (summary.changesKnown) {
        for (size_t i = 0U; i < summary.observedFiles && status == UMI_STATUS_OK; ++i) {
            const UmiReleaseSourceItem *a = UmiReleaseSourceObservationAt(baseline, i);
            const UmiReleaseSourceItem *b = UmiReleaseSourceObservationAt(current, i);
            if (a == NULL || b == NULL) { status = UMI_STATUS_INVALID_STATE; break; }
            if (a->bytes == b->bytes && strcmp(a->sha256, b->sha256) == 0) continue;
            char *relative = NULL;
            status = UmiImpactRelativePath(UmiReleaseInventorySourceRoot(inventory),
                b->path, &relative);
            if (status == UMI_STATUS_NOT_FOUND) {
                summary.fallbackReasons |= UMI_OBSERVED_IMPACT_OUTSIDE_ROOT;
                status = UMI_STATUS_OK;
            } else if (status == UMI_STATUS_OK) {
                if (mapped == summary.changedFiles) {
                    free(relative);
                    status = UMI_STATUS_INVALID_STATE;
                } else paths[mapped++] = relative;
            }
        }
    }
    UmiReleaseImpactPlan *plan = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiReleaseImpactCreateWithTests(inventory, (const char *const *)paths,
            mapped, requiredTests, requiredCount, summary.fallbackReasons != 0U, &plan);
    for (size_t i = 0U; i < mapped; ++i) free(paths[i]);
    free(paths);
    /* Publish the selection and explanation together. A malformed graph must
     * not leave a plausible changed-file count beside a missing or stale plan. */
    if (status == UMI_STATUS_OK) {
        *outPlan = plan;
        *outSummary = summary;
    } else UmiReleaseImpactDestroy(plan);
    return status;
}
UmiStatus UmiReleaseImpactFromObservations(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceObservation *baseline,
    const UmiReleaseSourceObservation *current,
    UmiReleaseImpactPlan **outPlan, UmiObservedImpactSummary *outSummary)
{
    return UmiReleaseImpactFromObservationsWithTests(inventory, baseline, current,
        NULL, 0U, outPlan, outSummary);
}

