/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/inventory.c
 * Purpose: Compare complete test identities through the canonical distribution owner.
 * Author: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "inventory_internal.h"
#include <stdlib.h>
#include <string.h>

void UmiReleaseInventoryDestroy(UmiReleaseInventory *inventory)
{
    if (inventory != NULL) {
        free(inventory->records); free(inventory->storage); free(inventory);
    }
}
size_t UmiReleaseInventoryCount(const UmiReleaseInventory *inventory)
{ return inventory != NULL ? inventory->count : 0U; }
const UmiReleaseInventoryRecord *UmiReleaseInventoryAt(const UmiReleaseInventory *inventory, size_t index)
{ return inventory != NULL && index < inventory->count ? &inventory->records[index] : NULL; }
const char *UmiReleaseInventoryProducer(const UmiReleaseInventory *inventory)
{ return inventory != NULL ? inventory->producer : NULL; }
const char *UmiReleaseInventoryGeneration(const UmiReleaseInventory *inventory)
{ return inventory != NULL ? inventory->generation : NULL; }
const char *UmiReleaseInventorySourceRoot(const UmiReleaseInventory *inventory)
{ return inventory != NULL ? inventory->sourceRoot : NULL; }
const char *UmiReleaseInventoryBuildRoot(const UmiReleaseInventory *inventory)
{ return inventory != NULL ? inventory->buildRoot : NULL; }
const char *UmiReleaseInventoryConfiguration(const UmiReleaseInventory *inventory)
{ return inventory != NULL ? inventory->configuration : NULL; }

UmiStatus UmiReleaseInventorySummarise(const UmiReleaseInventory *inventory,
    UmiReleaseInventorySummary *outSummary)
{
    if (inventory == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiReleaseInventorySummary summary = {0};
    for (size_t i = 0U; i < inventory->count; ++i) {
        const UmiReleaseInventoryRecord *row = &inventory->records[i];
        switch (row->kind) {
        case UMI_RELEASE_INVENTORY_HEADER:
            ++summary.headers;
            if (strcmp(row->state, "unassigned") == 0) ++summary.unassignedHeaders;
            break;
        case UMI_RELEASE_INVENTORY_TARGET: ++summary.targets; break;
        case UMI_RELEASE_INVENTORY_SOURCE:
            ++summary.sources;
            if (strcmp(row->state, "unresolved") == 0) ++summary.unresolvedSources;
            break;
        case UMI_RELEASE_INVENTORY_TEST:
            ++summary.tests;
            if (strcmp(row->state, "disabled") == 0 || strcmp(row->state, "disabled-missing-command") == 0)
                ++summary.disabledTests;
            if (strcmp(row->state, "missing-command") == 0 || strcmp(row->state, "disabled-missing-command") == 0)
                ++summary.missingCommands;
            break;
        }
    }
    *outSummary = summary;
    return UMI_STATUS_OK;
}

/* The parser sorts records once. A linear merge then reports exact names even
 * when the populations have equal counts. No regex or basename conversion can
 * accidentally equate different tests, profiles or build directories. */
UmiStatus UmiReleaseInventoryCompareTests(const UmiReleaseInventory *expected,
    const UmiReleaseInventory *observed, UmiReleaseInventoryDifferenceFn difference,
    void *context, UmiReleaseInventoryComparison *outComparison)
{
    if (expected == NULL || observed == NULL || outComparison == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(expected->producer, "cmake") != 0 || strcmp(observed->producer, "ctest") != 0 ||
        strcmp(expected->generation, observed->generation) != 0 ||
        strcmp(expected->sourceRoot, observed->sourceRoot) != 0 ||
        strcmp(expected->buildRoot, observed->buildRoot) != 0 ||
        strcmp(expected->configuration, observed->configuration) != 0)
        return UMI_STATUS_INVALID_STATE;
    UmiReleaseInventorySummary left = {0}, right = {0};
    (void)UmiReleaseInventorySummarise(expected, &left);
    (void)UmiReleaseInventorySummarise(observed, &right);
    if (left.tests == 0U || right.tests == 0U) return UMI_STATUS_UNAVAILABLE;
    UmiReleaseInventoryComparison result = {0};
    result.expectedTests = left.tests; result.observedTests = right.tests;
    result.disabledTests = right.disabledTests; result.missingCommands = right.missingCommands;
    size_t a = 0U, b = 0U;
    while (a < expected->count && expected->records[a].kind != UMI_RELEASE_INVENTORY_TEST) ++a;
    while (b < observed->count && observed->records[b].kind != UMI_RELEASE_INVENTORY_TEST) ++b;
    while (a < expected->count || b < observed->count) {
        const char *leftName = a < expected->count ? expected->records[a].identity : NULL;
        const char *rightName = b < observed->count ? observed->records[b].identity : NULL;
        int order = leftName == NULL ? 1 : rightName == NULL ? -1 : strcmp(leftName, rightName);
        if (order < 0) {
            ++result.missingTests;
            if (difference != NULL) difference(context, UMI_RELEASE_INVENTORY_TEST_MISSING, leftName);
            ++a;
        } else if (order > 0) {
            ++result.addedTests;
            if (difference != NULL) difference(context, UMI_RELEASE_INVENTORY_TEST_ADDED, rightName);
            ++b;
        } else { ++a; ++b; }
    }
    result.namesMatch = result.missingTests == 0U && result.addedTests == 0U;
    *outComparison = result;
    return UMI_STATUS_OK;
}
