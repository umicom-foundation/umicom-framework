/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/server.c
 *
 * PURPOSE:
 *   Implement bounded MCP server metadata initialisation and validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/server.h"

#include <string.h>

static UmiStatus copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;

    if (destination == NULL ||
        capacity == 0U ||
        source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_server_init(
    UmiAiMcpServerDescriptor *server,
    const char *server_id,
    const char *display_name,
    const char *endpoint,
    UmiAiMcpTrust trust)
{
    UmiStatus status;

    if (server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(server, 0, sizeof(*server));

    status = copy_text(
        server->server_id,
        sizeof(server->server_id),
        server_id);
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        server->display_name,
        sizeof(server->display_name),
        display_name);
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        server->endpoint,
        sizeof(server->endpoint),
        endpoint);
    if (status != UMI_STATUS_OK) return status;

    server->trust = trust;
    server->enabled = 1;

    return umi_ai_mcp_server_validate(server);
}

UmiStatus umi_ai_mcp_server_validate(
    const UmiAiMcpServerDescriptor *server)
{
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
