/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_construction.c
 *
 * PURPOSE:
 *   Verify staged MCP construction with borrowed inputs and unchanged refused destinations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "test_support.h"
#include <stddef.h>
#include <stdlib.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

typedef struct Field { const char *name; size_t offset, capacity; int required; } Field;
static const Field fields[] = {
    {"id", offsetof(UmiAiMcpServerDescriptor, server_id), UMI_AI_ID_CAPACITY, 1},
    {"display", offsetof(UmiAiMcpServerDescriptor, display_name), UMI_AI_SMALL_TEXT_CAPACITY, 1},
    {"endpoint", offsetof(UmiAiMcpServerDescriptor, endpoint), UMI_AI_SMALL_TEXT_CAPACITY, 1},
    {"permission", offsetof(UmiAiMcpServerDescriptor, permission_prefix), UMI_AI_ID_CAPACITY, 0}
};
static const Field *FieldNamed(const char *name)
{
    for (size_t i = 0U; i < sizeof(fields) / sizeof(fields[0]); ++i)
        if (strcmp(name, fields[i].name) == 0) return &fields[i];
    return NULL;
}
static int SameServer(const UmiAiMcpServerDescriptor *a, const UmiAiMcpServerDescriptor *b)
{
    return strcmp(a->server_id, b->server_id) == 0 && strcmp(a->display_name, b->display_name) == 0 &&
        strcmp(a->endpoint, b->endpoint) == 0 && strcmp(a->permission_prefix, b->permission_prefix) == 0 &&
        a->trust == b->trust && a->enabled == b->enabled && a->expected_capabilities == b->expected_capabilities;
}
static int ServerCase(const char *name, const char *field_name)
{
    UmiAiMcpServerDescriptor server, previous;
    memset(&server, 0x5a, sizeof(server));
    strcpy(server.server_id, "old-id"); strcpy(server.display_name, "Old display"); strcpy(server.endpoint, "stdio://old");
    const char *values[] = {"new-id", "New display", "stdio://new"};
    char expected_values[3][UMI_AI_SMALL_TEXT_CAPACITY];
    UmiAiMcpTrust trust = UMI_AI_MCP_TRUST_LOCAL;
    UmiStatus expected = UMI_STATUS_OK;
    char *bounded = NULL;
    const Field *field = strcmp(field_name, "all") == 0 ? NULL : FieldNamed(field_name);
    size_t index = field != NULL ? (size_t)(field - fields) : 0U;
    if (field != NULL) CHECK(index < 3U);
    if (strcmp(name, "basic") == 0) { /* Defaults replace a retired metadata value. */ }
    else if (strcmp(name, "untrusted") == 0) trust = UMI_AI_MCP_TRUST_UNTRUSTED;
    else if (strcmp(name, "verified") == 0) trust = UMI_AI_MCP_TRUST_VERIFIED;
    else if (strcmp(name, "trust-low") == 0) { trust = (UmiAiMcpTrust)-1; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "trust-high") == 0) { trust = (UmiAiMcpTrust)(UMI_AI_MCP_TRUST_VERIFIED + 1); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "null") == 0) { CHECK(field != NULL); values[index] = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "empty") == 0) { CHECK(field != NULL); values[index] = ""; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "last") == 0 || strcmp(name, "unterminated") == 0) {
        CHECK(field != NULL); bounded = malloc(field->capacity); CHECK(bounded != NULL);
        memset(bounded, 'x', field->capacity);
        if (strcmp(name, "last") == 0) bounded[field->capacity - 1U] = '\0';
        else expected = UMI_STATUS_CAPACITY_EXCEEDED;
        values[index] = bounded;
    } else if (strcmp(name, "alias") == 0) {
        CHECK(field != NULL); values[index] = (const char *)&server + field->offset;
    } else if (strcmp(name, "alias-all") == 0) {
        values[0] = server.server_id; values[1] = server.display_name; values[2] = server.endpoint;
    } else if (strcmp(name, "cross-alias") == 0) {
        values[0] = server.endpoint; values[1] = server.server_id; values[2] = server.display_name;
    } else if (strcmp(name, "null-output") != 0) return 2;
    if (expected == UMI_STATUS_OK) for (size_t i = 0U; i < 3U; ++i) strcpy(expected_values[i], values[i]);
    memcpy(&previous, &server, sizeof(server));
    UmiStatus status = umi_ai_mcp_server_init(strcmp(name, "null-output") == 0 ? NULL : &server,
        values[0], values[1], values[2], trust);
    free(bounded);
    if (strcmp(name, "null-output") == 0) expected = UMI_STATUS_INVALID_ARGUMENT;
    CHECK(status == expected);
    if (status != UMI_STATUS_OK) CHECK(memcmp(&server, &previous, sizeof(server)) == 0);
    else {
        CHECK(strcmp(server.server_id, expected_values[0]) == 0 && strcmp(server.display_name, expected_values[1]) == 0 && strcmp(server.endpoint, expected_values[2]) == 0);
        CHECK(server.permission_prefix[0] == '\0' && server.expected_capabilities == 0U && server.enabled == 1 && server.trust == trust);
        CHECK(umi_ai_mcp_server_validate(&server) == UMI_STATUS_OK);
    }
    return 0;
}

static int OwnerCase(int client_mode, const char *name, const char *field_name)
{
    size_t size = client_mode ? sizeof(UmiAiMcpClient) : sizeof(UmiAiMcpSession);
    void *output = malloc(size), *previous = malloc(size);
    if (output == NULL || previous == NULL) { free(output); free(previous); return 3; }
    memset(output, 0x5a, size);
    UmiAiMcpSession *session = client_mode ? &((UmiAiMcpClient *)output)->session : output;
    UmiAiMcpServerDescriptor server; CHECK(umi_test_mcp_server(&server) == UMI_STATUS_OK);
    UmiTestMcpTransportState state = {0}; UmiAiMcpTransport transport = umi_test_mcp_transport(&state);
    session->server = server; session->transport = transport;
    session->state = UMI_AI_MCP_SESSION_DISCONNECTED; session->revision = 17U;
    session->capabilities.flags = 0x5aU;
    strcpy(session->capabilities.protocol_version, "old");
    strcpy(session->capabilities.server_name, "old"); strcpy(session->capabilities.server_version, "old");
    const UmiAiMcpServerDescriptor *selected_server = &server;
    const UmiAiMcpTransport *selected_transport = &transport;
    const Field *field = strcmp(field_name, "all") == 0 ? NULL : FieldNamed(field_name);
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "basic") == 0) { /* Valid external metadata and binding. */ }
    else if (strcmp(name, "null-server") == 0) { selected_server = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "null-transport") == 0) { selected_transport = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "no-request") == 0) { transport.request = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "no-notify") == 0) transport.notify = NULL;
    else if (strcmp(name, "no-context") == 0) transport.user_data = NULL;
    else if (strcmp(name, "self-context") == 0) transport.user_data = session;
    else if (strcmp(name, "alias-server") == 0) selected_server = &session->server;
    else if (strcmp(name, "alias-transport") == 0) selected_transport = &session->transport;
    else if (strcmp(name, "alias-both") == 0) { selected_server = &session->server; selected_transport = &session->transport; }
    else if (strcmp(name, "metadata") == 0) {
        server.enabled = 0; server.expected_capabilities = UMI_AI_MCP_CAPABILITY_TOOLS | UMI_AI_MCP_CAPABILITY_RESOURCES;
        strcpy(server.permission_prefix, "permission.scope");
    } else if (strcmp(name, "trust-low") == 0) { server.trust = (UmiAiMcpTrust)-1; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "trust-high") == 0) { server.trust = (UmiAiMcpTrust)(UMI_AI_MCP_TRUST_VERIFIED + 1); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "empty") == 0 || strcmp(name, "unterminated") == 0 || strcmp(name, "last") == 0) {
        CHECK(field != NULL); char *text = (char *)&server + field->offset;
        if (strcmp(name, "empty") == 0) { text[0] = '\0'; if (field->required) expected = UMI_STATUS_INVALID_ARGUMENT; }
        else {
            memset(text, 'x', field->capacity);
            if (strcmp(name, "last") == 0) text[field->capacity - 1U] = '\0';
            else expected = UMI_STATUS_INVALID_ARGUMENT;
        }
    } else if (strcmp(name, "null-output") != 0) { free(output); free(previous); return 2; }
    UmiAiMcpServerDescriptor expected_server = {0}; UmiAiMcpTransport expected_transport = {0};
    if (expected == UMI_STATUS_OK) { expected_server = *selected_server; expected_transport = *selected_transport; }
    memcpy(previous, output, size);
    void *destination = strcmp(name, "null-output") == 0 ? NULL : output;
    UmiStatus status = client_mode ? umi_ai_mcp_client_init(destination, selected_server, selected_transport) :
        umi_ai_mcp_session_init(destination, selected_server, selected_transport);
    if (strcmp(name, "null-output") == 0) expected = UMI_STATUS_INVALID_ARGUMENT;
    int valid = status == expected && state.request_count == 0U && state.notify_count == 0U;
    if (status != UMI_STATUS_OK) valid = valid && memcmp(output, previous, size) == 0;
    else {
        valid = valid && SameServer(&session->server, &expected_server) &&
            session->transport.request == expected_transport.request && session->transport.notify == expected_transport.notify &&
            session->transport.user_data == expected_transport.user_data &&
            session->state == UMI_AI_MCP_SESSION_CONNECTED && session->revision == 1U &&
            session->capabilities.flags == 0U && session->capabilities.protocol_version[0] == '\0' &&
            session->capabilities.server_name[0] == '\0' && session->capabilities.server_version[0] == '\0';
        if (client_mode) {
            const UmiAiMcpClient *client = output;
            valid = valid && client->tools.count == 0U && client->tools.revision == 1U &&
                client->resources.count == 0U && client->resources.revision == 1U &&
                client->prompts.count == 0U && client->prompts.revision == 1U && client->tool_bridge.binding_count == 0U;
        }
    }
    free(output); free(previous); CHECK(valid); return 0;
}
int main(int argc, char **argv)
{
    if (argc != 4) return 2;
    if (strcmp(argv[1], "server") == 0) return ServerCase(argv[2], argv[3]);
    if (strcmp(argv[1], "session") == 0) return OwnerCase(0, argv[2], argv[3]);
    if (strcmp(argv[1], "client") == 0) return OwnerCase(1, argv[2], argv[3]);
    return 2;
}
