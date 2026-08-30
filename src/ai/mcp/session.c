/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/session.c
 *
 * PURPOSE:
 *   Implement MCP initialize, ready-state, ping and disconnect transitions over the transport boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/session.h"

#include <string.h>

UmiStatus umi_ai_mcp_session_init(
    UmiAiMcpSession *session,
    const UmiAiMcpServerDescriptor *server,
    const UmiAiMcpTransport *transport)
{
    UmiStatus status;

    if (session == NULL ||
        server == NULL ||
        transport == NULL ||
        transport->request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_mcp_server_validate(server);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)memset(session, 0, sizeof(*session));
    session->server = *server;
    session->transport = *transport;
    session->state = UMI_AI_MCP_SESSION_CONNECTED;
    session->revision = 1U;

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_session_initialize(
    UmiAiMcpSession *session,
    const char *client_name,
    const char *client_version)
{
    char params[UMI_AI_MCP_TEXT_CAPACITY];
    char result[UMI_AI_MCP_TEXT_CAPACITY];
    UmiStatus status;

    if (session == NULL ||
        client_name == NULL ||
        client_version == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (session->state != UMI_AI_MCP_SESSION_CONNECTED) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_mcp_encode_initialize_params(
        client_name,
        client_version,
        params,
        sizeof(params));
    if (status != UMI_STATUS_OK) {
        session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }

    status = umi_ai_mcp_transport_request(
        &session->transport,
        umi_ai_mcp_method_initialize(),
        params,
        result,
        sizeof(result));
    if (status != UMI_STATUS_OK) {
        session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }

    status = umi_ai_mcp_decode_initialize_result(
        result,
        &session->capabilities);
    if (status != UMI_STATUS_OK) {
        session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }

    session->state = UMI_AI_MCP_SESSION_INITIALIZED;

    status = umi_ai_mcp_transport_notify(
        &session->transport,
        umi_ai_mcp_method_initialized(),
        "{}");
    if (status != UMI_STATUS_OK &&
        status != UMI_STATUS_NOT_IMPLEMENTED) {
        session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }

    session->state = UMI_AI_MCP_SESSION_READY;
    session->revision += 1U;

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_session_ping(
    UmiAiMcpSession *session)
{
    char result[UMI_AI_MCP_TEXT_CAPACITY];

    if (session == NULL ||
        session->state != UMI_AI_MCP_SESSION_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    return umi_ai_mcp_transport_request(
        &session->transport,
        umi_ai_mcp_method_ping(),
        "{}",
        result,
        sizeof(result));
}

void umi_ai_mcp_session_disconnect(
    UmiAiMcpSession *session)
{
    if (session == NULL) {
        return;
    }

    (void)memset(
        &session->capabilities,
        0,
        sizeof(session->capabilities));
    session->state = UMI_AI_MCP_SESSION_DISCONNECTED;
    session->revision += 1U;
}
