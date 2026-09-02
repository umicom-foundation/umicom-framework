/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/chat.c
 *
 * PURPOSE:
 *   Project recent coding-chat messages into a toolkit-neutral transcript.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/chat.h"

#include <stdio.h>

#include "umicom/ai_developer_experience/action_ids.h"

/* Provide the role text operation used by this module and its client applications. */
static const char *role_text(UmiAiRole role)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (role) {
        case UMI_AI_ROLE_SYSTEM: return "system";
        case UMI_AI_ROLE_USER: return "user";
        case UMI_AI_ROLE_ASSISTANT: return "assistant";
        case UMI_AI_ROLE_TOOL: return "tool";
        default: return "unknown";
    }
}

/*
 * Initialise ai developer chat view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_chat_view_create(
    const char *view_id,
    const UmiAiCodingToolChatSession *session,
    size_t visible_rows,
    UmiUiViewModel **out_view)
{
    size_t count;
    size_t first;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || visible_rows == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this operation only while the related capability or state is available. */
    if (visible_rows > UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY) {
        visible_rows = UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY;
    }

    status = umi_ai_developer_view_create_base(
        view_id,
        "ai-developer-chat",
        "AI Coding Chat",
        "Repository-aware coding conversation with controlled developer tools.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    count =
        session->message_count > visible_rows
            ? visible_rows
            : session->message_count;
    first = session->message_count - count;

    status = umi_ai_developer_view_set_string(
        *out_view, "ai-chat.session-id", session->session_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_string(
            *out_view, "ai-chat.provider", session->provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_string(
            *out_view, "ai-chat.model", session->model_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view, "ai-chat.row-count", (int64_t)count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view, "ai-chat.turn-count", (int64_t)session->turn_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-chat.tool-result-count",
            (int64_t)session->tool_result_count);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiAiMessage *message = &session->messages[first + index];
        char key[96];
        char row[UMI_UI_VALUE_STRING_CAPACITY];

        (void)snprintf(key, sizeof(key), "ai-chat.row.%zu", index);
        (void)snprintf(
            row,
            sizeof(row),
            "%s | %.80s | %.380s",
            role_text(message->role),
            message->name,
            message->text);

        status = umi_ai_developer_view_set_string(*out_view, key, row);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 0U,
            umi_ai_developer_action_new_chat(),
            "New Chat",
            "Start another coding conversation",
            1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 1U,
            umi_ai_developer_action_open_context(),
            "Context",
            "Inspect repository context",
            1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 2U,
            umi_ai_developer_action_open_tools(),
            "Tools",
            "Inspect recent developer-tool calls",
            1);

    return status;
}
