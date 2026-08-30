/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/activity_sync.c
 *
 * PURPOSE:
 *   Implement incremental timeline synchronization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/activity_sync.h"

#include <string.h>

void umi_ai_developer_activity_cursor_init(
    UmiAiDeveloperActivityCursor *cursor)
{
    if (cursor != NULL) {
        (void)memset(cursor, 0, sizeof(*cursor));
    }
}

UmiStatus umi_ai_developer_activity_sync_tools(
    UmiAiDeveloperActivityCursor *cursor,
    const char *task_id,
    const UmiAiCodingToolResultHistory *history,
    UmiAiDeveloperTimeline *timeline)
{
    size_t total;
    size_t index;

    if (cursor == NULL || task_id == NULL ||
        history == NULL || timeline == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    total = umi_ai_coding_tool_result_history_count(history);

    if (cursor->tool_result_count > total) {
        cursor->tool_result_count = 0U;
    }

    for (index = cursor->tool_result_count; index < total; ++index) {
        UmiAiCodingToolResult result;
        UmiAiDeveloperTimelineEvent event;
        UmiStatus status;

        status = umi_ai_coding_tool_result_history_at(
            history, index, &result);
        if (status != UMI_STATUS_OK) return status;

        status = umi_ai_developer_timeline_from_tool_result(
            task_id,
            &result,
            &event);
        if (status != UMI_STATUS_OK) return status;

        status = umi_ai_developer_timeline_append(timeline, &event);
        if (status != UMI_STATUS_OK) return status;
    }

    cursor->tool_result_count = total;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_activity_sync_validation(
    UmiAiDeveloperActivityCursor *cursor,
    const char *task_id,
    const UmiAiCodingValidationReport *report,
    UmiAiDeveloperTimeline *timeline)
{
    size_t index;

    if (cursor == NULL || task_id == NULL ||
        report == NULL || timeline == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (cursor->validation_revision != report->revision) {
        cursor->validation_revision = report->revision;
        cursor->validation_result_count = 0U;
    }

    for (index = cursor->validation_result_count;
         index < report->result_count;
         ++index) {
        UmiAiDeveloperTimelineEvent event;
        UmiStatus status =
            umi_ai_developer_timeline_from_validation(
                task_id,
                &report->results[index],
                cursor->validation_revision + index + 1U,
                &event);

        if (status != UMI_STATUS_OK) return status;

        status = umi_ai_developer_timeline_append(timeline, &event);
        if (status != UMI_STATUS_OK) return status;
    }

    cursor->validation_result_count = report->result_count;
    return UMI_STATUS_OK;
}
