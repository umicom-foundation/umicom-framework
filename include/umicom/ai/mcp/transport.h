/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/transport.h
 *
 * PURPOSE:
 *   Define an MCP transport callback boundary so stdio, HTTP and future transports share the same client logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_TRANSPORT_H
#define UMICOM_AI_MCP_TRANSPORT_H

#include "umicom/ai/mcp/protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiAiMcpRequestFn)(
    void *user_data,
    const char *method,
    const char *params_json,
    char *result_json,
    size_t result_capacity);

typedef UmiStatus (*UmiAiMcpNotifyFn)(
    void *user_data,
    const char *method,
    const char *params_json);

/**
 * Represent the ai mcp transport data shared with callers of this public contract.
 */
typedef struct UmiAiMcpTransport {
    UmiAiMcpRequestFn request;
    UmiAiMcpNotifyFn notify;
    void *user_data;
} UmiAiMcpTransport;

/* Validate the transport and forward one MCP request to its adapter. */
UmiStatus umi_ai_mcp_transport_request(
    const UmiAiMcpTransport *transport,
    const char *method,
    const char *params_json,
    char *result_json,
    size_t result_capacity);

/* Forward an MCP notification when the adapter exposes notification support. */
UmiStatus umi_ai_mcp_transport_notify(
    const UmiAiMcpTransport *transport,
    const char *method,
    const char *params_json);

/* Snapshot the callback tuple, invoke into allocated staging storage, require
 * a terminator within result_capacity, then copy only the completed string.
 * Every failure preserves result_json. Callback errors remain the primary
 * status even if their unfinished bytes are malformed. An OK callback without
 * terminated output returns INTERNAL_ERROR; allocation refusal is OUT_OF_MEMORY.
 * Empty terminated text is allowed; this does not validate JSON or UTF-8.
 * Method/params are readable C strings and may refer to the old result buffer.
 * Result storage must not overlap transport. Keep all borrowed inputs and
 * callback context alive, serialize access, and return normally from callbacks.
 * Native callbacks remain trusted; this is not a lifetime guard or a sandbox.
 * The direct umi_ai_mcp_transport_request API retains callback error output. */
UmiStatus UmiAiMcpTransportRequestChecked(const UmiAiMcpTransport *transport,
    const char *method, const char *params_json, char *result_json,
    size_t result_capacity);

#ifdef __cplusplus
}
#endif
#endif
