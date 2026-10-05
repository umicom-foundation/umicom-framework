/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/provider_connections/chat.h
 * PURPOSE: Capture a single reviewed chat request without implicit workspace context or tool execution.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROVIDER_CONNECTIONS_CHAT_H
#define UMICOM_PROVIDER_CONNECTIONS_CHAT_H
#include "umicom/provider_connections/check.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_PROVIDER_CHAT_PROMPT_CAPACITY 4097U
#define UMI_PROVIDER_CHAT_CONTEXT_CAPACITY 8193U
#define UMI_PROVIDER_CHAT_TEXT_CAPACITY 16385U
    typedef struct UmiProviderChatPlan UmiProviderChatPlan;
    typedef struct UmiProviderChatResult
    {
        unsigned http_status;
        bool refused, truncated;
        char model[513];
        char text[UMI_PROVIDER_CHAT_TEXT_CAPACITY];
    } UmiProviderChatResult;
    /* Capture saved connection state and copy only the supplied prompt/context.
 * No files are read, credentials accessed or requests sent. Context may be
 * empty; all supplied text is part of one user message, never instructions
 * granting tools. Valid UTF-8 with tab/newline/CR is accepted; other ASCII
 * controls are refused. Output tokens must be 16..4096. *out is null on failure.
 * Model and endpoint compatibility remains the provider's decision. */
    UmiStatus UmiProviderChatPrepare(UmiProviderConnections *store, const char *application,
                                     const char *profile, const char *connection_id, uint64_t revision,
                                     const char *prompt, const char *context, uint32_t max_output_tokens,
                                     UmiProviderChatPlan **out);
    /* Borrowed review values stay valid until Destroy; do not modify them. The
 * input accessor returns the exact user-message text, including any context
 * separator. Show input, endpoint, model and limit before obtaining approval. */
    const UmiProviderConnectionCheckPlan *UmiProviderChatConnection(const UmiProviderChatPlan *plan);
    const char *UmiProviderChatInput(const UmiProviderChatPlan *plan);
    uint32_t UmiProviderChatOutputLimit(const UmiProviderChatPlan *plan);
    /* Each approved call consumes the plan, even if password verification or HTTP
 * fails. Prepare/review again to retry; never automatically resend a request
 * whose remote outcome is uncertain. Denial does not consume the plan.
 * This synchronous call belongs on a worker with its own Data Server. Keep
 * store, token and plan alive until return. Do not destroy during a call.
 * Result is cleared on entry. Replies are plain text, never executable actions.
 * Truncation/refusal are reported explicitly; no partial parse is published.
 * Remote requests use store:false; this does not override provider retention
 * policies. There is no history persistence, streaming, retry or file mutation. */
    UmiStatus UmiProviderChatRun(UmiProviderChatPlan *plan, UmiProviderConnections *store, bool approved,
                                 const char *password, const UmiCancellationToken *cancellation,
                                 UmiProviderChatResult *out);
    /* Clears copied prompt, context and request bytes. Caller owns the result and
 * must clear it when no longer needed. Destroy(NULL) is harmless. */
    void UmiProviderChatDestroy(UmiProviderChatPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
