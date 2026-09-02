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

/*
 * Initialise ai coding tool chat registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_tool_chat_registry_init(
    UmiAiCodingToolChatRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;

    (void)memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
}

/*
 * Find ai coding tool chat registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiAiCodingToolChatSession *umi_ai_coding_tool_chat_registry_find(
    UmiAiCodingToolChatRegistry *registry,
    const char *session_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || session_id == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->sessions[index].session_id, session_id) == 0) {
            return &registry->sessions[index];
        }
    }

    return NULL;
}

/*
 * Provide the ai coding tool chat registry open operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_chat_registry_open(
    UmiAiCodingToolChatRegistry *registry,
    const char *session_id,
    const char *provider_id,
    const char *model_id,
    UmiAiCodingToolChatSession **out_session)
{
    UmiAiCodingToolChatSession *session;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || session_id == NULL ||
        provider_id == NULL || model_id == NULL ||
        out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_session = NULL;

    session = umi_ai_coding_tool_chat_registry_find(registry, session_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session != NULL) {
        *out_session = session;
        return UMI_STATUS_OK;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_AI_CODING_TOOL_CHAT_SESSION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    session = &registry->sessions[registry->count];

    status = umi_ai_coding_tool_chat_session_init(
        session,
        session_id,
        provider_id,
        model_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    registry->count += 1U;
    registry->revision += 1U;
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding tool chat registry close operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_chat_registry_close(
    UmiAiCodingToolChatRegistry *registry,
    const char *session_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->sessions[index].session_id, session_id) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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


/*
 * Return the number of records represented by ai coding tool chat registry without
 * changing their state.
 */
size_t umi_ai_coding_tool_chat_registry_count(
    const UmiAiCodingToolChatRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Find ai coding tool chat registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ai_coding_tool_chat_registry_at(
    const UmiAiCodingToolChatRegistry *registry,
    size_t index,
    UmiAiCodingToolChatSession *out_session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;

    *out_session = registry->sessions[index];
    return UMI_STATUS_OK;
}
