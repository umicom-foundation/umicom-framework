/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_discovery_publication.c
 *
 * PURPOSE:
 *   Verify unchanged MCP catalogues after failed discovery and composed refresh.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdlib.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

void UmiTestMcpDiscoveryFailAllocation(size_t fail_at);
UmiStatus UmiTestMcpDiscoverTools(UmiAiMcpSession *, UmiAiMcpToolCatalogue *);
UmiStatus UmiTestMcpDiscoverResources(UmiAiMcpSession *, UmiAiMcpResourceCatalogue *);
UmiStatus UmiTestMcpDiscoverPrompts(UmiAiMcpSession *, UmiAiMcpPromptCatalogue *);
UmiStatus UmiTestMcpClientDiscover(UmiAiMcpClient *);

typedef struct Fixture {
    UmiAiMcpClient client;
    UmiAiMcpClient saved;
    UmiTestMcpTransportState setup_transport;
    const char *mode;
    const char *selected;
    unsigned calls;
} Fixture;

static size_t Limit(const char *kind)
{
    return strcmp(kind, "tools") == 0 ? UMI_AI_MCP_MAX_TOOLS :
        strcmp(kind, "resources") == 0 ? UMI_AI_MCP_MAX_RESOURCES : UMI_AI_MCP_MAX_PROMPTS;
}

static UmiStatus Request(void *data, const char *method, const char *params,
    char *output, size_t capacity)
{
    Fixture *fixture = data;
    if (strcmp(method, "initialize") == 0)
        return umi_test_mcp_request(&fixture->setup_transport, method, params, output, capacity);
    ++fixture->calls;
    const char *kind = strcmp(method, "tools/list") == 0 ? "tools" :
        strcmp(method, "resources/list") == 0 ? "resources" : "prompts";
    const char *mode = strcmp(fixture->selected, kind) == 0 ? fixture->mode : "success";
    if (strcmp(mode, "error") == 0) return UMI_STATUS_IO_ERROR;
    if (strcmp(mode, "revision") == 0) ++fixture->client.session.revision;
    if (strcmp(mode, "disconnect") == 0) fixture->client.session.state = UMI_AI_MCP_SESSION_DISCONNECTED;
    if (strcmp(mode, "flags") == 0) fixture->client.session.capabilities.flags ^= UMI_AI_MCP_CAPABILITY_RESOURCES;
    if (strcmp(mode, "missing") == 0) {
        if (capacity < 3U) return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(output, "{}", 3U); return UMI_STATUS_OK;
    }
    size_t count = strcmp(mode, "empty") == 0 ? 0U : strcmp(mode, "maximum") == 0 ? Limit(kind) :
        strcmp(mode, "overflow") == 0 ? Limit(kind) + 1U : strcmp(mode, "duplicate") == 0 ? 2U : 1U;
    int written = snprintf(output, capacity, "{\"%s\":[", kind);
    if (written < 0 || (size_t)written >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t used = (size_t)written;
    for (size_t index = 0U; index < count; ++index) {
        written = snprintf(output + used, capacity - used, "%s{\"%s\":\"new-%zu\"}",
            index == 0U ? "" : ",", strcmp(kind, "resources") == 0 ? "uri" : "name",
            strcmp(mode, "duplicate") == 0 ? 0U : index);
        if (written < 0 || (size_t)written >= capacity - used) return UMI_STATUS_CAPACITY_EXCEEDED;
        used += (size_t)written;
    }
    const char *tail = strcmp(mode, "late-invalid") == 0 ? ",{}]}" : strcmp(mode, "truncated") == 0 ? "," : "]}";
    written = snprintf(output + used, capacity - used, "%s", tail);
    return written >= 0 && (size_t)written < capacity - used ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

static int Setup(Fixture *fixture)
{
    UmiAiMcpServerDescriptor server;
    CHECK(umi_test_mcp_server(&server) == UMI_STATUS_OK);
    UmiAiMcpTransport transport = {Request, NULL, fixture};
    CHECK(umi_ai_mcp_client_init(&fixture->client, &server, &transport) == UMI_STATUS_OK);
    CHECK(umi_ai_mcp_client_start(&fixture->client, "fixture", "1") == UMI_STATUS_OK);
    UmiAiMcpToolDescriptor tool = {0}; strcpy(tool.name, "old-tool");
    UmiAiMcpResourceDescriptor resource = {0}; strcpy(resource.uri, "old:resource");
    UmiAiMcpPromptDescriptor prompt = {0}; strcpy(prompt.name, "old-prompt");
    CHECK(umi_ai_mcp_tool_catalogue_add(&fixture->client.tools, &tool) == UMI_STATUS_OK);
    CHECK(umi_ai_mcp_resource_catalogue_add(&fixture->client.resources, &resource) == UMI_STATUS_OK);
    CHECK(umi_ai_mcp_prompt_catalogue_add(&fixture->client.prompts, &prompt) == UMI_STATUS_OK);
    fixture->client.tools.revision = 31U;
    fixture->client.resources.revision = 41U;
    fixture->client.prompts.revision = 51U;
    memset(&fixture->client.tool_bridge, 0x5a, sizeof(fixture->client.tool_bridge));
    fixture->mode = "success"; fixture->selected = "tools";
    return 0;
}

static int Preserved(Fixture *fixture)
{
    CHECK(memcmp(&fixture->client.tools, &fixture->saved.tools, sizeof(fixture->client.tools)) == 0);
    CHECK(memcmp(&fixture->client.resources, &fixture->saved.resources, sizeof(fixture->client.resources)) == 0);
    CHECK(memcmp(&fixture->client.prompts, &fixture->saved.prompts, sizeof(fixture->client.prompts)) == 0);
    CHECK(memcmp(&fixture->client.tool_bridge, &fixture->saved.tool_bridge, sizeof(fixture->client.tool_bridge)) == 0);
    return 0;
}

static void *Catalogue(Fixture *fixture, const char *kind, size_t *size)
{
    if (strcmp(kind, "tools") == 0) { *size = sizeof(fixture->client.tools); return &fixture->client.tools; }
    if (strcmp(kind, "resources") == 0) { *size = sizeof(fixture->client.resources); return &fixture->client.resources; }
    *size = sizeof(fixture->client.prompts); return &fixture->client.prompts;
}

static uint32_t Capability(const char *kind)
{
    return strcmp(kind, "tools") == 0 ? UMI_AI_MCP_CAPABILITY_TOOLS :
        strcmp(kind, "resources") == 0 ? UMI_AI_MCP_CAPABILITY_RESOURCES : UMI_AI_MCP_CAPABILITY_PROMPTS;
}

static int Published(Fixture *fixture, const char *kind, size_t count)
{
    if (strcmp(kind, "tools") == 0) {
        CHECK(fixture->client.tools.count == count && fixture->client.tools.revision == count + 1U);
        if (count != 0U) CHECK(strcmp(fixture->client.tools.items[0].name, "new-0") == 0);
    } else if (strcmp(kind, "resources") == 0) {
        CHECK(fixture->client.resources.count == count && fixture->client.resources.revision == count + 1U);
        if (count != 0U) CHECK(strcmp(fixture->client.resources.items[0].uri, "new-0") == 0);
    } else {
        CHECK(fixture->client.prompts.count == count && fixture->client.prompts.revision == count + 1U);
        if (count != 0U) CHECK(strcmp(fixture->client.prompts.items[0].name, "new-0") == 0);
    }
    CHECK(memcmp(&fixture->client.tool_bridge, &fixture->saved.tool_bridge, sizeof(fixture->client.tool_bridge)) == 0);
    return 0;
}

static UmiStatus Expected(const char *mode)
{
    if (strcmp(mode, "error") == 0) return UMI_STATUS_IO_ERROR;
    if (strcmp(mode, "missing") == 0) return UMI_STATUS_NOT_FOUND;
    if (strcmp(mode, "duplicate") == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (strcmp(mode, "late-invalid") == 0 || strcmp(mode, "truncated") == 0) return UMI_STATUS_PARSE_ERROR;
    if (strcmp(mode, "overflow") == 0) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(mode, "allocation") == 0) return UMI_STATUS_OUT_OF_MEMORY;
    if (strcmp(mode, "revision") == 0 || strcmp(mode, "disconnect") == 0 ||
        strcmp(mode, "not-ready") == 0 || strcmp(mode, "null-session") == 0 || strcmp(mode, "flags") == 0)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

static int Single(Fixture *fixture, const char *kind, const char *mode)
{
    fixture->selected = kind; fixture->mode = mode;
    size_t bytes = 0U;
    if (strcmp(mode, "fresh") == 0) {
        void *catalogue = Catalogue(fixture, kind, &bytes); memset(catalogue, 0x71, bytes);
    }
    if (strcmp(mode, "not-ready") == 0) fixture->client.session.state = UMI_AI_MCP_SESSION_DISCONNECTED;
    memcpy(&fixture->saved, &fixture->client, sizeof(fixture->saved));
    UmiAiMcpSession *session = strcmp(mode, "null-session") == 0 ? NULL : &fixture->client.session;
    const int fault = strcmp(mode, "allocation") == 0;
    if (fault) UmiTestMcpDiscoveryFailAllocation(1U);
    UmiStatus status;
    if (strcmp(kind, "tools") == 0) status = fault ? UmiTestMcpDiscoverTools(session, &fixture->client.tools) : umi_ai_mcp_discover_tools(session, &fixture->client.tools);
    else if (strcmp(kind, "resources") == 0) status = fault ? UmiTestMcpDiscoverResources(session, &fixture->client.resources) : umi_ai_mcp_discover_resources(session, &fixture->client.resources);
    else status = fault ? UmiTestMcpDiscoverPrompts(session, &fixture->client.prompts) : umi_ai_mcp_discover_prompts(session, &fixture->client.prompts);
    CHECK(status == Expected(mode));
    CHECK(fixture->calls == ((fault || session == NULL || strcmp(mode, "not-ready") == 0) ? 0U : 1U));
    if (status != UMI_STATUS_OK) CHECK(Preserved(fixture) == 0);
    else CHECK(Published(fixture, kind, strcmp(mode, "empty") == 0 ? 0U : strcmp(mode, "maximum") == 0 ? Limit(kind) : 1U) == 0);
    return 0;
}

static int Client(Fixture *fixture, const char *kind, const char *mode)
{
    fixture->selected = kind; fixture->mode = mode;
    if (strcmp(mode, "subset") == 0) fixture->client.session.capabilities.flags = Capability(kind);
    if (strcmp(mode, "none") == 0) fixture->client.session.capabilities.flags = 0U;
    memcpy(&fixture->saved, &fixture->client, sizeof(fixture->saved));
    UmiStatus status;
    size_t failure_at = 0U;
    if (strncmp(mode, "allocation-", 11U) == 0) {
        failure_at = (size_t)strtoul(mode + 11U, NULL, 10);
        CHECK(failure_at >= 1U && failure_at <= 4U);
        UmiTestMcpDiscoveryFailAllocation(failure_at);
        status = UmiTestMcpClientDiscover(&fixture->client);
    } else status = umi_ai_mcp_client_discover(&fixture->client);
    CHECK(status == (failure_at != 0U ? UMI_STATUS_OUT_OF_MEMORY : Expected(mode)));
    if (status != UMI_STATUS_OK) {
        CHECK(Preserved(fixture) == 0);
        if (failure_at != 0U) CHECK(fixture->calls == (failure_at < 2U ? 0U : failure_at - 2U));
    } else if (strcmp(mode, "none") == 0) { CHECK(fixture->calls == 0U); CHECK(Preserved(fixture) == 0); }
    else if (strcmp(mode, "subset") == 0) {
        CHECK(fixture->calls == 1U); CHECK(Published(fixture, kind, 1U) == 0);
        if (strcmp(kind, "tools") != 0) CHECK(memcmp(&fixture->client.tools, &fixture->saved.tools, sizeof(fixture->client.tools)) == 0);
        if (strcmp(kind, "resources") != 0) CHECK(memcmp(&fixture->client.resources, &fixture->saved.resources, sizeof(fixture->client.resources)) == 0);
        if (strcmp(kind, "prompts") != 0) CHECK(memcmp(&fixture->client.prompts, &fixture->saved.prompts, sizeof(fixture->client.prompts)) == 0);
    } else {
        CHECK(fixture->calls == 3U);
        CHECK(Published(fixture, "tools", 1U) == 0);
        CHECK(Published(fixture, "resources", 1U) == 0);
        CHECK(Published(fixture, "prompts", 1U) == 0);
    }
    return 0;
}

static int NullOutputs(Fixture *fixture)
{
    CHECK(umi_ai_mcp_discover_tools(&fixture->client.session, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ai_mcp_discover_resources(&fixture->client.session, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ai_mcp_discover_prompts(&fixture->client.session, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ai_mcp_client_discover(NULL) == UMI_STATUS_INVALID_STATE);
    CHECK(fixture->calls == 0U);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 4) return 2;
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    if (fixture == NULL) return 3;
    int result = Setup(fixture);
    if (result == 0) {
        if (strcmp(argv[1], "single") == 0) result = Single(fixture, argv[2], argv[3]);
        else if (strcmp(argv[1], "client") == 0) result = Client(fixture, argv[2], argv[3]);
        else if (strcmp(argv[1], "nulls") == 0) result = NullOutputs(fixture);
        else result = 2;
    }
    free(fixture);
    return result;
}
