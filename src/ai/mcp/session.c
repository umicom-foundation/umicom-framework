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

/*
 * Initialise ai mcp session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_mcp_session_init(
    UmiAiMcpSession *session,
    const UmiAiMcpServerDescriptor *server,
    const UmiAiMcpTransport *transport)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL ||
        server == NULL ||
        transport == NULL ||
        transport->request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_mcp_server_validate(server);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Initialise ai mcp session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_mcp_session_initialize(
    UmiAiMcpSession *session,
    const char *client_name,
    const char *client_version)
{
    char params[UMI_AI_MCP_TEXT_CAPACITY];
    char result[UMI_AI_MCP_TEXT_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL ||
        client_name == NULL ||
        client_version == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->state != UMI_AI_MCP_SESSION_CONNECTED) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_mcp_encode_initialize_params(
        client_name,
        client_version,
        params,
        sizeof(params));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }

    status = umi_ai_mcp_decode_initialize_result(
        result,
        &session->capabilities);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }

    session->state = UMI_AI_MCP_SESSION_INITIALIZED;

    status = umi_ai_mcp_transport_notify(
        &session->transport,
        umi_ai_mcp_method_initialized(),
        "{}");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK &&
        status != UMI_STATUS_NOT_IMPLEMENTED) {
        session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }

    session->state = UMI_AI_MCP_SESSION_READY;
    session->revision += 1U;

    return UMI_STATUS_OK;
}

/*
 * Provide the ai mcp session ping operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_session_ping(
    UmiAiMcpSession *session)
{
    char result[UMI_AI_MCP_TEXT_CAPACITY];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the ai mcp session disconnect operation used by this module and its client
 * applications.
 */
void umi_ai_mcp_session_disconnect(
    UmiAiMcpSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
