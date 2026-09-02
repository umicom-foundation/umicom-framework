/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_request_internal.c
 *
 * PURPOSE:
 *   Implement the dap request internal behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework DAP bounded request helpers.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "dap_request_internal.h"
#include <stdio.h>
#include "umicom/protocol/json.h"

/* Provide the dap escape operation used by this module and its client applications. */
UmiStatus umi_dap_escape(const char *text, char *out, size_t capacity)
{
    return text != NULL && out != NULL && capacity > 0U
        ? umi_json_escape(text, out, capacity)
        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Provide the dap request operation used by this module and its client applications. */
UmiStatus umi_dap_request(UmiDapClient *client, const char *command,
                          const char *arguments_json,
                          int64_t *out_request_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (client == NULL || client->client == NULL || command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_protocol_client_request(client->client, command,
        arguments_json != NULL ? arguments_json : "{}", out_request_id);
}

/*
 * Provide the dap thread request operation used by this module and its client
 * applications.
 */
UmiStatus umi_dap_thread_request(UmiDapClient *client, const char *command,
                                 int thread_id, const char *suffix_json,
                                 int64_t *out_request_id)
{
    char arguments[512];
    const char *suffix = suffix_json != NULL ? suffix_json : "";
    int written;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (thread_id < 0) return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(arguments, sizeof(arguments),
                       "{\"threadId\":%d%s}", thread_id, suffix);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(arguments)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_dap_request(client, command, arguments, out_request_id);
}
