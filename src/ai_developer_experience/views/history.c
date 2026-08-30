/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/history.c
 *
 * PURPOSE:
 *   Project the unified AI developer timeline into a bounded history view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/history.h"

#include <stdio.h>

#include "umicom/ai_developer_experience/action_ids.h"

UmiStatus umi_ai_developer_history_view_create(
    const char *view_id,
    const UmiAiDeveloperTimeline *timeline,
    size_t visible_rows,
    UmiUiViewModel **out_view)
{
    size_t total;
    size_t count;
    size_t first;
    size_t index;
    UmiStatus status;

    if (timeline == NULL || visible_rows == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (visible_rows > UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY) {
        visible_rows = UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY;
    }

    status = umi_ai_developer_view_create_base(
        view_id,
        "ai-developer-history",
        "AI History",
        "Unified task, tool, validation, approval and patch activity.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    total = umi_ai_developer_timeline_count(timeline);
    count = total > visible_rows ? visible_rows : total;
    first = total - count;

    status = umi_ai_developer_view_set_integer(
        *out_view, "ai-history.total-count", (int64_t)total);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view, "ai-history.row-count", (int64_t)count);

    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiAiDeveloperTimelineEvent event;
        char key[96];
        char row[UMI_UI_VALUE_STRING_CAPACITY];

        status = umi_ai_developer_timeline_at(
            timeline, first + index, &event);
        if (status != UMI_STATUS_OK) break;

        (void)snprintf(key, sizeof(key), "ai-history.row.%zu", index);
        (void)snprintf(
            row,
            sizeof(row),
            "%s | %.160s | status %u | %.260s",
            event.category,
            event.title,
            (unsigned)event.status,
            event.detail);

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
