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
