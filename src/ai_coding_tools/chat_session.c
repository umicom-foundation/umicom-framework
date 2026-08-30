/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/chat_session.c
 *
 * PURPOSE:
 *   Implement bounded owned coding-chat history and provider request projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/chat_session.h"
#include "umicom/ai/coding_types.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *out, size_t capacity, const char *value)
{
    size_t length;

    if (out == NULL || capacity == 0U || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(value);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_tool_chat_session_init(
    UmiAiCodingToolChatSession *session,
    const char *session_id,
    const char *provider_id,
    const char *model_id)
{
    UmiStatus status;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    (void)memset(session, 0, sizeof(*session));

    status = copy_text(session->session_id, sizeof(session->session_id), session_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            session->provider_id, sizeof(session->provider_id), provider_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            session->model_id, sizeof(session->model_id), model_id);
    }
    if (status != UMI_STATUS_OK) return status;

    session->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_tool_chat_add(
    UmiAiCodingToolChatSession *session,
    UmiAiRole role,
    const char *name,
    const char *text)
{
    UmiStatus status;

    if (session == NULL || name == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (session->message_count >= UMI_AI_CODING_TOOL_CHAT_MESSAGE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_ai_message_set(
        &session->messages[session->message_count],
        role,
        name,
        text);
    if (status != UMI_STATUS_OK) return status;

    session->message_count += 1U;
    if (role == UMI_AI_ROLE_USER) session->turn_count += 1U;
    if (role == UMI_AI_ROLE_TOOL) session->tool_result_count += 1U;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_tool_chat_add_chunked(
    UmiAiCodingToolChatSession *session,
    UmiAiRole role,
    const char *name,
    const char *text)
{
    const size_t chunk_capacity = UMI_AI_TEXT_CAPACITY - 1U;
    const size_t length = text != NULL ? strlen(text) : 0U;
    size_t offset = 0U;

    if (session == NULL || name == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (length == 0U) {
        return umi_ai_coding_tool_chat_add(session, role, name, "");
    }

    while (offset < length) {
        char chunk[UMI_AI_TEXT_CAPACITY];
        const size_t remaining = length - offset;
        const size_t count =
            remaining < chunk_capacity ? remaining : chunk_capacity;
        UmiStatus status;

        (void)memcpy(chunk, text + offset, count);
        chunk[count] = '\0';

        status = umi_ai_coding_tool_chat_add(
            session, role, name, chunk);
        if (status != UMI_STATUS_OK) return status;

        offset += count;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_tool_chat_build_request(
    const UmiAiCodingToolChatSession *session,
    uint32_t max_output_tokens,
    double temperature,
    UmiAiRequest *out_request)
{
    size_t index;
    UmiStatus status;

    if (session == NULL || out_request == NULL ||
        max_output_tokens == 0U ||
        temperature < 0.0 || temperature > 2.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_ai_request_init(out_request);

    (void)snprintf(
        out_request->request_id,
        sizeof(out_request->request_id),
        "chat.%016llx.turn.%llu",
        (unsigned long long)umi_ai_coding_text_hash(
            session->session_id,
            strlen(session->session_id)),
        (unsigned long long)(session->turn_count + 1U));
    (void)snprintf(
        out_request->model_id,
        sizeof(out_request->model_id),
        "%s",
        session->model_id);

    out_request->max_output_tokens = max_output_tokens;
    out_request->temperature = temperature;
    out_request->allow_tools = 1;

    for (index = 0U; index < session->message_count; ++index) {
        status = umi_ai_request_add_message(
            out_request,
            &session->messages[index]);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
