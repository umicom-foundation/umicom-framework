/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/transport.c
 *
 * PURPOSE:
 *   Implement validation and forwarding for the MCP transport callback boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/transport.h"

UmiStatus umi_ai_mcp_transport_request(
    const UmiAiMcpTransport *transport,
    const char *method,
    const char *params_json,
    char *result_json,
    size_t result_capacity)
{
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

UmiStatus umi_ai_mcp_transport_notify(
    const UmiAiMcpTransport *transport,
    const char *method,
    const char *params_json)
{
    if (transport == NULL ||
        method == NULL ||
        params_json == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (transport->notify == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return transport->notify(
        transport->user_data,
        method,
        params_json);
}
