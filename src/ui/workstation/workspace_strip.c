/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/workspace_strip.c
 *
 * PURPOSE:
 *   Implement bottom/top workspace strip state for rapid switching between named layouts and perspectives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/workspace_strip.h"

/*
 * Initialise ws workspace strip from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_workspace_strip_init(UmiWsWorkspaceStrip *strip) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strip != NULL) *strip = (UmiWsWorkspaceStrip){0};
}

/* Add ws workspace strip only after its inputs and available capacity have been checked. */
UmiStatus umi_ws_workspace_strip_add(UmiWsWorkspaceStrip *strip,
                                     const char *workspace_id,
                                     const char *label,
                                     const char *layout_id) {
    UmiWsWorkspaceEntry *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strip == NULL || !umi_ws_id_valid(workspace_id) || label == NULL || !umi_ws_id_valid(layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strip->count >= UMI_WS_MAX_WORKSPACES) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &strip->entries[strip->count];
    *entry = (UmiWsWorkspaceEntry){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(entry->workspace_id, sizeof(entry->workspace_id), workspace_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(entry->label, sizeof(entry->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(entry->layout_id, sizeof(entry->layout_id), layout_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry->pinned = true;
    ++strip->count;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws workspace strip activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_workspace_strip_activate(UmiWsWorkspaceStrip *strip, const char *workspace_id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strip == NULL || !umi_ws_id_valid(workspace_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < strip->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(strip->entries[index].workspace_id, workspace_id) == 0) {
            strip->active_index = index;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the ws workspace strip active operation used by this module and its client
 * applications.
 */
const UmiWsWorkspaceEntry *umi_ws_workspace_strip_active(const UmiWsWorkspaceStrip *strip) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strip == NULL || strip->count == 0U || strip->active_index >= strip->count) return NULL;
    return &strip->entries[strip->active_index];
}
