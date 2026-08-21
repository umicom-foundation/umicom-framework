/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/viewport_history.c
 *
 * PURPOSE:
 *   Implement bounded viewport navigation with forward-history truncation and
 *   named bookmark restoration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/viewport_history.h"
#include "internal.h"


void umi_workbench_designer_viewport_history_init(
    UmiWorkbenchDesignerViewportHistory *history)
{
    if (history == NULL) return;
    (void)memset(history, 0, sizeof(*history));
    history->cursor = UMI_WORKBENCH_DESIGNER_INDEX_NONE;
}

static UmiStatus viewport_entry_apply(
    const UmiWorkbenchDesignerViewportEntry *entry,
    UmiWorkbenchDesignerViewport *viewport)
{
    if (entry == NULL || viewport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    viewport->origin = entry->origin;
    viewport->canvas_size = entry->canvas_size;
    viewport->zoom = entry->zoom;
    viewport->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_viewport_history_push(
    UmiWorkbenchDesignerViewportHistory *history,
    const UmiWorkbenchDesignerViewport *viewport,
    const char *entry_id,
    const char *label,
    uint64_t timestamp_ms,
    bool bookmarked)
{
    UmiWorkbenchDesignerViewportEntry *entry;
    if (history == NULL || viewport == NULL || entry_id == NULL ||
        entry_id[0] == '\0' || label == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->cursor != UMI_WORKBENCH_DESIGNER_INDEX_NONE &&
        history->cursor + 1U < history->count) {
        history->count = history->cursor + 1U;
    }
    if (history->count >= UMI_WORKBENCH_DESIGNER_MAX_VIEWPORT_HISTORY) {
        size_t index;
        for (index = 0U; index + 1U < history->count; ++index) {
            history->entries[index] = history->entries[index + 1U];
        }
        history->count -= 1U;
    }
    entry = &history->entries[history->count];
    (void)memset(entry, 0, sizeof(*entry));
    (void)umi_workbench_designer_copy_text(
        entry->entry_id, sizeof(entry->entry_id), entry_id);
    (void)umi_workbench_designer_copy_text(
        entry->label, sizeof(entry->label), label);
    entry->origin = viewport->origin;
    entry->canvas_size = viewport->canvas_size;
    entry->zoom = viewport->zoom;
    entry->timestamp_ms = timestamp_ms;
    entry->bookmarked = bookmarked;
    history->cursor = history->count;
    history->count += 1U;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_workbench_designer_viewport_history_can_back(
    const UmiWorkbenchDesignerViewportHistory *history)
{
    return history != NULL && history->cursor != UMI_WORKBENCH_DESIGNER_INDEX_NONE &&
        history->cursor > 0U;
}

bool umi_workbench_designer_viewport_history_can_forward(
    const UmiWorkbenchDesignerViewportHistory *history)
{
    return history != NULL && history->cursor != UMI_WORKBENCH_DESIGNER_INDEX_NONE &&
        history->cursor + 1U < history->count;
}

UmiStatus umi_workbench_designer_viewport_history_back(
    UmiWorkbenchDesignerViewportHistory *history,
    UmiWorkbenchDesignerViewport *viewport)
{
    if (!umi_workbench_designer_viewport_history_can_back(history)) {
        return UMI_STATUS_NOT_FOUND;
    }
    history->cursor -= 1U;
    history->revision += 1U;
    return viewport_entry_apply(&history->entries[history->cursor], viewport);
}

UmiStatus umi_workbench_designer_viewport_history_forward(
    UmiWorkbenchDesignerViewportHistory *history,
    UmiWorkbenchDesignerViewport *viewport)
{
    if (!umi_workbench_designer_viewport_history_can_forward(history)) {
        return UMI_STATUS_NOT_FOUND;
    }
    history->cursor += 1U;
    history->revision += 1U;
    return viewport_entry_apply(&history->entries[history->cursor], viewport);
}

UmiStatus umi_workbench_designer_viewport_history_restore(
    const UmiWorkbenchDesignerViewportHistory *history,
    const char *entry_id,
    UmiWorkbenchDesignerViewport *viewport)
{
    size_t index;
    if (history == NULL || entry_id == NULL || viewport == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < history->count; ++index) {
        if (strcmp(history->entries[index].entry_id, entry_id) == 0) {
            return viewport_entry_apply(&history->entries[index], viewport);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
