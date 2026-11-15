/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/request.c
 *
 * PURPOSE:
 *   Initialise and populate bounded provider-neutral AI requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A fixed message capacity keeps request ownership simple and produces an explicit error when a conversation must be summarised or trimmed.
 */

#include "umicom/ai/request.h"
#include <stddef.h>
#include <math.h>

#include <string.h>

/*
 * Initialise ai request from caller-provided values so later operations receive a known
 * state.
 */
void umi_ai_request_init(UmiAiRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request != NULL) {
        (void)memset(request, 0, sizeof(*request));
        request->max_output_tokens = 1024U;
        request->temperature = 0.2;
    }
}

/*
 * Provide the ai request add message operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_request_add_message(UmiAiRequest *request,
                                     const UmiAiMessage *message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (request->message_count >= UMI_AI_MAX_MESSAGES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    request->messages[request->message_count++] = *message;
    return UMI_STATUS_OK;
}

/* A value boundary is shared by all providers. Bound the count before
 * inspecting active slots; unused slots are not part of the conversation.
 * Provider-specific requirements such as a nonempty model stay with providers. */
UmiStatus UmiAiRequestValidate(const UmiAiRequest *request)
{
    if (request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (request->message_count > UMI_AI_MAX_MESSAGES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(request->request_id, '\0', sizeof(request->request_id)) == NULL ||
        memchr(request->model_id, '\0', sizeof(request->model_id)) == NULL ||
        !isfinite(request->temperature))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < request->message_count; ++i) {
        const UmiAiMessage *message = &request->messages[i];
        if (message->role < UMI_AI_ROLE_SYSTEM || message->role > UMI_AI_ROLE_TOOL ||
            memchr(message->name, '\0', sizeof(message->name)) == NULL ||
            memchr(message->text, '\0', sizeof(message->text)) == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
