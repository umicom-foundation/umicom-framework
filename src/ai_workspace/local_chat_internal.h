/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_chat_internal.h
 *
 * PURPOSE:
 *   Keep protocol codecs private while allowing focused wire-boundary regression tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_WORKSPACE_LOCAL_CHAT_INTERNAL_H
#define UMICOM_AI_WORKSPACE_LOCAL_CHAT_INTERNAL_H
#include "umicom/ai_workspace/providers.h"
/* Only the canonical local-provider function table may expose this snapshot.
 * Settings are copied; the existing cancellation token remains borrowed. */
typedef struct AwLocalProviderSettings {
    uint16_t port;
    uint32_t timeout_millis;
    const UmiAiWorkspaceCancellation *cancellation;
} AwLocalProviderSettings;
UmiStatus AwLocalChatProviderSettingsRead(const UmiAiProvider *provider,
    AwLocalProviderSettings *out_settings);
#define AW_HTTP_BODY_CAPACITY (128U * 1024U)
#define AW_HEALTH_BODY_CAPACITY 4096U
UmiStatus AwLocalChatEncode(const UmiAiRequest *request, char **outJson);
void AwLocalChatFree(char *json);
/* Streaming uses the same request admission and JSON ownership as full replies.
 * These private codecs do not start HTTP, bind a server session or publish UI.
 * Decode stages one delta; refusal preserves the entire output structure. */
/* Keep the original private record for comparison; an explicit usage-only tag preserves the distinction between no choices and an empty choice. The former implementation is retained for owner review. */
#if 0
typedef struct AwLocalChatDelta {
    char completion_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    char text[UMI_AI_TEXT_CAPACITY];
    UmiAiUsage usage;
    UmiAiFinishReason finish_reason;
    bool done, has_role, has_content, has_usage, has_finish;
} AwLocalChatDelta;
#endif
typedef struct AwLocalChatDelta {
    char completion_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    char text[UMI_AI_TEXT_CAPACITY];
    UmiAiUsage usage;
    UmiAiFinishReason finish_reason;
    bool usage_only; /* True only for a valid zero-choice usage record. */
    bool done, has_role, has_content, has_usage, has_finish;
} AwLocalChatDelta;
UmiStatus AwLocalChatEncodeStreaming(const UmiAiRequest *request,char **outJson);
/* Exact [DONE] is a marker, not evidence that a prior finish event existed.
 * JSON chunks require matching model, nonempty completion ID, chunk object
 * type, and one choice at index zero, or an empty choices usage-only record.
 * Consumer must bind ID across chunks and verify terminal order separately. */
UmiStatus AwLocalChatDeltaDecode(const UmiAiRequest *request,const char *json,
    size_t length,AwLocalChatDelta *outDelta);
UmiStatus AwLocalChatDecode(const UmiAiRequest *request, const char *json, size_t length, UmiAiResponse *response);
UmiStatus AwLocalHealthDecode(long httpStatus, const char *json, size_t length, UmiAiLocalHealth *outHealth);
#endif
