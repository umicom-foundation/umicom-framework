/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_bridge_publication.c
 *
 * PURPOSE:
 *   Verify complete MCP registration and preserve borrowed owners on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_support.h"
#include <stdint.h>
#include <stdlib.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

UmiStatus UmiTestMcpRegistrationAllocationFailure(UmiAiMcpToolBridge *bridge,
    UmiAiMcpSession *session, const UmiAiMcpToolCatalogue *catalogue,
    UmiAiToolRegistry *registry);

typedef struct Fixture {
    UmiAiRuntime runtime;
    UmiAiToolRegistry saved_registry;
    UmiAiMcpSession session;
    UmiTestMcpTransportState transport;
    UmiAiMcpToolCatalogue catalogue;
    UmiAiMcpToolCatalogue saved_catalogue;
    UmiAiMcpToolBridge bridge;
    UmiAiMcpToolBridge saved_bridge;
    UmiAiMcpToolDescriptor descriptor;
} Fixture;

static UmiStatus UnrelatedTool(const char *arguments, char *output, size_t capacity, void *data)
{
    (void)arguments; (void)data;
    if (capacity < 1U) return UMI_STATUS_CAPACITY_EXCEEDED;
    output[0] = '\0';
    return UMI_STATUS_OK;
}

static int Setup(Fixture *fixture)
{
    umi_ai_runtime_init(&fixture->runtime);
    fixture->runtime.policy.allow_tools = 1;
    CHECK(umi_test_mcp_ready_session(&fixture->session, &fixture->transport) == UMI_STATUS_OK);
    CHECK(umi_ai_mcp_discover_tools(&fixture->session, &fixture->catalogue) == UMI_STATUS_OK);
    UmiAiTool tool;
    CHECK(umi_ai_tool_init(&tool, "existing", "Keep this tool", "test.existing", UnrelatedTool, fixture) == UMI_STATUS_OK);
    CHECK(umi_ai_tool_registry_add(&fixture->runtime.tools, &tool) == UMI_STATUS_OK);
    /* Fresh registration must not depend on the old destination's byte values. */
    memset(&fixture->bridge, 0x6d, sizeof(fixture->bridge));
    return 0;
}

static void Save(Fixture *fixture)
{
    memcpy(&fixture->saved_registry, &fixture->runtime.tools, sizeof(fixture->saved_registry));
    memcpy(&fixture->saved_bridge, &fixture->bridge, sizeof(fixture->saved_bridge));
    memcpy(&fixture->saved_catalogue, &fixture->catalogue, sizeof(fixture->saved_catalogue));
}

static int Preserved(const Fixture *fixture)
{
    CHECK(memcmp(&fixture->saved_registry, &fixture->runtime.tools, sizeof(fixture->saved_registry)) == 0);
    CHECK(memcmp(&fixture->saved_bridge, &fixture->bridge, sizeof(fixture->saved_bridge)) == 0);
    CHECK(memcmp(&fixture->saved_catalogue, &fixture->catalogue, sizeof(fixture->saved_catalogue)) == 0);
    return 0;
}

static char *ToolField(UmiAiMcpToolDescriptor *item, const char *field, size_t *capacity)
{
    if (strcmp(field, "name") == 0) { *capacity = sizeof(item->name); return item->name; }
    if (strcmp(field, "description") == 0) { *capacity = sizeof(item->description); return item->description; }
    if (strcmp(field, "schema") == 0) { *capacity = sizeof(item->input_schema); return item->input_schema; }
    if (strcmp(field, "permission") == 0) { *capacity = sizeof(item->permission); return item->permission; }
    return NULL;
}

static int Refused(Fixture *fixture, const char *kind, const char *name)
{
    UmiStatus expected = UMI_STATUS_INVALID_STATE;
    if (strcmp(kind, "catalogue") == 0) {
        if (strcmp(name, "count") == 0) fixture->catalogue.count = SIZE_MAX;
        else if (strcmp(name, "duplicate") == 0) fixture->catalogue.items[1] = fixture->catalogue.items[0];
        else if (strcmp(name, "empty-name") == 0) fixture->catalogue.items[1].name[0] = '\0';
        else {
            size_t capacity = 0U;
            char *field = ToolField(&fixture->catalogue.items[1], name, &capacity);
            CHECK(field != NULL); memset(field, 'x', capacity);
        }
    } else if (strcmp(kind, "server") == 0) {
        UmiAiMcpServerDescriptor *server = &fixture->session.server;
        if (strcmp(name, "id") == 0) memset(server->server_id, 'x', sizeof(server->server_id));
        else if (strcmp(name, "display") == 0) memset(server->display_name, 'x', sizeof(server->display_name));
        else if (strcmp(name, "endpoint") == 0) memset(server->endpoint, 'x', sizeof(server->endpoint));
        else if (strcmp(name, "prefix") == 0) memset(server->permission_prefix, 'x', sizeof(server->permission_prefix));
        else return 1;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "registry-count") == 0) fixture->runtime.tools.count = SIZE_MAX;
    else if (strcmp(name, "registry-text") == 0) memset(fixture->runtime.tools.tools[0].description, 'x', sizeof(fixture->runtime.tools.tools[0].description));
    else if (strcmp(name, "id-overflow") == 0) {
        memset(fixture->catalogue.items[1].name, 'x', sizeof(fixture->catalogue.items[1].name) - 1U);
        fixture->catalogue.items[1].name[sizeof(fixture->catalogue.items[1].name) - 1U] = '\0';
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "permission-overflow") == 0) {
        /* First entry has an explicit permission; only the later fallback fails. */
        strcpy(fixture->catalogue.items[0].permission, "explicit.first");
        memset(fixture->session.server.permission_prefix, 'p', sizeof(fixture->session.server.permission_prefix) - 1U);
        fixture->session.server.permission_prefix[sizeof(fixture->session.server.permission_prefix) - 1U] = '\0';
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "late-collision") == 0) {
        UmiAiTool tool;
        CHECK(umi_ai_tool_init(&tool, "mcp.test.write_file", "existing", "test", UnrelatedTool, fixture) == UMI_STATUS_OK);
        CHECK(umi_ai_tool_registry_add(&fixture->runtime.tools, &tool) == UMI_STATUS_OK);
        expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(name, "allocation") == 0) expected = UMI_STATUS_OUT_OF_MEMORY;
    else return 1;
    Save(fixture);
    const size_t requests = fixture->transport.request_count;
    UmiStatus status = strcmp(name, "allocation") == 0
        ? UmiTestMcpRegistrationAllocationFailure(&fixture->bridge, &fixture->session, &fixture->catalogue, &fixture->runtime.tools)
        : umi_ai_mcp_tool_bridge_register(&fixture->bridge, &fixture->session, &fixture->catalogue, &fixture->runtime.tools);
    CHECK(status == expected && fixture->transport.request_count == requests);
    CHECK(Preserved(fixture) == 0);
    if (strcmp(kind, "catalogue") == 0) {
        CHECK(UmiAiMcpToolCatalogueValidate(&fixture->catalogue) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_ai_mcp_tool_catalogue_find(&fixture->catalogue, "read_file") == NULL);
        CHECK(umi_ai_mcp_tool_catalogue_at(&fixture->catalogue, 0U) == NULL);
        memset(&fixture->descriptor, 0, sizeof(fixture->descriptor)); strcpy(fixture->descriptor.name, "new");
        CHECK(umi_ai_mcp_tool_catalogue_add(&fixture->catalogue, &fixture->descriptor) == UMI_STATUS_INVALID_STATE);
        CHECK(Preserved(fixture) == 0);
    }
    return 0;
}

static int Descriptor(Fixture *fixture, const char *mode, const char *name)
{
    fixture->descriptor = fixture->catalogue.items[0];
    size_t capacity = 0U;
    char *field = ToolField(&fixture->descriptor, name, &capacity);
    CHECK(field != NULL); memset(field, 'x', capacity);
    const int boundary = strcmp(mode, "boundary") == 0;
    if (boundary) field[capacity - 1U] = '\0';
    Save(fixture);
    CHECK(UmiAiMcpToolDescriptorValidate(&fixture->descriptor) == (boundary ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT));
    if (!boundary) {
        CHECK(umi_ai_mcp_tool_catalogue_add(&fixture->catalogue, &fixture->descriptor) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Preserved(fixture) == 0);
    }
    return 0;
}

static int Published(Fixture *fixture, const char *name)
{
    if (strcmp(name, "empty") == 0) fixture->catalogue.count = 0U;
    if (strcmp(name, "explicit") == 0) strcpy(fixture->catalogue.items[1].permission, "custom.write");
    if (strcmp(name, "empty-prefix") == 0) fixture->session.server.permission_prefix[0] = '\0';
    if (strcmp(name, "flags") == 0) fixture->catalogue.items[0].flags = UINT32_MAX;
    if (strcmp(name, "full-catalogue") == 0) {
        umi_ai_tool_registry_init(&fixture->runtime.tools);
        for (size_t index = fixture->catalogue.count; index < UMI_AI_MCP_MAX_TOOLS; ++index) {
            UmiAiMcpToolDescriptor item = {0};
            int written = snprintf(item.name, sizeof(item.name), "remote-%zu", index);
            CHECK(written > 0 && (size_t)written < sizeof(item.name));
            CHECK(umi_ai_mcp_tool_catalogue_add(&fixture->catalogue, &item) == UMI_STATUS_OK);
        }
    }
    const size_t original_count = fixture->runtime.tools.count;
    const size_t requests = fixture->transport.request_count;
    CHECK(umi_ai_mcp_tool_bridge_register(&fixture->bridge, &fixture->session, &fixture->catalogue, &fixture->runtime.tools) == UMI_STATUS_OK);
    CHECK(fixture->transport.request_count == requests);
    CHECK(fixture->runtime.tools.count == fixture->catalogue.count + original_count);
    CHECK(fixture->bridge.binding_count == fixture->catalogue.count);
    if (original_count != 0U) CHECK(strcmp(fixture->runtime.tools.tools[0].tool_id, "existing") == 0);
    if (fixture->catalogue.count == 0U) return 0;
    for (size_t index = 0U; index < fixture->catalogue.count; ++index) {
        CHECK(fixture->runtime.tools.tools[index + original_count].user_data == &fixture->bridge.bindings[index]);
        CHECK(fixture->bridge.bindings[index].session == &fixture->session);
        CHECK(strcmp(fixture->bridge.bindings[index].remote_name, fixture->catalogue.items[index].name) == 0);
    }
    CHECK(fixture->runtime.tools.tools[original_count].flags == fixture->catalogue.items[0].flags);
    CHECK(strcmp(fixture->runtime.tools.tools[original_count + 1U].permission,
        strcmp(name, "explicit") == 0 ? "custom.write" : strcmp(name, "empty-prefix") == 0 ? "write_file" : "mcp.test.write_file") == 0);
    if (strcmp(name, "rebind") == 0 || strcmp(name, "rebind-empty") == 0) {
        if (strcmp(name, "rebind-empty") == 0) fixture->catalogue.count = 0U;
        Save(fixture);
        CHECK(umi_ai_mcp_tool_bridge_register(&fixture->bridge, &fixture->session, &fixture->catalogue, &fixture->runtime.tools) == UMI_STATUS_BUSY);
        CHECK(Preserved(fixture) == 0);
    }
    char output[512] = "previous";
    CHECK(umi_ai_runtime_invoke_tool(&fixture->runtime, "mcp.test.read_file", 0, "{}", output, sizeof(output)) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(fixture->transport.request_count == requests && strcmp(output, "previous") == 0);
    CHECK(umi_ai_runtime_invoke_tool(&fixture->runtime, "mcp.test.read_file", 1, "{}", output, sizeof(output)) == UMI_STATUS_OK);
    CHECK(fixture->transport.request_count == requests + 1U && strstr(output, "tool-ok") != NULL);
    CHECK(strstr(fixture->transport.last_params, "read_file") != NULL);
    return 0;
}

static int Capacity(Fixture *fixture, const char *name)
{
    UmiAiToolRegistry *registry = &fixture->runtime.tools;
    const size_t limit = UMI_AI_MAX_TOOLS - (strcmp(name, "exact") == 0 ? 2U : 1U);
    while (registry->count < limit) {
        char id[UMI_AI_ID_CAPACITY]; UmiAiTool tool;
        int written = snprintf(id, sizeof(id), "unrelated-%zu", registry->count);
        CHECK(written > 0 && (size_t)written < sizeof(id));
        CHECK(umi_ai_tool_init(&tool, id, "", "", UnrelatedTool, fixture) == UMI_STATUS_OK);
        CHECK(umi_ai_tool_registry_add(registry, &tool) == UMI_STATUS_OK);
    }
    Save(fixture);
    UmiStatus status = umi_ai_mcp_tool_bridge_register(&fixture->bridge, &fixture->session, &fixture->catalogue, registry);
    if (strcmp(name, "exact") == 0) {
        CHECK(status == UMI_STATUS_OK && registry->count == UMI_AI_MAX_TOOLS);
        CHECK(fixture->bridge.binding_count == 2U);
    } else { CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED); CHECK(Preserved(fixture) == 0); }
    return 0;
}

static int Miscellaneous(Fixture *fixture, const char *name)
{
    if (strcmp(name, "revision") == 0) {
        fixture->catalogue.revision = UINT64_MAX;
        Save(fixture);
        CHECK(umi_ai_mcp_tool_catalogue_add(&fixture->catalogue, &fixture->catalogue.items[0]) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(Preserved(fixture) == 0);
    } else if (strcmp(name, "inactive") == 0) {
        memset(&fixture->catalogue.items[2], 0x7f, sizeof(fixture->catalogue.items[2]));
        CHECK(UmiAiMcpToolCatalogueValidate(&fixture->catalogue) == UMI_STATUS_OK);
        CHECK(umi_ai_mcp_tool_catalogue_at(&fixture->catalogue, 2U) == NULL);
        CHECK(umi_ai_mcp_tool_catalogue_at(&fixture->catalogue, SIZE_MAX) == NULL);
        CHECK(umi_ai_mcp_tool_catalogue_find(&fixture->catalogue, "read_file") == &fixture->catalogue.items[0]);
    } else if (strcmp(name, "borrowed-slot") == 0) {
        fixture->runtime.tools.tools[0].user_data = &fixture->bridge.bindings[UMI_AI_MCP_MAX_TOOL_BINDINGS - 1U];
        Save(fixture);
        CHECK(umi_ai_mcp_tool_bridge_register(&fixture->bridge, &fixture->session, &fixture->catalogue, &fixture->runtime.tools) == UMI_STATUS_BUSY);
        CHECK(Preserved(fixture) == 0);
    } else if (strcmp(name, "nulls") == 0) {
        Save(fixture);
        CHECK(UmiAiMcpToolDescriptorValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiMcpToolCatalogueValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_tool_bridge_register(NULL, &fixture->session, &fixture->catalogue, &fixture->runtime.tools) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_tool_bridge_register(&fixture->bridge, NULL, &fixture->catalogue, &fixture->runtime.tools) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_tool_bridge_register(&fixture->bridge, &fixture->session, NULL, &fixture->runtime.tools) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_tool_bridge_register(&fixture->bridge, &fixture->session, &fixture->catalogue, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Preserved(fixture) == 0);
    } else return 1;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    if (fixture == NULL) return 3;
    int result = Setup(fixture);
    if (result == 0) {
        if (strcmp(argv[1], "catalogue") == 0 || strcmp(argv[1], "server") == 0 || strcmp(argv[1], "refusal") == 0)
            result = Refused(fixture, argv[1], argv[2]);
        else if (strcmp(argv[1], "descriptor") == 0 || strcmp(argv[1], "boundary") == 0)
            result = Descriptor(fixture, argv[1], argv[2]);
        else if (strcmp(argv[1], "published") == 0) result = Published(fixture, argv[2]);
        else if (strcmp(argv[1], "capacity") == 0) result = Capacity(fixture, argv[2]);
        else if (strcmp(argv[1], "misc") == 0) result = Miscellaneous(fixture, argv[2]);
        else result = 2;
    }
    /* Fake transport and callbacks borrow only this single fixture allocation. */
    free(fixture);
    return result;
}
