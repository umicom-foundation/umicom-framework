/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/request.c
 *
 * PURPOSE:
 *   Initialise and populate bounded provider-neutral AI requests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A fixed message capacity keeps request ownership simple and produces an explicit error when a conversation must be summarised or trimmed.
 */

#include "umicom/ai/request.h"
#include <stddef.h>

#include <string.h>

void umi_ai_request_init(UmiAiRequest *request)
{
    if (request != NULL) {
        (void)memset(request, 0, sizeof(*request));
        request->max_output_tokens = 1024U;
        request->temperature = 0.2;
    }
}

UmiStatus umi_ai_request_add_message(UmiAiRequest *request,
                                     const UmiAiMessage *message)
{
    if (request == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (request->message_count >= UMI_AI_MAX_MESSAGES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    request->messages[request->message_count++] = *message;
    return UMI_STATUS_OK;
}
