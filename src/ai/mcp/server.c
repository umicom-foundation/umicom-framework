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
/* Metadata copying scans only the destination field bound before deciding whether the supplied text fits. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < capacity && source[length] != '\0') ++length;
    if (length == capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise ai mcp server from caller-provided values so later operations receive a known
 * state.
 */
/* Construct and validate metadata privately before replacing a caller value, preserving failures and borrowed field inputs. The former implementation is retained for owner review. */
#if 0
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

#endif
UmiStatus umi_ai_mcp_server_init(UmiAiMcpServerDescriptor *server, const char *server_id,
    const char *display_name, const char *endpoint, UmiAiMcpTrust trust)
{
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiMcpServerDescriptor candidate = {0};
    UmiStatus status = copy_text(candidate.server_id, sizeof(candidate.server_id), server_id);
    if (status == UMI_STATUS_OK) status = copy_text(candidate.display_name, sizeof(candidate.display_name), display_name);
    if (status == UMI_STATUS_OK) status = copy_text(candidate.endpoint, sizeof(candidate.endpoint), endpoint);
    if (status != UMI_STATUS_OK) return status;
    candidate.trust = trust; candidate.enabled = 1;
    status = umi_ai_mcp_server_validate(&candidate);
    if (status != UMI_STATUS_OK) return status;
    *server = candidate;
    return UMI_STATUS_OK;
}

/* Check that ai mcp server satisfies its contract before another service relies on it. */
/* Bounded server metadata admission prevents unterminated public fields from reaching registration and transport formatting. The former implementation is retained for owner review. */
#if 0
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

#endif
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
        memchr(server->server_id, '\0', sizeof(server->server_id)) == NULL ||
        memchr(server->display_name, '\0', sizeof(server->display_name)) == NULL ||
        memchr(server->endpoint, '\0', sizeof(server->endpoint)) == NULL ||
        memchr(server->permission_prefix, '\0', sizeof(server->permission_prefix)) == NULL ||
        server->trust < UMI_AI_MCP_TRUST_UNTRUSTED ||
        server->trust > UMI_AI_MCP_TRUST_VERIFIED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
