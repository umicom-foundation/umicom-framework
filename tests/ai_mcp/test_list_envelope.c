/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_list_envelope.c
 *
 * PURPOSE:
 *   Verify complete list admission, callback ownership and discovery preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "test_support.h"
#include "umicom/ai/mcp/json_array.h"
#include "../../src/ai/mcp/json_internal.h"
#include <stdlib.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

typedef struct ListCase {
    const char *name, *json;
    UmiStatus expected;
    size_t count;
} ListCase;
static const ListCase cases[] = {
    {"empty", "{\"%s\":[]}", UMI_STATUS_OK, 0U},
    {"objects", "{\"%s\":[{\"name\":\"one\",\"uri\":\"one\"},{\"name\":\"two\",\"uri\":\"two\"}]}", UMI_STATUS_OK, 2U},
    {"spaced", "{ \"%s\" : [ { \"n\" : 1 }, {\"n\":2} ] }", UMI_STATUS_OK, 2U},
    {"nested-lookalike", "{\"outer\":{\"%s\":[{}]}}", UMI_STATUS_NOT_FOUND, 0U},
    {"root-precedence", "{\"outer\":{\"%s\":[1]},\"%s\":[{}]}", UMI_STATUS_OK, 1U},
    {"missing", "{}", UMI_STATUS_NOT_FOUND, 0U},
    {"null-list", "{\"%s\":null}", UMI_STATUS_PARSE_ERROR, 0U},
    {"object-list", "{\"%s\":{}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"string-list", "{\"%s\":\"[]\"}", UMI_STATUS_PARSE_ERROR, 0U},
    {"root-array", "[{\"%s\":[]}]", UMI_STATUS_PARSE_ERROR, 0U},
    {"leading-comma", "{\"%s\":[,{}]}", UMI_STATUS_PARSE_ERROR, 0U},
    {"trailing-comma", "{\"%s\":[{},]}", UMI_STATUS_PARSE_ERROR, 0U},
    {"double-comma", "{\"%s\":[{},,{}]}", UMI_STATUS_PARSE_ERROR, 0U},
    {"missing-comma", "{\"%s\":[{}{}]}", UMI_STATUS_PARSE_ERROR, 0U},
    {"nonobject-first", "{\"%s\":[1,{}]}", UMI_STATUS_PARSE_ERROR, 0U},
    {"nonobject-late", "{\"%s\":[{},false]}", UMI_STATUS_PARSE_ERROR, 0U},
    {"duplicate-list", "{\"%s\":[],\"%s\":[{}]}", UMI_STATUS_ALREADY_EXISTS, 0U},
    {"unclosed-array", "{\"%s\":[{}", UMI_STATUS_PARSE_ERROR, 0U},
    {"unclosed-root", "{\"%s\":[{}]", UMI_STATUS_PARSE_ERROR, 0U},
    {"trailing-root", "{\"%s\":[{}]} {}", UMI_STATUS_PARSE_ERROR, 0U},
    {"trailing-value", "{\"%s\":[{}]} false", UMI_STATUS_PARSE_ERROR, 0U},
    {"unknown-escape", "{\"%s\":[{\"x\":\"\\q\"}]}", UMI_STATUS_PARSE_ERROR, 0U},
    {"surrogate", "{\"%s\":[{}],\"extension\":\"\\uD800\"}", UMI_STATUS_PARSE_ERROR, 0U},
    {"nul-text", "{\"%s\":[{}],\"extension\":\"\\u0000\"}", UMI_STATUS_PARSE_ERROR, 0U},
    {"invalid-number", "{\"%s\":[{}],\"extension\":01}", UMI_STATUS_PARSE_ERROR, 0U},
    {"invalid-whitespace", "{\"%s\":[{}],\v\"extension\":0}", UMI_STATUS_PARSE_ERROR, 0U},
    {"unicode", "{\"%s\":[{\"text\":\"\\u20ac\\uD83D\\uDE00\"}]}", UMI_STATUS_OK, 1U},
    {"unknown-duplicate", "{\"%s\":[{}],\"extension\":0,\"extension\":1}", UMI_STATUS_OK, 1U},
    {"nested-shapes", "{\"%s\":[{\"nested\":[{},[1,2]],\"text\":\"[{}]\"}]}", UMI_STATUS_OK, 1U},
};

typedef struct VisitorState {
    size_t calls, fail_at;
    int mutate, nested, invalid;
    char *input;
    char seen[2][256];
} VisitorState;
UmiStatus UmiTestMcpListVisitFault(int mode, const char *json, const char *key,
    UmiAiMcpJsonObjectVisitor visitor, void *data, size_t *count, unsigned *allocations);

static UmiStatus Visit(const char *object, void *data)
{
    VisitorState *s = data;
    size_t index = s->calls++;
    if (index < 2U) {
        size_t length = strlen(object);
        if (length >= sizeof(s->seen[index])) { s->invalid = 1; return UMI_STATUS_CAPACITY_EXCEEDED; }
        memcpy(s->seen[index], object, length + 1U);
    }
    if (s->mutate && index == 0U) strcpy(s->input, "{}");
    if (s->nested && index == 0U) {
        VisitorState child = {0}; size_t count = 99U;
        UmiStatus status = umi_ai_mcp_json_array_visit_objects("{\"tools\":[{}]}", "tools", Visit, &child, &count);
        if (status != UMI_STATUS_OK || count != 1U || child.calls != 1U) {
            s->invalid = 1; return UMI_STATUS_INVALID_STATE;
        }
    }
    return s->fail_at == s->calls ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static const ListCase *FindCase(const char *name)
{
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(cases[i].name, name) == 0) return &cases[i];
    return NULL;
}
static int Written(int length, size_t capacity) { return length >= 0 && (size_t)length < capacity; }
static int Direct(const char *name, char *json)
{
    const ListCase *item = FindCase(name);
    VisitorState state = {0}; state.input = json;
    size_t count = 99U, expected_count = 0U;
    UmiStatus expected = UMI_STATUS_OK;
    const char *key = "tools";
    char long_key[4098];
    int fault = 0; unsigned allocations = 0U;
    if (item != NULL) {
        CHECK(Written(snprintf(json, UMI_AI_MCP_TEXT_CAPACITY + 1U, item->json, key, key), UMI_AI_MCP_TEXT_CAPACITY + 1U));
        expected = item->expected; expected_count = item->count;
    } else if (strcmp(name, "escaped-key") == 0 || strcmp(name, "escaped-duplicate") == 0) {
        if (strcmp(name, "escaped-key") == 0) { strcpy(json, "{\"\\u0074ools\":[{}]}"); expected_count = 1U; }
        else { strcpy(json, "{\"tools\":[],\"\\u0074ools\":[{}]}"); expected = UMI_STATUS_ALREADY_EXISTS; }
    } else if (strcmp(name, "key-last") == 0 || strcmp(name, "key-overflow") == 0) {
        size_t length = strcmp(name, "key-last") == 0 ? 4096U : 4097U;
        memset(long_key, 'k', length); long_key[length] = '\0'; key = long_key;
        CHECK(Written(snprintf(json, UMI_AI_MCP_TEXT_CAPACITY + 1U, "{\"%s\":[]}", key), UMI_AI_MCP_TEXT_CAPACITY + 1U));
        if (length == 4097U) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "byte-last") == 0 || strcmp(name, "byte-overflow") == 0) {
        strcpy(json, "{\"tools\":[{}]}"); size_t start = strlen(json);
        size_t end = strcmp(name, "byte-last") == 0 ? UMI_AI_MCP_TEXT_CAPACITY - 1U : UMI_AI_MCP_TEXT_CAPACITY;
        memset(json + start, ' ', end - start); json[end] = '\0';
        if (end == UMI_AI_MCP_TEXT_CAPACITY) expected = UMI_STATUS_CAPACITY_EXCEEDED;
        else expected_count = 1U;
    } else if (strcmp(name, "depth-last") == 0 || strcmp(name, "depth-overflow") == 0) {
        strcpy(json, "{\"tools\":[{}],\"extra\":"); size_t at = strlen(json);
        size_t depth = strcmp(name, "depth-last") == 0 ? 127U : 128U;
        for (size_t i = 0U; i < depth; ++i) json[at++] = '[';
        json[at++] = '0';
        for (size_t i = 0U; i < depth; ++i) json[at++] = ']';
        json[at++] = '}'; json[at] = '\0';
        if (depth == 128U) expected = UMI_STATUS_CAPACITY_EXCEEDED; else expected_count = 1U;
    } else if (strcmp(name, "invalid-utf8") == 0) {
        strcpy(json, "{\"tools\":[{}],\"extra\":\"XY\"}");
        char *bad = strstr(json, "XY"); CHECK(bad != NULL);
        bad[0] = (char)0xc0; bad[1] = (char)0xaf; expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "callback-first") == 0 || strcmp(name, "callback-second") == 0 ||
               strcmp(name, "nested-visit") == 0 || strcmp(name, "mutate-input") == 0) {
        strcpy(json, "{\"tools\":[{\"name\":\"one\"},{\"name\":\"two\"}]}"); expected_count = 2U;
        if (strcmp(name, "callback-first") == 0) state.fail_at = 1U;
        if (strcmp(name, "callback-second") == 0) state.fail_at = 2U;
        if (state.fail_at != 0U) { expected = UMI_STATUS_IO_ERROR; expected_count = 0U; }
        state.nested = strcmp(name, "nested-visit") == 0;
        state.mutate = strcmp(name, "mutate-input") == 0;
    } else if (strncmp(name, "fault-", 6U) == 0) {
        strcpy(json, "{\"tools\":[{}]}"); expected = UMI_STATUS_OUT_OF_MEMORY;
        if (strcmp(name, "fault-parse") == 0) fault = 1;
        else if (strcmp(name, "fault-buffer") == 0) fault = 2;
        else if (strcmp(name, "fault-empty") == 0) { fault = 2; strcpy(json, "{\"tools\":[]}"); expected = UMI_STATUS_OK; }
        else if (strcmp(name, "fault-preflight") == 0) { fault = 2; strcpy(json, "{\"tools\":[{},false]}"); expected = UMI_STATUS_PARSE_ERROR; }
        else return 2;
    } else if (strcmp(name, "nulls") == 0) {
        CHECK(umi_ai_mcp_json_array_visit_objects(NULL, "tools", Visit, &state, &count) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_json_array_visit_objects("{}", NULL, Visit, &state, &count) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_json_array_visit_objects("{}", "", Visit, &state, &count) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_json_array_visit_objects("{}", "tools", NULL, &state, &count) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_json_array_visit_objects("{}", "tools", Visit, &state, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(count == 99U && state.calls == 0U); return 0;
    } else if (strcmp(name, "parser-output") == 0) {
        UmiJsonTree *tree = NULL; CHECK(umi_ai_mcp_json_object_parse("{}", &tree) == UMI_STATUS_OK);
        UmiJsonTree *saved = tree;
        int valid = umi_ai_mcp_json_object_parse(NULL, &tree) == UMI_STATUS_INVALID_ARGUMENT && tree == saved &&
            umi_ai_mcp_json_object_parse("{}", NULL) == UMI_STATUS_INVALID_ARGUMENT &&
            umi_ai_mcp_json_object_parse("[]", &tree) == UMI_STATUS_PARSE_ERROR && tree == saved &&
            umi_ai_mcp_json_object_parse("{", &tree) == UMI_STATUS_PARSE_ERROR && tree == saved;
        UmiJsonTreeDestroy(saved); CHECK(valid); return 0;
    } else return 2;
    UmiStatus status = fault == 0 ? umi_ai_mcp_json_array_visit_objects(json, key, Visit, &state, &count) :
        UmiTestMcpListVisitFault(fault, json, key, Visit, &state, &count, &allocations);
    CHECK(status == expected && count == expected_count && !state.invalid);
    CHECK(state.calls == (state.fail_at != 0U ? state.fail_at : expected_count));
    if (fault != 0) CHECK(allocations == (strcmp(name, "fault-buffer") == 0 ? 1U : 0U));
    if (strcmp(name, "spaced") == 0) CHECK(strcmp(state.seen[0], "{ \"n\" : 1 }") == 0 && strcmp(state.seen[1], "{\"n\":2}") == 0);
    if (state.mutate || state.nested) CHECK(strcmp(state.seen[1], "{\"name\":\"two\"}") == 0);
    return 0;
}

typedef struct Adapter {
    const char *list;
    size_t catalogue_size, descriptor_size;
    void (*init)(void *);
    UmiStatus (*decode)(const char *, void *);
    UmiStatus (*add)(void *, const void *);
    UmiStatus (*discover)(UmiAiMcpSession *, void *);
    size_t (*count)(const void *);
    const void *(*find)(const void *, const char *);
} Adapter;

static void ToolInit(void *c) { umi_ai_mcp_tool_catalogue_init(c); }
static UmiStatus ToolDecode(const char *json, void *d) { return umi_ai_mcp_decode_tool(json, d); }
static UmiStatus ToolAdd(void *c, const void *d) { return umi_ai_mcp_tool_catalogue_add(c, d); }
static UmiStatus ToolDiscover(UmiAiMcpSession *s, void *c) { return umi_ai_mcp_discover_tools(s, c); }
static size_t ToolCount(const void *c) { return ((const UmiAiMcpToolCatalogue *)c)->count; }
static const void *ToolFind(const void *c, const char *id) { return umi_ai_mcp_tool_catalogue_find(c, id); }
static const Adapter tool_adapter = {"tools", sizeof(UmiAiMcpToolCatalogue), sizeof(UmiAiMcpToolDescriptor),
    ToolInit, ToolDecode, ToolAdd, ToolDiscover, ToolCount, ToolFind};


static void ResourceInit(void *c) { umi_ai_mcp_resource_catalogue_init(c); }
static UmiStatus ResourceDecode(const char *json, void *d) { return umi_ai_mcp_decode_resource(json, d); }
static UmiStatus ResourceAdd(void *c, const void *d) { return umi_ai_mcp_resource_catalogue_add(c, d); }
static UmiStatus ResourceDiscover(UmiAiMcpSession *s, void *c) { return umi_ai_mcp_discover_resources(s, c); }
static size_t ResourceCount(const void *c) { return ((const UmiAiMcpResourceCatalogue *)c)->count; }
static const void *ResourceFind(const void *c, const char *id) { return umi_ai_mcp_resource_catalogue_find(c, id); }
static const Adapter resource_adapter = {"resources", sizeof(UmiAiMcpResourceCatalogue), sizeof(UmiAiMcpResourceDescriptor),
    ResourceInit, ResourceDecode, ResourceAdd, ResourceDiscover, ResourceCount, ResourceFind};


static void PromptInit(void *c) { umi_ai_mcp_prompt_catalogue_init(c); }
static UmiStatus PromptDecode(const char *json, void *d) { return umi_ai_mcp_decode_prompt(json, d); }
static UmiStatus PromptAdd(void *c, const void *d) { return umi_ai_mcp_prompt_catalogue_add(c, d); }
static UmiStatus PromptDiscover(UmiAiMcpSession *s, void *c) { return umi_ai_mcp_discover_prompts(s, c); }
static size_t PromptCount(const void *c) { return ((const UmiAiMcpPromptCatalogue *)c)->count; }
static const void *PromptFind(const void *c, const char *id) { return umi_ai_mcp_prompt_catalogue_find(c, id); }
static const Adapter prompt_adapter = {"prompts", sizeof(UmiAiMcpPromptCatalogue), sizeof(UmiAiMcpPromptDescriptor),
    PromptInit, PromptDecode, PromptAdd, PromptDiscover, PromptCount, PromptFind};

typedef struct DiscoveryState {
    const Adapter *api;
    const char *json;
    unsigned requests;
} DiscoveryState;
static UmiStatus Request(void *data, const char *method, const char *params, char *out, size_t capacity)
{
    DiscoveryState *s = data; (void)params; ++s->requests;
    char expected[32]; (void)snprintf(expected, sizeof(expected), "%s/list", s->api->list);
    if (strcmp(method, expected) != 0) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = strlen(s->json);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, s->json, length + 1U); return UMI_STATUS_OK;
}
static int Discover(const char *kind, const char *name, char *json)
{
    const Adapter *api = strcmp(kind, "tool") == 0 ? &tool_adapter : strcmp(kind, "resource") == 0 ?
        &resource_adapter : strcmp(kind, "prompt") == 0 ? &prompt_adapter : NULL;
    const ListCase *item = FindCase(name);
    CHECK(api != NULL && item != NULL);
    CHECK(Written(snprintf(json, UMI_AI_MCP_TEXT_CAPACITY + 1U, item->json, api->list, api->list), UMI_AI_MCP_TEXT_CAPACITY + 1U));
    void *catalogue = calloc(1U, api->catalogue_size), *previous = malloc(api->catalogue_size);
    void *descriptor = calloc(1U, api->descriptor_size);
    if (catalogue == NULL || previous == NULL || descriptor == NULL) {
        free(catalogue); free(previous); free(descriptor); return 3;
    }
    api->init(catalogue);
    UmiStatus status = api->decode("{\"name\":\"previous\",\"uri\":\"previous\"}", descriptor);
    if (status == UMI_STATUS_OK) status = api->add(catalogue, descriptor);
    if (status != UMI_STATUS_OK) { free(catalogue); free(previous); free(descriptor); return 4; }
    memcpy(previous, catalogue, api->catalogue_size);
    DiscoveryState state = {api, json, 0U};
    UmiAiMcpSession session = {0}; session.state = UMI_AI_MCP_SESSION_READY; session.revision = 7U;
    session.transport = (UmiAiMcpTransport){Request, NULL, &state};
    status = api->discover(&session, catalogue);
    int valid = status == item->expected && state.requests == 1U;
    if (item->expected != UMI_STATUS_OK) valid = valid && memcmp(previous, catalogue, api->catalogue_size) == 0;
    else {
        valid = valid && api->count(catalogue) == item->count && api->find(catalogue, "previous") == NULL;
        if (item->count == 2U) valid = valid && api->find(catalogue, "one") != NULL && api->find(catalogue, "two") != NULL;
    }
    free(catalogue); free(previous); free(descriptor); CHECK(valid); return 0;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    char *json = malloc(UMI_AI_MCP_TEXT_CAPACITY + 1U);
    if (json == NULL) return 3;
    int result = strcmp(argv[1], "visitor") == 0 ? Direct(argv[2], json) : Discover(argv[1], argv[2], json);
    free(json); return result;
}
