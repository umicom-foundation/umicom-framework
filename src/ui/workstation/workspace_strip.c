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

void umi_ws_workspace_strip_init(UmiWsWorkspaceStrip *strip) {
    if (strip != NULL) *strip = (UmiWsWorkspaceStrip){0};
}

UmiStatus umi_ws_workspace_strip_add(UmiWsWorkspaceStrip *strip,
                                     const char *workspace_id,
                                     const char *label,
                                     const char *layout_id) {
    UmiWsWorkspaceEntry *entry;
    if (strip == NULL || !umi_ws_id_valid(workspace_id) || label == NULL || !umi_ws_id_valid(layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (strip->count >= UMI_WS_MAX_WORKSPACES) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &strip->entries[strip->count];
    *entry = (UmiWsWorkspaceEntry){0};
    if (umi_ws_copy_text(entry->workspace_id, sizeof(entry->workspace_id), workspace_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(entry->label, sizeof(entry->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(entry->layout_id, sizeof(entry->layout_id), layout_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry->pinned = true;
    ++strip->count;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_workspace_strip_activate(UmiWsWorkspaceStrip *strip, const char *workspace_id) {
    size_t index;
    if (strip == NULL || !umi_ws_id_valid(workspace_id)) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < strip->count; ++index) {
        if (strcmp(strip->entries[index].workspace_id, workspace_id) == 0) {
            strip->active_index = index;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

const UmiWsWorkspaceEntry *umi_ws_workspace_strip_active(const UmiWsWorkspaceStrip *strip) {
    if (strip == NULL || strip->count == 0U || strip->active_index >= strip->count) return NULL;
    return &strip->entries[strip->active_index];
}
