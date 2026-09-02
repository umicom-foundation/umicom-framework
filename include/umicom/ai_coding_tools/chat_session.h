/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/chat_session.h
 *
 * PURPOSE:
 *   Retain repository-aware AI coding chat messages by value so conversation
 *   history is reusable across Studio, headless automation and future frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_CHAT_SESSION_H
#define UMICOM_AI_CODING_TOOLS_CHAT_SESSION_H

#include "umicom/ai/request.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CODING_TOOL_CHAT_MESSAGE_CAPACITY 48U

/**
 * Represent the ai coding tool chat session data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingToolChatSession {
    char session_id[UMI_AI_ID_CAPACITY];
    char provider_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    UmiAiMessage messages[UMI_AI_CODING_TOOL_CHAT_MESSAGE_CAPACITY];
    size_t message_count;
    uint64_t turn_count;
    uint64_t tool_result_count;
    uint64_t revision;
} UmiAiCodingToolChatSession;

/**
 * Initialise ai coding tool chat session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_coding_tool_chat_session_init(
    UmiAiCodingToolChatSession *session,
    const char *session_id,
    const char *provider_id,
    const char *model_id);

/**
 * Add ai coding tool chat only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ai_coding_tool_chat_add(
    UmiAiCodingToolChatSession *session,
    UmiAiRole role,
    const char *name,
    const char *text);

/**
 * Provide the ai coding tool chat add chunked operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_tool_chat_add_chunked(
    UmiAiCodingToolChatSession *session,
    UmiAiRole role,
    const char *name,
    const char *text);

/**
 * Provide the ai coding tool chat build request operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_chat_build_request(
    const UmiAiCodingToolChatSession *session,
    uint32_t max_output_tokens,
    double temperature,
    UmiAiRequest *out_request);

#ifdef __cplusplus
}
#endif
#endif
