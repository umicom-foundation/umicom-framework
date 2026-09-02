/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/server.h
 *
 * PURPOSE:
 *   Validate and initialise MCP server metadata before a server enters the client registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_SERVER_H
#define UMICOM_AI_MCP_SERVER_H

#include "umicom/ai/mcp/transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise ai mcp server from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ai_mcp_server_init(
    UmiAiMcpServerDescriptor *server,
    const char *server_id,
    const char *display_name,
    const char *endpoint,
    UmiAiMcpTrust trust);

/**
 * Check that ai mcp server satisfies its contract before another service relies on it.
 */
UmiStatus umi_ai_mcp_server_validate(
    const UmiAiMcpServerDescriptor *server);

#ifdef __cplusplus
}
#endif
#endif
