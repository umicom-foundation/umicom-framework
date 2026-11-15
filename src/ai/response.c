/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/response.c
 *
 * PURPOSE:
 *   Initialise and fill bounded AI responses.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Providers fill the same response shape whether inference runs locally, through AuthorEngine, or through a remote API.
 */

#include "umicom/ai/response.h"
#include <stddef.h>

#include <stdio.h>
#include <string.h>

/*
 * Initialise ai response from caller-provided values so later operations receive a known
 * state.
 */
void umi_ai_response_init(UmiAiResponse *response)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (response != NULL) {
        (void)memset(response, 0, sizeof(*response));
        response->finish_reason = UMI_AI_FINISH_NONE;
    }
}

/*
 * Provide the ai response set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_response_set_text(UmiAiResponse *response, const char *text)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (response == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(response->text, sizeof(response->text), "%s", text);
    return written >= 0 && (size_t)written < sizeof(response->text)
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/* Validate provider-owned values before clients format or persist their text.
 * NONE remains valid for adapters which do not report a finish reason.
 * Token accounting is provider evidence and is not inferred from text length. */
UmiStatus UmiAiResponseValidate(const UmiAiResponse *response)
{
    if (response == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(response->request_id, '\0', sizeof(response->request_id)) == NULL ||
        memchr(response->provider_id, '\0', sizeof(response->provider_id)) == NULL ||
        memchr(response->model_id, '\0', sizeof(response->model_id)) == NULL ||
        memchr(response->text, '\0', sizeof(response->text)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    switch (response->finish_reason) {
    case UMI_AI_FINISH_NONE:
    case UMI_AI_FINISH_STOP:
    case UMI_AI_FINISH_LENGTH:
    case UMI_AI_FINISH_CANCELLED:
    case UMI_AI_FINISH_ERROR:
        return UMI_STATUS_OK;
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }
}
