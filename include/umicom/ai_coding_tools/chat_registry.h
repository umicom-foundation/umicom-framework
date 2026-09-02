/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/chat_registry.h
 *
 * PURPOSE:
 *   Retain multiple repository-aware coding chat sessions behind stable IDs so
 *   Studio can switch conversations without owning AI message storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_CHAT_REGISTRY_H
#define UMICOM_AI_CODING_TOOLS_CHAT_REGISTRY_H
#include "umicom/ai_coding_tools/chat_session.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CODING_TOOL_CHAT_SESSION_CAPACITY 8U

/**
 * Represent the ai coding tool chat registry data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingToolChatRegistry {
    UmiAiCodingToolChatSession sessions[UMI_AI_CODING_TOOL_CHAT_SESSION_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiAiCodingToolChatRegistry;

/**
 * Initialise ai coding tool chat registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_tool_chat_registry_init(
    UmiAiCodingToolChatRegistry *registry);

/**
 * Provide the ai coding tool chat registry open operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_chat_registry_open(
    UmiAiCodingToolChatRegistry *registry,
    const char *session_id,
    const char *provider_id,
    const char *model_id,
    UmiAiCodingToolChatSession **out_session);

/**
 * Find ai coding tool chat registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiAiCodingToolChatSession *umi_ai_coding_tool_chat_registry_find(
    UmiAiCodingToolChatRegistry *registry,
    const char *session_id);

/**
 * Provide the ai coding tool chat registry close operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_chat_registry_close(
    UmiAiCodingToolChatRegistry *registry,
    const char *session_id);

/**
 * Return the number of records represented by ai coding tool chat registry without
 * changing their state.
 */
size_t umi_ai_coding_tool_chat_registry_count(
    const UmiAiCodingToolChatRegistry *registry);

/**
 * Find ai coding tool chat registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ai_coding_tool_chat_registry_at(
    const UmiAiCodingToolChatRegistry *registry,
    size_t index,
    UmiAiCodingToolChatSession *out_session);

#ifdef __cplusplus
}
#endif
#endif
