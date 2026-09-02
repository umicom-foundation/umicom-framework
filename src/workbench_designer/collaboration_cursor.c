/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/collaboration_cursor.c
 *
 * PURPOSE:
 *   Maintain ordered remote cursor state while rejecting stale sequence updates
 *   from delayed collaboration messages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/collaboration_cursor.h"
#include "internal.h"


/*
 * Initialise workbench designer remote cursor model from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_remote_cursor_model_init(
    UmiWorkbenchDesignerRemoteCursorModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
}

/*
 * Provide the remote cursor index operation used by this module and its client
 * applications.
 */
static size_t remote_cursor_index(
    const UmiWorkbenchDesignerRemoteCursorModel *model,
    const char *user_id,
    const char *client_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || user_id == NULL || client_id == NULL) {
        return UMI_WORKBENCH_DESIGNER_INDEX_NONE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->cursors[index].user_id, user_id) == 0 &&
            strcmp(model->cursors[index].client_id, client_id) == 0) {
            return index;
        }
    }
    return UMI_WORKBENCH_DESIGNER_INDEX_NONE;
}

/*
 * Provide the workbench designer remote cursor upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_remote_cursor_upsert(
    UmiWorkbenchDesignerRemoteCursorModel *model,
    const UmiWorkbenchDesignerRemoteCursor *cursor)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || cursor == NULL || cursor->user_id[0] == '\0' ||
        cursor->client_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = remote_cursor_index(model, cursor->user_id, cursor->client_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index != UMI_WORKBENCH_DESIGNER_INDEX_NONE) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (cursor->sequence < model->cursors[index].sequence) {
            return UMI_STATUS_INVALID_STATE;
        }
        model->cursors[index] = *cursor;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->count >= UMI_WORKBENCH_DESIGNER_MAX_REMOTE_CURSORS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        model->cursors[model->count++] = *cursor;
    }
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench designer remote cursor while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_workbench_designer_remote_cursor_remove(
    UmiWorkbenchDesignerRemoteCursorModel *model,
    const char *user_id,
    const char *client_id)
{
    size_t index;
    size_t move;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = remote_cursor_index(model, user_id, client_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_WORKBENCH_DESIGNER_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (move = index; move + 1U < model->count; ++move) {
        model->cursors[move] = model->cursors[move + 1U];
    }
    model->count -= 1U;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer remote cursor prune operation used by this module and its
 * client applications.
 */
size_t umi_workbench_designer_remote_cursor_prune(
    UmiWorkbenchDesignerRemoteCursorModel *model,
    uint64_t now_ms,
    uint64_t timeout_ms)
{
    size_t index = 0U;
    size_t removed = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || timeout_ms == 0U) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < model->count) {
        const UmiWorkbenchDesignerRemoteCursor *cursor = &model->cursors[index];
        const bool expired = now_ms >= cursor->last_update_ms &&
            now_ms - cursor->last_update_ms >= timeout_ms;
        /* Apply this branch only when its contract condition is satisfied. */
        if (expired) {
            size_t move;
            /* Visit each bounded item once so every record receives the same rule. */
            for (move = index; move + 1U < model->count; ++move) {
                model->cursors[move] = model->cursors[move + 1U];
            }
            model->count -= 1U;
            removed += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            index += 1U;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (removed > 0U) model->revision += 1U;
    return removed;
}

/*
 * Find workbench designer remote cursor while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerRemoteCursor *umi_workbench_designer_remote_cursor_find(
    const UmiWorkbenchDesignerRemoteCursorModel *model,
    const char *user_id,
    const char *client_id)
{
    const size_t index = remote_cursor_index(model, user_id, client_id);
    return index != UMI_WORKBENCH_DESIGNER_INDEX_NONE
        ? &model->cursors[index] : NULL;
}
