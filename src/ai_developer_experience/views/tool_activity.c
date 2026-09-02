/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/tool_activity.c
 *
 * PURPOSE:
 *   Project recent controlled developer-tool results into a toolkit-neutral view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/tool_activity.h"

#include <stdio.h>

#include "umicom/ai_developer_experience/action_ids.h"

/*
 * Initialise ai developer tool activity view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_ai_developer_tool_activity_view_create(
    const char *view_id,
    const UmiAiCodingToolResultHistory *history,
    size_t visible_rows,
    UmiUiViewModel **out_view)
{
    size_t total;
    size_t count;
    size_t first;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || visible_rows == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this operation only while the related capability or state is available. */
    if (visible_rows > UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY) {
        visible_rows = UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY;
    }

    status = umi_ai_developer_view_create_base(
        view_id,
        "ai-developer-tool-activity",
        "AI Tool Activity",
        "Controlled developer-tool calls, state, status and bounded output evidence.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    total = umi_ai_coding_tool_result_history_count(history);
    count = total > visible_rows ? visible_rows : total;
    first = total - count;

    status = umi_ai_developer_view_set_integer(
        *out_view, "ai-tools.total-count", (int64_t)total);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view, "ai-tools.row-count", (int64_t)count);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiAiCodingToolResult result;
        char key[96];
        char row[UMI_UI_VALUE_STRING_CAPACITY];

        status = umi_ai_coding_tool_result_history_at(
            history, first + index, &result);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;

        (void)snprintf(key, sizeof(key), "ai-tools.row.%zu", index);
        (void)snprintf(
            row,
            sizeof(row),
            "%s | %s | status %u | %.330s",
            result.tool_id,
            umi_ai_coding_tool_call_state_text(result.state),
            (unsigned)result.status,
            result.output);

        status = umi_ai_developer_view_set_string(*out_view, key, row);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 0U,
            umi_ai_developer_action_open_policy(),
            "Policy",
            "Inspect tool capability and approval policy",
            1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 1U,
            umi_ai_developer_action_open_overview(),
            "Overview",
            "Return to AI Developer overview",
            1);

    return status;
}
