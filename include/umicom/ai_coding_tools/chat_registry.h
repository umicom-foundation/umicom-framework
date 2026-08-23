/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/chat_registry.h
 *
 * PURPOSE:
 *   Retain multiple repository-aware coding chat sessions behind stable IDs so
 *   Studio can switch conversations without owning AI message storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_CHAT_REGISTRY_H
#define UMICOM_AI_CODING_TOOLS_CHAT_REGISTRY_H
#include "umicom/ai_coding_tools/chat_session.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CODING_TOOL_CHAT_SESSION_CAPACITY 8U

typedef struct UmiAiCodingToolChatRegistry {
    UmiAiCodingToolChatSession sessions[UMI_AI_CODING_TOOL_CHAT_SESSION_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiAiCodingToolChatRegistry;

void umi_ai_coding_tool_chat_registry_init(
    UmiAiCodingToolChatRegistry *registry);

UmiStatus umi_ai_coding_tool_chat_registry_open(
    UmiAiCodingToolChatRegistry *registry,
    const char *session_id,
    const char *provider_id,
    const char *model_id,
    UmiAiCodingToolChatSession **out_session);

UmiAiCodingToolChatSession *umi_ai_coding_tool_chat_registry_find(
    UmiAiCodingToolChatRegistry *registry,
    const char *session_id);

UmiStatus umi_ai_coding_tool_chat_registry_close(
    UmiAiCodingToolChatRegistry *registry,
    const char *session_id);

size_t umi_ai_coding_tool_chat_registry_count(
    const UmiAiCodingToolChatRegistry *registry);

UmiStatus umi_ai_coding_tool_chat_registry_at(
    const UmiAiCodingToolChatRegistry *registry,
    size_t index,
    UmiAiCodingToolChatSession *out_session);

#ifdef __cplusplus
}
#endif
#endif
