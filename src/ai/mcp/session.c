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
#include <stdint.h>
#include <stdlib.h>

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
/* Checked transport stages and admits terminated result text before this high-level operation decodes or publishes it. The former implementation is retained for owner review. */
#if 0
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

#endif
/* Revision exhaustion is admitted before external work, and callback lifecycle writes belong only to the captured session revision and phase. The former implementation is retained for owner review. */
#if 0
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

    status = UmiAiMcpTransportRequestChecked(
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

#endif
/* A callback may retire or otherwise change the session while a request is
 * outstanding. Only the same revision in the expected phase accepts the next
 * lifecycle write. This is a reentry check, not a lease or thread lock. */
static int SessionOperationMatches(const UmiAiMcpSession *session,
    uint64_t revision, UmiAiMcpSessionState state)
{
    return session->revision == revision && session->state == state;
}

/* Stage negotiated capabilities and require the supported protocol version before notification. Prior revision-safe initialization is retained for owner review. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_session_initialize(UmiAiMcpSession *session,
    const char *client_name, const char *client_version)
{
    if (session == NULL || client_name == NULL || client_version == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state != UMI_AI_MCP_SESSION_CONNECTED) return UMI_STATUS_INVALID_STATE;
    if (session->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    const uint64_t revision = session->revision;
    char params[UMI_AI_MCP_TEXT_CAPACITY];
    char result[UMI_AI_MCP_TEXT_CAPACITY];
    UmiStatus status = umi_ai_mcp_encode_initialize_params(client_name, client_version,
        params, sizeof(params));
    if (status != UMI_STATUS_OK) {
        session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }
    status = UmiAiMcpTransportRequestChecked(&session->transport,
        umi_ai_mcp_method_initialize(), params, result, sizeof(result));
    if (status != UMI_STATUS_OK) {
        if (SessionOperationMatches(session, revision, UMI_AI_MCP_SESSION_CONNECTED))
            session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }
    if (!SessionOperationMatches(session, revision, UMI_AI_MCP_SESSION_CONNECTED))
        return UMI_STATUS_INVALID_STATE;
    status = umi_ai_mcp_decode_initialize_result(result, &session->capabilities);
    if (status != UMI_STATUS_OK) {
        session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }
    /* Preserve the established notification phase and negotiated capabilities.
     * Their publication still requires READY before normal session operations. */
    session->state = UMI_AI_MCP_SESSION_INITIALIZED;
    status = umi_ai_mcp_transport_notify(&session->transport,
        umi_ai_mcp_method_initialized(), "{}");
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_IMPLEMENTED) {
        if (SessionOperationMatches(session, revision, UMI_AI_MCP_SESSION_INITIALIZED))
            session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }
    if (!SessionOperationMatches(session, revision, UMI_AI_MCP_SESSION_INITIALIZED))
        return UMI_STATUS_INVALID_STATE;
    session->state = UMI_AI_MCP_SESSION_READY;
    session->revision = revision + 1U;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_mcp_session_initialize(UmiAiMcpSession *session,
    const char *client_name, const char *client_version)
{
    if (session == NULL || client_name == NULL || client_version == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state != UMI_AI_MCP_SESSION_CONNECTED) return UMI_STATUS_INVALID_STATE;
    if (session->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    const uint64_t revision = session->revision;
    char params[UMI_AI_MCP_TEXT_CAPACITY];
    char result[UMI_AI_MCP_TEXT_CAPACITY];
    UmiStatus status = umi_ai_mcp_encode_initialize_params(client_name, client_version,
        params, sizeof(params));
    if (status != UMI_STATUS_OK) {
        session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }
    status = UmiAiMcpTransportRequestChecked(&session->transport,
        umi_ai_mcp_method_initialize(), params, result, sizeof(result));
    if (status != UMI_STATUS_OK) {
        if (SessionOperationMatches(session, revision, UMI_AI_MCP_SESSION_CONNECTED))
            session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }
    if (!SessionOperationMatches(session, revision, UMI_AI_MCP_SESSION_CONNECTED))
        return UMI_STATUS_INVALID_STATE;
    UmiAiMcpServerCapabilities capabilities;
    status = umi_ai_mcp_decode_initialize_result(result, &capabilities);
    if (status == UMI_STATUS_OK && strcmp(capabilities.protocol_version, UMI_AI_MCP_PROTOCOL_VERSION) != 0)
        status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status != UMI_STATUS_OK) {
        session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }
    session->capabilities = capabilities;
    /* Preserve the established notification phase and negotiated capabilities.
     * Their publication still requires READY before normal session operations. */
    session->state = UMI_AI_MCP_SESSION_INITIALIZED;
    status = umi_ai_mcp_transport_notify(&session->transport,
        umi_ai_mcp_method_initialized(), "{}");
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_IMPLEMENTED) {
        if (SessionOperationMatches(session, revision, UMI_AI_MCP_SESSION_INITIALIZED))
            session->state = UMI_AI_MCP_SESSION_FAILED;
        return status;
    }
    if (!SessionOperationMatches(session, revision, UMI_AI_MCP_SESSION_INITIALIZED))
        return UMI_STATUS_INVALID_STATE;
    session->state = UMI_AI_MCP_SESSION_READY;
    session->revision = revision + 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the ai mcp session ping operation used by this module and its client
 * applications.
 */
/* Checked transport stages and admits terminated result text before this high-level operation decodes or publishes it. The former implementation is retained for owner review. */
#if 0
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

#endif
/* Ping completion now uses the same ready-session ownership admission as ordinary requests. The former implementation is retained for owner review. */
#if 0
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

    return UmiAiMcpTransportRequestChecked(
        &session->transport,
        umi_ai_mcp_method_ping(),
        "{}",
        result,
        sizeof(result));
}

#endif
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

    return UmiAiMcpSessionRequestChecked(
        session,
        umi_ai_mcp_method_ping(),
        "{}",
        result,
        sizeof(result));
}

/*
 * Provide the ai mcp session disconnect operation used by this module and its client
 * applications.
 */
/* Checked disconnect refuses revision exhaustion without erasing state; the void entry point delegates and preserves the owner on refusal. The former implementation is retained for owner review. */
#if 0
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

#endif
UmiStatus UmiAiMcpSessionDisconnectChecked(UmiAiMcpSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    memset(&session->capabilities, 0, sizeof(session->capabilities));
    session->state = UMI_AI_MCP_SESSION_DISCONNECTED;
    ++session->revision;
    return UMI_STATUS_OK;
}

void umi_ai_mcp_session_disconnect(UmiAiMcpSession *session)
{
    if (session != NULL) (void)UmiAiMcpSessionDisconnectChecked(session);
}

/* Complete text admission belongs to transport; ready-session ownership belongs
 * here. Keep a private result until both contracts have accepted the reply. */
UmiStatus UmiAiMcpSessionRequestChecked(UmiAiMcpSession *session,
    const char *method, const char *params_json, char *result_json, size_t result_capacity)
{
    if (session == NULL || method == NULL || params_json == NULL || result_json == NULL || result_capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state != UMI_AI_MCP_SESSION_READY) return UMI_STATUS_INVALID_STATE;
    const uint64_t revision = session->revision;
    const UmiAiMcpTransport binding = session->transport;
    if (binding.request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char *candidate = malloc(result_capacity);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiAiMcpTransportRequestChecked(&binding, method, params_json, candidate, result_capacity);
    if (status == UMI_STATUS_OK) {
        if (session->state != UMI_AI_MCP_SESSION_READY || session->revision != revision ||
            session->transport.request != binding.request || session->transport.notify != binding.notify ||
            session->transport.user_data != binding.user_data)
            status = UMI_STATUS_INVALID_STATE;
        else memcpy(result_json, candidate, strlen(candidate) + 1U);
    }
    /* A callback failure remains primary even when it also changed the session. */
    free(candidate);
    return status;
}
