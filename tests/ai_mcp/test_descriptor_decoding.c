/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_descriptor_decoding.c
 *
 * PURPOSE:
 *   Verify structured descriptor decoding and retained discovery catalogues on malformed metadata.
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

typedef struct Field { const char *name; size_t offset, capacity; int kind; } Field;
typedef struct Adapter {
    const char *kind, *identity, *list;
    size_t size, catalogue_size, identity_capacity;
    Field fields[3]; size_t field_count;
    UmiStatus (*decode)(const char *, void *);
    UmiStatus (*allocation)(const char *, void *);
    void (*init)(void *);
    UmiStatus (*add)(void *, const void *);
    UmiStatus (*discover)(UmiAiMcpSession *, void *);
} Adapter;

UmiStatus UmiTestMcpUnusedDecodeTool(const char *json, UmiAiMcpToolDescriptor *out);
static UmiStatus ToolDecode(const char *json, void *out) { return umi_ai_mcp_decode_tool(json, out); }
static UmiStatus ToolAllocation(const char *json, void *out) { return UmiTestMcpUnusedDecodeTool(json, out); }
static void ToolInit(void *c) { umi_ai_mcp_tool_catalogue_init(c); }
static UmiStatus ToolAdd(void *c, const void *item) { return umi_ai_mcp_tool_catalogue_add(c, item); }
static UmiStatus ToolDiscover(UmiAiMcpSession *s, void *c) { return umi_ai_mcp_discover_tools(s, c); }
static const Adapter tool_adapter = {
    "tool", "name", "tools", sizeof(UmiAiMcpToolDescriptor), sizeof(UmiAiMcpToolCatalogue), sizeof(((UmiAiMcpToolDescriptor *)0)->name),
    {{"description", offsetof(UmiAiMcpToolDescriptor, description), sizeof(((UmiAiMcpToolDescriptor *)0)->description), 0}, {"inputSchema", offsetof(UmiAiMcpToolDescriptor, input_schema), sizeof(((UmiAiMcpToolDescriptor *)0)->input_schema), 1}, {NULL, 0U, 0U, 0}}, 2U,
    ToolDecode, ToolAllocation, ToolInit, ToolAdd, ToolDiscover
};

UmiStatus UmiTestMcpUnusedDecodeResource(const char *json, UmiAiMcpResourceDescriptor *out);
static UmiStatus ResourceDecode(const char *json, void *out) { return umi_ai_mcp_decode_resource(json, out); }
static UmiStatus ResourceAllocation(const char *json, void *out) { return UmiTestMcpUnusedDecodeResource(json, out); }
static void ResourceInit(void *c) { umi_ai_mcp_resource_catalogue_init(c); }
static UmiStatus ResourceAdd(void *c, const void *item) { return umi_ai_mcp_resource_catalogue_add(c, item); }
static UmiStatus ResourceDiscover(UmiAiMcpSession *s, void *c) { return umi_ai_mcp_discover_resources(s, c); }
static const Adapter resource_adapter = {
    "resource", "uri", "resources", sizeof(UmiAiMcpResourceDescriptor), sizeof(UmiAiMcpResourceCatalogue), sizeof(((UmiAiMcpResourceDescriptor *)0)->uri),
    {{"name", offsetof(UmiAiMcpResourceDescriptor, name), sizeof(((UmiAiMcpResourceDescriptor *)0)->name), 0}, {"description", offsetof(UmiAiMcpResourceDescriptor, description), sizeof(((UmiAiMcpResourceDescriptor *)0)->description), 0}, {"mimeType", offsetof(UmiAiMcpResourceDescriptor, mime_type), sizeof(((UmiAiMcpResourceDescriptor *)0)->mime_type), 0}}, 3U,
    ResourceDecode, ResourceAllocation, ResourceInit, ResourceAdd, ResourceDiscover
};

UmiStatus UmiTestMcpUnusedDecodePrompt(const char *json, UmiAiMcpPromptDescriptor *out);
static UmiStatus PromptDecode(const char *json, void *out) { return umi_ai_mcp_decode_prompt(json, out); }
static UmiStatus PromptAllocation(const char *json, void *out) { return UmiTestMcpUnusedDecodePrompt(json, out); }
static void PromptInit(void *c) { umi_ai_mcp_prompt_catalogue_init(c); }
static UmiStatus PromptAdd(void *c, const void *item) { return umi_ai_mcp_prompt_catalogue_add(c, item); }
static UmiStatus PromptDiscover(UmiAiMcpSession *s, void *c) { return umi_ai_mcp_discover_prompts(s, c); }
static const Adapter prompt_adapter = {
    "prompt", "name", "prompts", sizeof(UmiAiMcpPromptDescriptor), sizeof(UmiAiMcpPromptCatalogue), sizeof(((UmiAiMcpPromptDescriptor *)0)->name),
    {{"description", offsetof(UmiAiMcpPromptDescriptor, description), sizeof(((UmiAiMcpPromptDescriptor *)0)->description), 0}, {"arguments", offsetof(UmiAiMcpPromptDescriptor, arguments_schema), sizeof(((UmiAiMcpPromptDescriptor *)0)->arguments_schema), 2}, {NULL, 0U, 0U, 0}}, 2U,
    PromptDecode, PromptAllocation, PromptInit, PromptAdd, PromptDiscover
};

typedef struct Fixture {
    const Adapter *api;
    void *output, *previous;
    char json[UMI_AI_MCP_TEXT_CAPACITY + 1U];
    char payload[UMI_AI_MCP_TEXT_CAPACITY + 1U];
    char value[UMI_AI_MCP_SCHEMA_CAPACITY + 3U];
    UmiAiMcpSession session;
    unsigned requests;
} Fixture;
static int Written(int count, size_t capacity) { return count >= 0 && (size_t)count < capacity; }
static int Object(Fixture *f, const char *extra)
{
    return Written(snprintf(f->json, sizeof(f->json), "{\"%s\":\"entry\"%s%s}",
        f->api->identity, extra[0] == '\0' ? "" : ",", extra), sizeof(f->json));
}
static int PrepareField(Fixture *f, const Field *field, const char *mode, UmiStatus *expected)
{
    char extra[UMI_AI_MCP_SCHEMA_CAPACITY * 2U + 256U];
    *expected = UMI_STATUS_OK;
    const char *normal = field->kind == 0 ? "\"value\"" : field->kind == 1 ? "{}" : "[]";
    if (strcmp(mode, "wrong-kind") == 0) {
        CHECK(Written(snprintf(extra, sizeof(extra), "\"%s\":%s", field->name, field->kind == 0 ? "0" : "\"value\""), sizeof(extra)));
        *expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(mode, "duplicate") == 0) {
        CHECK(Written(snprintf(extra, sizeof(extra), "\"%s\":%s,\"%s\":%s", field->name, normal, field->name, normal), sizeof(extra)));
        *expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(mode, "nested") == 0) {
        CHECK(Written(snprintf(extra, sizeof(extra), "\"extra\":{\"%s\":%s}", field->name, normal), sizeof(extra)));
    } else if (strcmp(mode, "empty") == 0) {
        CHECK(Written(snprintf(extra, sizeof(extra), "\"%s\":%s", field->name, field->kind == 0 ? "\"\"" : normal), sizeof(extra)));
    } else if (strcmp(mode, "unicode") == 0) {
        const char *unicode = field->kind == 0 ? "\"caf\\u00e9\"" :
            field->kind == 1 ? "{ \"label\" : \"caf\\u00e9\" }" : "[ \"caf\\u00e9\" ]";
        CHECK(Written(snprintf(extra, sizeof(extra), "\"%s\":%s", field->name, unicode), sizeof(extra)));
        strcpy(f->value, field->kind == 0 ? "caf\xc3\xa9" : unicode);
    } else if (strcmp(mode, "boundary") == 0 || strcmp(mode, "overflow") == 0) {
        const int overflow = strcmp(mode, "overflow") == 0;
        size_t length = overflow ? field->capacity : field->capacity - 1U;
        CHECK(length + 3U <= sizeof(f->value));
        if (field->kind == 0) {
            memset(f->value, 'x', length); f->value[length] = '\0';
            CHECK(Written(snprintf(extra, sizeof(extra), "\"%s\":\"%s\"", field->name, f->value), sizeof(extra)));
        } else {
            f->value[0] = field->kind == 1 ? '{' : '[';
            memset(f->value + 1U, ' ', length - 2U);
            f->value[length - 1U] = field->kind == 1 ? '}' : ']'; f->value[length] = '\0';
            CHECK(Written(snprintf(extra, sizeof(extra), "\"%s\":%s", field->name, f->value), sizeof(extra)));
        }
        if (overflow) *expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else return 2;
    CHECK(Object(f, extra)); return 0;
}

static int PrepareBasic(Fixture *f, const char *name, UmiStatus *expected)
{
    *expected = UMI_STATUS_OK;
    CHECK(Object(f, ""));
    if (strcmp(name, "minimal") == 0 || strcmp(name, "alias") == 0 || strcmp(name, "allocation") == 0 || strcmp(name, "nulls") == 0) return 0;
    if (strcmp(name, "full") == 0) {
        size_t used = (size_t)snprintf(f->json, sizeof(f->json), "{\"%s\":\"entry\"", f->api->identity);
        for (size_t i = 0U; i < f->api->field_count; ++i) {
            const Field *field = &f->api->fields[i];
            int count = snprintf(f->json + used, sizeof(f->json) - used, ",\"%s\":%s", field->name,
                field->kind == 0 ? "\"value\"" : field->kind == 1 ? "{}" : "[]");
            CHECK(Written(count, sizeof(f->json) - used)); used += (size_t)count;
        }
        CHECK(used + 2U <= sizeof(f->json)); f->json[used++] = '}'; f->json[used] = '\0';
    } else if (strcmp(name, "unicode") == 0) {
        CHECK(Written(snprintf(f->json, sizeof(f->json), "{\"%s\":\"caf\\u00e9\"}", f->api->identity), sizeof(f->json)));
    } else if (strcmp(name, "empty-identity") == 0) {
        CHECK(Written(snprintf(f->json, sizeof(f->json), "{\"%s\":\"\"}", f->api->identity), sizeof(f->json)));
        *expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "missing-identity") == 0) {
        strcpy(f->json, "{}"); *expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "nested-identity") == 0) {
        CHECK(Written(snprintf(f->json, sizeof(f->json), "{\"extra\":{\"%s\":\"entry\"}}", f->api->identity), sizeof(f->json)));
        *expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "wrong-identity") == 0) {
        CHECK(Written(snprintf(f->json, sizeof(f->json), "{\"%s\":0}", f->api->identity), sizeof(f->json)));
        *expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "duplicate-identity") == 0) {
        const char *escaped = strcmp(f->api->identity, "name") == 0 ? "na\\u006de" : "u\\u0072i";
        CHECK(Written(snprintf(f->json, sizeof(f->json), "{\"%s\":\"entry\",\"%s\":\"other\"}", f->api->identity, escaped), sizeof(f->json)));
        *expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(name, "invalid-extension") == 0) {
        CHECK(Object(f, "\"extra\":\"\\uD800\"")); *expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "trailing") == 0) {
        strcat(f->json, " false"); *expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "root-array") == 0) {
        strcpy(f->json, "[]"); *expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "byte-last") == 0 || strcmp(name, "byte-overflow") == 0) {
        size_t used = strlen(f->json), end = strcmp(name, "byte-last") == 0 ? UMI_AI_MCP_TEXT_CAPACITY - 1U : UMI_AI_MCP_TEXT_CAPACITY;
        memset(f->json + used, ' ', end - used); f->json[end] = '\0';
        if (end == UMI_AI_MCP_TEXT_CAPACITY) *expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "identity-boundary") == 0 || strcmp(name, "identity-overflow") == 0) {
        size_t length = strcmp(name, "identity-boundary") == 0 ? f->api->identity_capacity - 1U : f->api->identity_capacity;
        CHECK(length < sizeof(f->value)); memset(f->value, 'x', length); f->value[length] = '\0';
        CHECK(Written(snprintf(f->json, sizeof(f->json), "{\"%s\":\"%s\"}", f->api->identity, f->value), sizeof(f->json)));
        if (length == f->api->identity_capacity) *expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else return 2;
    return 0;
}
static int Decode(Fixture *f, const char *name, const Field *field, UmiStatus expected)
{
    if (field == NULL && strcmp(name, "nulls") == 0) {
        CHECK(f->api->decode(NULL, f->output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f->api->decode(f->json, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(f->output, f->previous, f->api->size) == 0); return 0;
    }
    if (field == NULL && strcmp(name, "alias") == 0) {
        void *storage = malloc(UMI_AI_MCP_TEXT_CAPACITY); CHECK(storage != NULL); strcpy(storage, f->json);
        UmiStatus status = f->api->decode(storage, storage);
        int valid = status == UMI_STATUS_OK && strcmp(storage, "entry") == 0;
        free(storage); CHECK(valid); return 0;
    }
    UmiStatus status = field == NULL && strcmp(name, "allocation") == 0 ?
        f->api->allocation(f->json, f->output) : f->api->decode(f->json, f->output);
    if (field == NULL && strcmp(name, "allocation") == 0) expected = UMI_STATUS_OUT_OF_MEMORY;
    CHECK(status == expected);
    if (status != UMI_STATUS_OK) { CHECK(memcmp(f->output, f->previous, f->api->size) == 0); return 0; }
    const char *identity = field == NULL && strcmp(name, "unicode") == 0 ? "caf\xc3\xa9" :
        field == NULL && strcmp(name, "identity-boundary") == 0 ? f->value : "entry";
    CHECK(strcmp(f->output, identity) == 0);
    for (size_t i = 0U; i < f->api->field_count; ++i) {
        const Field *current = &f->api->fields[i];
        const char *value = (const char *)f->output + current->offset;
        if (current == field && (strcmp(name, "boundary") == 0 || strcmp(name, "unicode") == 0)) CHECK(strcmp(value, f->value) == 0);
        else if ((current == field && strcmp(name, "empty") == 0) || (field == NULL && strcmp(name, "full") == 0)) {
            const char *wanted = current->kind == 0 ? (field == NULL ? "value" : "") : current->kind == 1 ? "{}" : "[]";
            CHECK(strcmp(value, wanted) == 0);
        } else CHECK(value[0] == '\0');
    }
    if (strcmp(f->api->kind, "tool") == 0) {
        const UmiAiMcpToolDescriptor *tool = f->output;
        CHECK(tool->flags == 0U && tool->permission[0] == '\0');
    }
    return 0;
}

static UmiStatus DiscoveryRequest(void *data, const char *method, const char *params,
    char *out, size_t capacity)
{
    Fixture *f = data; (void)params; ++f->requests;
    char expected[32]; (void)snprintf(expected, sizeof(expected), "%s/list", f->api->list);
    if (strcmp(method, expected) != 0) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = strlen(f->payload);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, f->payload, length + 1U); return UMI_STATUS_OK;
}
static int Discover(Fixture *f, UmiStatus expected)
{
    CHECK(expected != UMI_STATUS_OK);
    char seed[64]; (void)snprintf(seed, sizeof(seed), "{\"%s\":\"previous\"}", f->api->identity);
    CHECK(f->api->decode(seed, f->output) == UMI_STATUS_OK);
    void *catalogue = calloc(1U, f->api->catalogue_size), *previous = malloc(f->api->catalogue_size);
    if (catalogue == NULL || previous == NULL) { free(catalogue); free(previous); return 3; }
    f->api->init(catalogue);
    UmiStatus status = f->api->add(catalogue, f->output);
    if (status != UMI_STATUS_OK) { free(catalogue); free(previous); return 4; }
    memcpy(previous, catalogue, f->api->catalogue_size);
    int count = snprintf(f->payload, sizeof(f->payload), "{\"%s\":[{\"%s\":\"first\"},%s]}",
        f->api->list, f->api->identity, f->json);
    if (!Written(count, sizeof(f->payload))) { free(catalogue); free(previous); return 5; }
    f->session.state = UMI_AI_MCP_SESSION_READY; f->session.revision = 1U;
    f->session.transport = (UmiAiMcpTransport){DiscoveryRequest, NULL, f};
    status = f->api->discover(&f->session, catalogue);
    int preserved = memcmp(previous, catalogue, f->api->catalogue_size) == 0;
    free(catalogue); free(previous);
    CHECK(status == expected && preserved && f->requests == 1U);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 5) return 2;
    const Adapter *api = strcmp(argv[1], "tool") == 0 ? &tool_adapter :
        strcmp(argv[1], "resource") == 0 ? &resource_adapter : strcmp(argv[1], "prompt") == 0 ? &prompt_adapter : NULL;
    if (api == NULL) return 2;
    const Field *field = NULL;
    if (strcmp(argv[4], "all") != 0) {
        for (size_t i = 0U; i < api->field_count; ++i)
            if (strcmp(argv[4], api->fields[i].name) == 0) field = &api->fields[i];
        if (field == NULL) return 2;
    }
    Fixture *f = calloc(1U, sizeof(*f));
    if (f == NULL) return 3;
    f->api = api; f->output = malloc(api->size); f->previous = malloc(api->size);
    if (f->output == NULL || f->previous == NULL) { free(f->output); free(f->previous); free(f); return 3; }
    memset(f->output, 0x5a, api->size); memcpy(f->previous, f->output, api->size);
    UmiStatus expected;
    int result = field == NULL ? PrepareBasic(f, argv[3], &expected) : PrepareField(f, field, argv[3], &expected);
    if (result == 0) result = strcmp(argv[2], "codec") == 0 ? Decode(f, argv[3], field, expected) :
        strcmp(argv[2], "discovery") == 0 ? Discover(f, expected) : 2;
    free(f->output); free(f->previous); free(f); return result;
}
