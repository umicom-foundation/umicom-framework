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

/*
 * Initialise ai developer activity cursor from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_developer_activity_cursor_init(
    UmiAiDeveloperActivityCursor *cursor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor != NULL) {
        (void)memset(cursor, 0, sizeof(*cursor));
    }
}

/*
 * Provide the ai developer activity sync tools operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_activity_sync_tools(
    UmiAiDeveloperActivityCursor *cursor,
    const char *task_id,
    const UmiAiCodingToolResultHistory *history,
    UmiAiDeveloperTimeline *timeline)
{
    size_t total;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL || task_id == NULL ||
        history == NULL || timeline == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    total = umi_ai_coding_tool_result_history_count(history);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (cursor->tool_result_count > total) {
        cursor->tool_result_count = 0U;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = cursor->tool_result_count; index < total; ++index) {
        UmiAiCodingToolResult result;
        UmiAiDeveloperTimelineEvent event;
        UmiStatus status;

        status = umi_ai_coding_tool_result_history_at(
            history, index, &result);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = umi_ai_developer_timeline_from_tool_result(
            task_id,
            &result,
            &event);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = umi_ai_developer_timeline_append(timeline, &event);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    cursor->tool_result_count = total;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer activity sync validation operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_activity_sync_validation(
    UmiAiDeveloperActivityCursor *cursor,
    const char *task_id,
    const UmiAiCodingValidationReport *report,
    UmiAiDeveloperTimeline *timeline)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL || task_id == NULL ||
        report == NULL || timeline == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this operation only while the related capability or state is available. */
    if (cursor->validation_revision != report->revision) {
        cursor->validation_revision = report->revision;
        cursor->validation_result_count = 0U;
    }

    /* Visit each bounded item once so every record receives the same rule. */
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

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = umi_ai_developer_timeline_append(timeline, &event);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    cursor->validation_result_count = report->result_count;
    return UMI_STATUS_OK;
}
