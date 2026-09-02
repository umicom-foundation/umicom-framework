/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/server.c
 *
 * PURPOSE:
 *   Implement bounded MCP server metadata initialisation and validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/server.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL ||
        capacity == 0U ||
        source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise ai mcp server from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ai_mcp_server_init(
    UmiAiMcpServerDescriptor *server,
    const char *server_id,
    const char *display_name,
    const char *endpoint,
    UmiAiMcpTrust trust)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(server, 0, sizeof(*server));

    status = copy_text(
        server->server_id,
        sizeof(server->server_id),
        server_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        server->display_name,
        sizeof(server->display_name),
        display_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        server->endpoint,
        sizeof(server->endpoint),
        endpoint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    server->trust = trust;
    server->enabled = 1;

    return umi_ai_mcp_server_validate(server);
}

/* Check that ai mcp server satisfies its contract before another service relies on it. */
UmiStatus umi_ai_mcp_server_validate(
    const UmiAiMcpServerDescriptor *server)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL ||
        server->server_id[0] == '\0' ||
        server->display_name[0] == '\0' ||
        server->endpoint[0] == '\0' ||
        server->trust < UMI_AI_MCP_TRUST_UNTRUSTED ||
        server->trust > UMI_AI_MCP_TRUST_VERIFIED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
