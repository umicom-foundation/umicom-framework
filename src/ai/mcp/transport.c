/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/transport.c
 *
 * PURPOSE:
 *   Implement validation and forwarding for the MCP transport callback boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/transport.h"
#include <stdlib.h>
#include <string.h>

/*
 * Provide the ai mcp transport request operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_transport_request(
    const UmiAiMcpTransport *transport,
    const char *method,
    const char *params_json,
    char *result_json,
    size_t result_capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transport == NULL ||
        transport->request == NULL ||
        method == NULL ||
        params_json == NULL ||
        result_json == NULL ||
        result_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return transport->request(
        transport->user_data,
        method,
        params_json,
        result_json,
        result_capacity);
}

/*
 * Provide the ai mcp transport notify operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_transport_notify(
    const UmiAiMcpTransport *transport,
    const char *method,
    const char *params_json)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transport == NULL ||
        method == NULL ||
        params_json == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transport->notify == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return transport->notify(
        transport->user_data,
        method,
        params_json);
}

/* Result text crosses an adapter boundary before C-string decoders consume it.
 * Stage bytes separately and require the adapter to write its terminator. The
 * nonzero fill distinguishes a completed empty result from no output at all.
 * This protects publication, not memory against an untrusted native callback. */
UmiStatus UmiAiMcpTransportRequestChecked(const UmiAiMcpTransport *transport,
    const char *method, const char *params_json, char *result_json,
    size_t result_capacity)
{
    if (transport == NULL || transport->request == NULL || method == NULL ||
        params_json == NULL || result_json == NULL || result_capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiAiMcpTransport selected = *transport;
    char *candidate = malloc(result_capacity);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memset(candidate, 0xff, result_capacity);
    UmiStatus status = selected.request(selected.user_data, method, params_json,
        candidate, result_capacity);
    if (status == UMI_STATUS_OK) {
        const char *end = memchr(candidate, '\0', result_capacity);
        if (end == NULL) status = UMI_STATUS_INTERNAL_ERROR;
        else memcpy(result_json, candidate, (size_t)(end - candidate) + 1U);
    }
    free(candidate);
    return status;
}
