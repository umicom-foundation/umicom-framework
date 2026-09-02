/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/chat_persistence.h
 *
 * PURPOSE:
 *   Persist recent repository-aware coding chat sessions into UmiSessionStore
 *   using bounded line-safe chunks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_CHAT_PERSISTENCE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_CHAT_PERSISTENCE_H

#include "umicom/ai_developer_experience/persistence_codec.h"
#include "umicom/ai_coding_tools/chat_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_DEVELOPER_PERSISTED_CHAT_SESSIONS 3U
#define UMI_AI_DEVELOPER_PERSISTED_CHAT_MESSAGES 8U

/**
 * Write ai developer chat session in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_ai_developer_chat_session_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiCodingToolChatSession *session);

/**
 * Provide the ai developer chat session restore operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_chat_session_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiCodingToolChatSession *out_session,
    int *out_restored);

/**
 * Write ai developer chat registry in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_ai_developer_chat_registry_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiCodingToolChatRegistry *registry);

/**
 * Provide the ai developer chat registry restore operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_chat_registry_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiCodingToolChatRegistry *registry,
    size_t *out_restored_count);

#ifdef __cplusplus
}
#endif
#endif
