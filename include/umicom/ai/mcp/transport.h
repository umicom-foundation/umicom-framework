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

#ifdef __cplusplus
}
#endif
#endif
