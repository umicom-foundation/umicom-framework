/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/chat_registry.c
 *
 * PURPOSE:
 *   Implement bounded coding-chat session ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/chat_registry.h"

#include <string.h>

void umi_ai_coding_tool_chat_registry_init(
    UmiAiCodingToolChatRegistry *registry)
{
    if (registry == NULL) return;

    (void)memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
}

UmiAiCodingToolChatSession *umi_ai_coding_tool_chat_registry_find(
    UmiAiCodingToolChatRegistry *registry,
    const char *session_id)
{
    size_t index;

    if (registry == NULL || session_id == NULL) return NULL;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->sessions[index].session_id, session_id) == 0) {
            return &registry->sessions[index];
        }
    }

    return NULL;
}

UmiStatus umi_ai_coding_tool_chat_registry_open(
    UmiAiCodingToolChatRegistry *registry,
    const char *session_id,
    const char *provider_id,
    const char *model_id,
    UmiAiCodingToolChatSession **out_session)
{
    UmiAiCodingToolChatSession *session;
    UmiStatus status;

    if (registry == NULL || session_id == NULL ||
        provider_id == NULL || model_id == NULL ||
        out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_session = NULL;

    session = umi_ai_coding_tool_chat_registry_find(registry, session_id);
    if (session != NULL) {
        *out_session = session;
        return UMI_STATUS_OK;
    }

    if (registry->count >= UMI_AI_CODING_TOOL_CHAT_SESSION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    session = &registry->sessions[registry->count];

    status = umi_ai_coding_tool_chat_session_init(
        session,
        session_id,
        provider_id,
        model_id);
    if (status != UMI_STATUS_OK) return status;

    registry->count += 1U;
    registry->revision += 1U;
    *out_session = session;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_tool_chat_registry_close(
    UmiAiCodingToolChatRegistry *registry,
    const char *session_id)
{
    size_t index;

    if (registry == NULL || session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->sessions[index].session_id, session_id) == 0) {
            if (index + 1U < registry->count) {
                (void)memmove(
                    &registry->sessions[index],
                    &registry->sessions[index + 1U],
                    (registry->count - index - 1U) *
                        sizeof(registry->sessions[0]));
            }

            registry->count -= 1U;
            (void)memset(
                &registry->sessions[registry->count],
                0,
                sizeof(registry->sessions[0]));
            registry->revision += 1U;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}


size_t umi_ai_coding_tool_chat_registry_count(
    const UmiAiCodingToolChatRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

UmiStatus umi_ai_coding_tool_chat_registry_at(
    const UmiAiCodingToolChatRegistry *registry,
    size_t index,
    UmiAiCodingToolChatSession *out_session)
{
    if (registry == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;

    *out_session = registry->sessions[index];
    return UMI_STATUS_OK;
}
