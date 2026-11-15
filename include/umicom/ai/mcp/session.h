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

/**
 * Represent the ai mcp session data shared with callers of this public contract.
 */
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
/* Initialization refuses an exhausted revision before encoding or transport.
 * Callback completion must still match the captured revision and expected
 * phase before Framework publishes FAILED or READY. Callback errors retain
 * their status even when the callback changed the session. Keep the session
 * alive and serialized; this observation is not a lock or callback sandbox.
 * Malformed or unsupported negotiation retains prior capabilities and enters
 * FAILED without notification. An unsupported protocol returns NOT_IMPLEMENTED;
 * transport shutdown remains the host's responsibility. */
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

/* Reset negotiated state only when a new revision can be represented. NULL is
 * INVALID_ARGUMENT; UINT64_MAX returns CAPACITY_EXCEEDED without any mutation.
 * Server metadata and transport binding are retained. The void compatibility
 * disconnect delegates here and leaves the complete session intact on refusal.
 * Retire all borrowed uses before constructing a fresh session lifetime. */
UmiStatus UmiAiMcpSessionDisconnectChecked(UmiAiMcpSession *session);

#ifdef __cplusplus
}
#endif
#endif
