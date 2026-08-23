/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_activity_sync.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience activity sync.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/activity_sync.h"

int main(void)
{
    UmiAiCodingToolResultHistory *history = NULL;
    UmiAiDeveloperTimeline *timeline = NULL;
    UmiAiDeveloperActivityCursor cursor;
    UmiAiCodingToolResult result = {0};

    assert(umi_ai_coding_tool_result_history_create(&history) == UMI_STATUS_OK);
    assert(umi_ai_developer_timeline_create(&timeline) == UMI_STATUS_OK);
    umi_ai_developer_activity_cursor_init(&cursor);

    result.call_id = 1U;
    result.sequence = 1U;
    result.state = UMI_AI_CODING_TOOL_CALL_SUCCEEDED;
    result.status = UMI_STATUS_OK;
    (void)strcpy(result.tool_id, "workspace.read");

    assert(umi_ai_coding_tool_result_history_record(
        history, &result) == UMI_STATUS_OK);
    assert(umi_ai_developer_activity_sync_tools(
        &cursor, "task.1", history, timeline) == UMI_STATUS_OK);
    assert(umi_ai_developer_timeline_count(timeline) == 1U);

    assert(umi_ai_developer_activity_sync_tools(
        &cursor, "task.1", history, timeline) == UMI_STATUS_OK);
    assert(umi_ai_developer_timeline_count(timeline) == 1U);

    umi_ai_developer_timeline_destroy(timeline);
    umi_ai_coding_tool_result_history_destroy(history);
    return 0;
}

