/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/session.h
 *
 * PURPOSE:
 *   Manage one MCP server lifecycle and negotiated capabilities without replacing the existing AI runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_SESSION_H
#define UMICOM_AI_MCP_SESSION_H

#include "umicom/ai/mcp/codec.h"
#include "umicom/ai/mcp/server_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiMcpSession {
    UmiAiMcpServerDescriptor server;
    UmiAiMcpTransport transport;
    UmiAiMcpServerCapabilities capabilities;
    UmiAiMcpSessionState state;
    uint64_t revision;
} UmiAiMcpSession;

/* Initialise a disconnected session from validated server metadata and transport. */
UmiStatus umi_ai_mcp_session_init(
    UmiAiMcpSession *session,
    const UmiAiMcpServerDescriptor *server,
    const UmiAiMcpTransport *transport);

/*
 * Send MCP initialize, decode negotiated capabilities and emit the initialized
 * notification when the transport supports notifications.
 */
UmiStatus umi_ai_mcp_session_initialize(
    UmiAiMcpSession *session,
    const char *client_name,
    const char *client_version);

/* Send MCP ping through the negotiated session. */
UmiStatus umi_ai_mcp_session_ping(
    UmiAiMcpSession *session);

/* Reset negotiated state while preserving server metadata and transport binding. */
void umi_ai_mcp_session_disconnect(
    UmiAiMcpSession *session);

#ifdef __cplusplus
}
#endif
#endif
