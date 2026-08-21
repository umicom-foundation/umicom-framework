/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/collaboration_cursor.c
 *
 * PURPOSE:
 *   Maintain ordered remote cursor state while rejecting stale sequence updates
 *   from delayed collaboration messages.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/collaboration_cursor.h"
#include "internal.h"


void umi_workbench_designer_remote_cursor_model_init(
    UmiWorkbenchDesignerRemoteCursorModel *model)
{
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
}

static size_t remote_cursor_index(
    const UmiWorkbenchDesignerRemoteCursorModel *model,
    const char *user_id,
    const char *client_id)
{
    size_t index;
    if (model == NULL || user_id == NULL || client_id == NULL) {
        return UMI_WORKBENCH_DESIGNER_INDEX_NONE;
    }
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->cursors[index].user_id, user_id) == 0 &&
            strcmp(model->cursors[index].client_id, client_id) == 0) {
            return index;
        }
    }
    return UMI_WORKBENCH_DESIGNER_INDEX_NONE;
}

UmiStatus umi_workbench_designer_remote_cursor_upsert(
    UmiWorkbenchDesignerRemoteCursorModel *model,
    const UmiWorkbenchDesignerRemoteCursor *cursor)
{
    size_t index;
    if (model == NULL || cursor == NULL || cursor->user_id[0] == '\0' ||
        cursor->client_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = remote_cursor_index(model, cursor->user_id, cursor->client_id);
    if (index != UMI_WORKBENCH_DESIGNER_INDEX_NONE) {
        if (cursor->sequence < model->cursors[index].sequence) {
            return UMI_STATUS_INVALID_STATE;
        }
        model->cursors[index] = *cursor;
    } else {
        if (model->count >= UMI_WORKBENCH_DESIGNER_MAX_REMOTE_CURSORS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        model->cursors[model->count++] = *cursor;
    }
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_remote_cursor_remove(
    UmiWorkbenchDesignerRemoteCursorModel *model,
    const char *user_id,
    const char *client_id)
{
    size_t index;
    size_t move;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = remote_cursor_index(model, user_id, client_id);
    if (index == UMI_WORKBENCH_DESIGNER_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    for (move = index; move + 1U < model->count; ++move) {
        model->cursors[move] = model->cursors[move + 1U];
    }
    model->count -= 1U;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

size_t umi_workbench_designer_remote_cursor_prune(
    UmiWorkbenchDesignerRemoteCursorModel *model,
    uint64_t now_ms,
    uint64_t timeout_ms)
{
    size_t index = 0U;
    size_t removed = 0U;
    if (model == NULL || timeout_ms == 0U) return 0U;
    while (index < model->count) {
        const UmiWorkbenchDesignerRemoteCursor *cursor = &model->cursors[index];
        const bool expired = now_ms >= cursor->last_update_ms &&
            now_ms - cursor->last_update_ms >= timeout_ms;
        if (expired) {
            size_t move;
            for (move = index; move + 1U < model->count; ++move) {
                model->cursors[move] = model->cursors[move + 1U];
            }
            model->count -= 1U;
            removed += 1U;
        } else {
            index += 1U;
        }
    }
    if (removed > 0U) model->revision += 1U;
    return removed;
}

const UmiWorkbenchDesignerRemoteCursor *umi_workbench_designer_remote_cursor_find(
    const UmiWorkbenchDesignerRemoteCursorModel *model,
    const char *user_id,
    const char *client_id)
{
    const size_t index = remote_cursor_index(model, user_id, client_id);
    return index != UMI_WORKBENCH_DESIGNER_INDEX_NONE
        ? &model->cursors[index] : NULL;
}
