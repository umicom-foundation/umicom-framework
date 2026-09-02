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
