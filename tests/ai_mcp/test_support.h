/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_support.h
 *
 * PURPOSE:
 *   Provide deterministic test transport fixtures for the Framework MCP client without external processes or network access.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_AI_MCP_SUPPORT_H
#define UMICOM_TEST_AI_MCP_SUPPORT_H

#include <stdio.h>
#include <string.h>

#include "umicom/ai/mcp/mcp.h"
#include "umicom/ai/runtime.h"

typedef struct UmiTestMcpTransportState {
    char last_method[128];
    char last_params[UMI_AI_MCP_TEXT_CAPACITY];
    size_t request_count;
    size_t notify_count;
    UmiStatus forced_status;
} UmiTestMcpTransportState;

static inline UmiStatus umi_test_mcp_request(
    void *user_data,
    const char *method,
    const char *params_json,
    char *result_json,
    size_t result_capacity)
{
    UmiTestMcpTransportState *state =
        (UmiTestMcpTransportState *)user_data;
    const char *result = "{}";

    if (state == NULL ||
        method == NULL ||
        params_json == NULL ||
        result_json == NULL ||
        result_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    state->request_count += 1U;
    (void)strncpy(
        state->last_method,
        method,
        sizeof(state->last_method) - 1U);
    (void)strncpy(
        state->last_params,
        params_json,
        sizeof(state->last_params) - 1U);

    if (state->forced_status != UMI_STATUS_OK) {
        return state->forced_status;
    }

    if (strcmp(method, "initialize") == 0) {
        result =
            "{\"protocolVersion\":\"2025-06-18\","
            "\"capabilities\":{\"tools\":{},\"resources\":{},\"prompts\":{}},"
            "\"serverInfo\":{\"name\":\"test-server\",\"version\":\"1.0\"}}";
    } else if (strcmp(method, "tools/list") == 0) {
        result =
            "{\"tools\":["
            "{\"name\":\"read_file\",\"description\":\"Read a file\","
            "\"inputSchema\":{\"type\":\"object\"}},"
            "{\"name\":\"write_file\",\"description\":\"Write a file\","
            "\"inputSchema\":{\"type\":\"object\"}}]}";
    } else if (strcmp(method, "resources/list") == 0) {
        result =
            "{\"resources\":["
            "{\"uri\":\"file:///one\",\"name\":\"One\",\"mimeType\":\"text/plain\"},"
            "{\"uri\":\"file:///two\",\"name\":\"Two\",\"mimeType\":\"text/plain\"}]}";
    } else if (strcmp(method, "prompts/list") == 0) {
        result =
            "{\"prompts\":["
            "{\"name\":\"review\",\"description\":\"Review code\"},"
            "{\"name\":\"explain\",\"description\":\"Explain code\"}]}";
    } else if (strcmp(method, "tools/call") == 0) {
        result = "{\"content\":[{\"type\":\"text\",\"text\":\"tool-ok\"}]}";
    } else if (strcmp(method, "resources/read") == 0) {
        result = "{\"contents\":[{\"uri\":\"file:///one\",\"text\":\"resource-ok\"}]}";
    } else if (strcmp(method, "prompts/get") == 0) {
        result = "{\"messages\":[{\"role\":\"user\",\"content\":\"prompt-ok\"}]}";
    }

    if (strlen(result) + 1U > result_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)strcpy(result_json, result);
    return UMI_STATUS_OK;
}

static inline UmiStatus umi_test_mcp_notify(
    void *user_data,
    const char *method,
    const char *params_json)
{
    UmiTestMcpTransportState *state =
        (UmiTestMcpTransportState *)user_data;

    if (state == NULL || method == NULL || params_json == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    state->notify_count += 1U;
    (void)strncpy(
        state->last_method,
        method,
        sizeof(state->last_method) - 1U);
    (void)strncpy(
        state->last_params,
        params_json,
        sizeof(state->last_params) - 1U);

    return state->forced_status;
}

static inline UmiStatus umi_test_mcp_server(
    UmiAiMcpServerDescriptor *server)
{
    UmiStatus status = umi_ai_mcp_server_init(
        server,
        "test",
        "Test MCP",
        "stdio://test",
        UMI_AI_MCP_TRUST_LOCAL);

    if (status == UMI_STATUS_OK) {
        (void)strcpy(
            server->permission_prefix,
            "mcp.test");
    }

    return status;
}

static inline UmiAiMcpTransport umi_test_mcp_transport(
    UmiTestMcpTransportState *state)
{
    UmiAiMcpTransport transport;

    transport.request = umi_test_mcp_request;
    transport.notify = umi_test_mcp_notify;
    transport.user_data = state;

    return transport;
}

static inline UmiStatus umi_test_mcp_ready_session(
    UmiAiMcpSession *session,
    UmiTestMcpTransportState *state)
{
    UmiAiMcpServerDescriptor server;
    UmiAiMcpTransport transport;
    UmiStatus status;

    (void)memset(state, 0, sizeof(*state));

    status = umi_test_mcp_server(&server);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    transport = umi_test_mcp_transport(state);

    status = umi_ai_mcp_session_init(
        session,
        &server,
        &transport);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return umi_ai_mcp_session_initialize(
        session,
        "test-client",
        "1.0");
}

#endif
