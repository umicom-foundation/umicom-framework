/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/context.c
 *
 * PURPOSE:
 *   Project the existing coding assistant repository context index.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/context.h"

#include <stdio.h>

#include "umicom/ai_developer_experience/action_ids.h"

UmiStatus umi_ai_developer_context_view_create(
    const char *view_id,
    UmiAiCodingAssistantService *assistant,
    size_t visible_rows,
    UmiUiViewModel **out_view)
{
    UmiAiCodingContextIndex *context;
    size_t total;
    size_t count;
    size_t index;
    UmiStatus status;

    if (assistant == NULL || visible_rows == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (visible_rows > UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY) {
        visible_rows = UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY;
    }

    context = umi_ai_coding_assistant_context(assistant);
    if (context == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ai_developer_view_create_base(
        view_id,
        "ai-developer-context",
        "AI Context",
        "Repository files eligible for governed coding prompts.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    total = umi_ai_coding_context_count(context);
    count = total > visible_rows ? visible_rows : total;

    status = umi_ai_developer_view_set_integer(
        *out_view, "ai-context.total-count", (int64_t)total);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view, "ai-context.row-count", (int64_t)count);

    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiAiCodingContextFile file;
        char key[96];
        char row[UMI_UI_VALUE_STRING_CAPACITY];

        status = umi_ai_coding_context_at(context, index, &file);
        if (status != UMI_STATUS_OK) break;

        (void)snprintf(key, sizeof(key), "ai-context.row.%zu", index);
        (void)snprintf(
            row,
            sizeof(row),
            "%.300s | %.64s | %u tokens | priority %u | %s",
            file.path,
            file.language_id,
            file.estimated_tokens,
            file.priority,
            file.active ? "active" : (file.enabled ? "enabled" : "disabled"));

        status = umi_ai_developer_view_set_string(*out_view, key, row);
    }

    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 0U,
            umi_ai_developer_action_open_overview(),
            "Overview",
            "Return to AI Developer overview",
            1);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 1U,
            umi_ai_developer_action_new_chat(),
            "New Chat",
            "Start a conversation using repository context",
            1);

    return status;
}
