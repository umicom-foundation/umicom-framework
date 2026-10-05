/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_chat/plan.c
 * PURPOSE: Own reviewed request bytes and consume each approved attempt exactly once.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiProviderChatPrepare(UmiProviderConnections *store, const char *application, const char *profile,
                                 const char *connection_id, uint64_t revision, const char *prompt,
                                 const char *context, uint32_t max_output_tokens, UmiProviderChatPlan **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (max_output_tokens < 16U || max_output_tokens > 4096U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderChatPlan *plan = calloc(1U, sizeof(*plan));
    if (plan == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    atomic_init(&plan->consumed, false);
    plan->output_limit = max_output_tokens;
    UmiStatus status = UmiProviderConnectionCheckPrepare(store, application, profile, connection_id, revision,
                                                         &plan->connection);
    if (status == UMI_STATUS_OK)
    {
        plan->responses =
            strcmp(plan->connection.connection.endpoint, "https://api.openai.com/v1/responses") == 0;
        status = UmiProviderChatEncode(plan, prompt, context);
    }
    if (status != UMI_STATUS_OK)
    {
        UmiProviderChatDestroy(plan);
        return status;
    }
    *out = plan;
    return UMI_STATUS_OK;
}
const UmiProviderConnectionCheckPlan *UmiProviderChatConnection(const UmiProviderChatPlan *plan)
{
    return plan != NULL ? &plan->connection : NULL;
}
const char *UmiProviderChatInput(const UmiProviderChatPlan *plan)
{
    return plan != NULL ? plan->input : NULL;
}
uint32_t UmiProviderChatOutputLimit(const UmiProviderChatPlan *plan)
{
    return plan != NULL ? plan->output_limit : 0U;
}
void UmiProviderChatDestroy(UmiProviderChatPlan *plan)
{
    if (plan != NULL)
    {
        umi_secret_clear(plan, sizeof(*plan));
        free(plan);
    }
}
typedef struct ChatDispatch
{
    UmiProviderChatPlan *plan;
    UmiProviderChatResult *result;
    UmiProviderChatTransfer transfer;
} ChatDispatch;
/* The credential owner keeps the key alive only during this callback. This
 * owner separately retires the raw reply after complete, bounded decoding. */
static UmiStatus Dispatch(const UmiProviderConnectionCheckPlan *connection, const char *key,
                          const UmiCancellationToken *cancellation, void *data)
{
    ChatDispatch *chat = data;
    unsigned http = 0U;
    char *body = calloc(UMI_CONNECTION_CHECK_BODY_CAPACITY, 1U);
    if (body == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = chat->transfer(connection, chat->plan->body, key, cancellation, body,
                                      UMI_CONNECTION_CHECK_BODY_CAPACITY, &http);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancellation))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
    {
        const char *end = memchr(body, '\0', UMI_CONNECTION_CHECK_BODY_CAPACITY);
        status = end == NULL
                     ? UMI_STATUS_CAPACITY_EXCEEDED
                     : UmiProviderChatDecode(chat->plan->responses, body, (size_t)(end - body), chat->result);
    }
    chat->result->http_status = http;
    umi_secret_clear(body, UMI_CONNECTION_CHECK_BODY_CAPACITY);
    free(body);
    return status;
}
UmiStatus UmiProviderChatRunWith(UmiProviderChatPlan *plan, UmiProviderConnections *store, bool approved,
                                 const char *password, const UmiCancellationToken *cancellation,
                                 UmiProviderChatResult *out, UmiConnectionCheckSecretsFactory factory,
                                 UmiProviderChatTransfer transfer)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (plan == NULL || store == NULL || transfer == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    bool unused = false;
    /* Never replay automatically after an uncertain remote outcome. Competing
     * calls cannot both pass this gate even before credential verification. */
    if (!atomic_compare_exchange_strong(&plan->consumed, &unused, true))
        return UMI_STATUS_INVALID_STATE;
    ChatDispatch chat = {plan, out, transfer};
    return UmiConnectionAuthorize(&plan->connection, store, approved, password, cancellation, factory,
                                  Dispatch, &chat);
}
UmiStatus UmiProviderChatRun(UmiProviderChatPlan *plan, UmiProviderConnections *store, bool approved,
                             const char *password, const UmiCancellationToken *cancellation,
                             UmiProviderChatResult *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    if (plan == NULL || store == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiProviderConnectionCheckAvailable())
    {
        bool unused = false;
        if (!atomic_compare_exchange_strong(&plan->consumed, &unused, true))
            return UMI_STATUS_INVALID_STATE;
        return UMI_STATUS_UNAVAILABLE;
    }
    return UmiProviderChatRunWith(plan, store, approved, password, cancellation, out,
                                  UmiProfileSecretsPlatform, UmiConnectionHttpExchange);
}
