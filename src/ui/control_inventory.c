/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/control_inventory.c
 *
 * PURPOSE:
 *   Keep a bounded, parent-ordered map of controls and their test identifiers.
 *   Inspecting this model never invokes an application action.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/control_inventory.h"
#include "ui_internal.h"
#include <stdlib.h>
#include <string.h>

struct UmiUiControlInventory {
    UmiUiControlRecord *records;
    size_t count;
    size_t capacity;
};

/* Allocate only the caller's reviewed capacity, not an unbounded widget list. */
UmiStatus UmiUiControlInventoryCreate(size_t capacity, UmiUiControlInventory **outInventory)
{
    UmiUiControlInventory *inventory;
    if (outInventory == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outInventory = NULL;
    if (capacity == 0U || capacity > UMI_UI_CONTROL_INVENTORY_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    inventory = calloc(1U, sizeof(*inventory));
    if (inventory == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    inventory->records = calloc(capacity, sizeof(*inventory->records));
    if (inventory->records == NULL) { free(inventory); return UMI_STATUS_OUT_OF_MEMORY; }
    inventory->capacity = capacity;
    *outInventory = inventory;
    return UMI_STATUS_OK;
}

void UmiUiControlInventoryDestroy(UmiUiControlInventory *inventory)
{
    if (inventory == NULL) return;
    free(inventory->records);
    free(inventory);
}

/* Reject ambiguous Boolean encodings at the shared boundary. */
static int Boolean(int value) { return value == 0 || value == 1; }

UmiStatus UmiUiControlInventoryAdd(UmiUiControlInventory *inventory,
    const UmiUiControlRecord *record)
{
    size_t i;
    if (inventory == NULL || record == NULL || record->scopeId == 0U ||
        !umi_ui_text_is_valid_utf8(record->automationId, sizeof(record->automationId)) ||
        !umi_ui_text_is_valid_utf8(record->roleName, sizeof(record->roleName)) ||
        !umi_ui_id_is_valid(record->roleName) ||
        (record->automationId[0] != '\0' && !umi_ui_id_is_valid(record->automationId)) ||
        !Boolean(record->visible) || !Boolean(record->mapped) ||
        !Boolean(record->enabled) || !Boolean(record->focused) ||
        !Boolean(record->interactive) || !Boolean(record->privateControl) ||
        (record->mapped && !record->visible)) return UMI_STATUS_INVALID_ARGUMENT;
    if (record->privateControl && (record->automationId[0] != '\0' ||
        strcmp(record->roleName, "private-control") != 0)) return UMI_STATUS_INVALID_ARGUMENT;
    if (record->parentIndex != UMI_UI_CONTROL_NO_PARENT) {
        if (record->parentIndex >= inventory->count ||
            inventory->records[record->parentIndex].scopeId != record->scopeId ||
            inventory->records[record->parentIndex].privateControl)
            return UMI_STATUS_INVALID_ARGUMENT;
    } else {
        for (i = 0U; i < inventory->count; ++i)
            if (inventory->records[i].scopeId == record->scopeId)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    if (inventory->count >= inventory->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    inventory->records[inventory->count++] = *record;
    return UMI_STATUS_OK;
}

size_t UmiUiControlInventoryCount(const UmiUiControlInventory *inventory)
{ return inventory != NULL ? inventory->count : 0U; }

UmiStatus UmiUiControlInventoryAt(const UmiUiControlInventory *inventory,
    size_t index, UmiUiControlRecord *outRecord)
{
    if (inventory == NULL || outRecord == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= inventory->count) return UMI_STATUS_NOT_FOUND;
    *outRecord = inventory->records[index];
    return UMI_STATUS_OK;
}

/* Parent indices strictly decrease, so malformed cycles cannot enter storage. */
UmiStatus UmiUiControlInventoryTarget(const UmiUiControlInventory *inventory,
    size_t index, size_t *outIndex)
{
    if (outIndex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outIndex = UMI_UI_CONTROL_NO_PARENT;
    if (inventory == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= inventory->count) return UMI_STATUS_NOT_FOUND;
    while (index != UMI_UI_CONTROL_NO_PARENT) {
        const UmiUiControlRecord *record = &inventory->records[index];
        if (record->privateControl) return UMI_STATUS_NOT_FOUND;
        if (record->automationId[0] != '\0') { *outIndex = index; return UMI_STATUS_OK; }
        index = record->parentIndex;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus UmiUiControlInventorySummarise(const UmiUiControlInventory *inventory,
    UmiUiControlInventorySummary *outSummary)
{
    size_t i, j;
    if (outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outSummary, 0, sizeof(*outSummary));
    if (inventory == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    outSummary->controls = inventory->count;
    for (i = 0U; i < inventory->count; ++i) {
        const UmiUiControlRecord *record = &inventory->records[i];
        size_t target;
        if (record->parentIndex == UMI_UI_CONTROL_NO_PARENT) ++outSummary->windows;
        if (record->privateControl) { ++outSummary->privateControls; continue; }
        if (record->interactive) {
            ++outSummary->interactive;
            if (UmiUiControlInventoryTarget(inventory, i, &target) == UMI_STATUS_NOT_FOUND)
                ++outSummary->unaddressableInteractive;
            else if (target != i) ++outSummary->inheritedTargets;
        }
        if (record->automationId[0] == '\0') continue;
        ++outSummary->tagged;
        for (j = 0U; j < inventory->count; ++j) {
            if (i != j && record->scopeId == inventory->records[j].scopeId &&
                strcmp(record->automationId, inventory->records[j].automationId) == 0) {
                ++outSummary->ambiguousTargets;
                break;
            }
        }
    }
    return UMI_STATUS_OK;
}
