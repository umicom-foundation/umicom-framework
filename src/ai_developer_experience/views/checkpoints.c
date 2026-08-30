/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/checkpoints.c
 *
 * PURPOSE:
 *   Project workspace rollback checkpoint metadata without exposing retained
 *   source contents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/checkpoints.h"

#include <stdio.h>

#include "umicom/ai_developer_experience/action_ids.h"

UmiStatus umi_ai_developer_checkpoints_view_create(
    const char *view_id,
    const UmiAiCodingCheckpointStore *checkpoints,
    size_t visible_rows,
    UmiUiViewModel **out_view)
{
    size_t count;
    size_t first;
    size_t index;
    UmiStatus status;

    if (checkpoints == NULL || visible_rows == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (visible_rows > UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY) {
        visible_rows = UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY;
    }

    status = umi_ai_developer_view_create_base(
        view_id,
        "ai-developer-checkpoints",
        "AI Checkpoints",
        "Rollback checkpoints captured before approved multi-step workspace changes.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    count =
        checkpoints->count > visible_rows
            ? visible_rows
            : checkpoints->count;
    first = checkpoints->count - count;

    status = umi_ai_developer_view_set_integer(
        *out_view,
        "ai-checkpoints.total-count",
        (int64_t)checkpoints->count);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view, "ai-checkpoints.row-count", (int64_t)count);

    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiAiCodingCheckpoint *checkpoint =
            &checkpoints->checkpoints[first + index];
        char key[96];
        char row[UMI_UI_VALUE_STRING_CAPACITY];

        (void)snprintf(key, sizeof(key), "ai-checkpoints.row.%zu", index);
        (void)snprintf(
            row,
            sizeof(row),
            "%s | %.260s | files %zu | revision %llu",
            checkpoint->checkpoint_id,
            checkpoint->label,
            checkpoint->file_count,
            (unsigned long long)checkpoint->revision);

        status = umi_ai_developer_view_set_string(*out_view, key, row);
    }

    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 0U,
            umi_ai_developer_action_open_overview(),
            "Overview",
            "Return to AI Developer overview",
            1);

    return status;
}
