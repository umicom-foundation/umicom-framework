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
