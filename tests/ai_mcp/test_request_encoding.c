/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_request_encoding.c
 *
 * PURPOSE:
 *   Verify complete MCP parameter encoding, Unicode admission and unchanged failure outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "test_support.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdlib.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)
#define OUTPUT_CAPACITY (UMI_AI_MCP_TEXT_CAPACITY * 6U + 16U)

typedef UmiStatus (*Encode)(const char *, const char *, char *, size_t);
UmiStatus UmiTestMcpEscapeAllocationFailure(const char *, char *, size_t);
UmiStatus UmiTestMcpEncodeInitializeAllocationFailure(const char *, const char *, char *, size_t);
UmiStatus UmiTestMcpEncodeToolAllocationFailure(const char *, const char *, char *, size_t);
UmiStatus UmiTestMcpEncodeResourceAllocationFailure(const char *, char *, size_t);
UmiStatus UmiTestMcpEncodePromptAllocationFailure(const char *, const char *, char *, size_t);
UmiStatus UmiTestMcpUnusedEncodeTool(const char *, const char *, char *, size_t);
UmiStatus UmiTestMcpUnusedEncodePrompt(const char *, const char *, char *, size_t);
static UmiStatus Escape(const char *a, const char *b, char *out, size_t n)
{ (void)b; return umi_ai_mcp_json_escape_string(a, out, n); }
static UmiStatus EscapeFault(const char *a, const char *b, char *out, size_t n)
{ (void)b; return UmiTestMcpEscapeAllocationFailure(a, out, n); }
static UmiStatus Resource(const char *a, const char *b, char *out, size_t n)
{ (void)b; return umi_ai_mcp_encode_resource_read_params(a, out, n); }
static UmiStatus ResourceFault(const char *a, const char *b, char *out, size_t n)
{ (void)b; return UmiTestMcpEncodeResourceAllocationFailure(a, out, n); }

typedef struct Adapter {
    const char *kind, *field;
    Encode encode, allocation;
    size_t plain_limit;
    int raw;
} Adapter;
static const Adapter adapters[] = {
    {"escape", NULL, Escape, EscapeFault, UMI_AI_MCP_TEXT_CAPACITY - 1U, 0},
    {"initialize", "name", umi_ai_mcp_encode_initialize_params, UmiTestMcpEncodeInitializeAllocationFailure, UMI_AI_SMALL_TEXT_CAPACITY - 3U, 0},
    {"tool", "name", umi_ai_mcp_encode_tool_call_params, UmiTestMcpEncodeToolAllocationFailure, UMI_AI_SMALL_TEXT_CAPACITY - 3U, 1},
    {"resource", "uri", Resource, ResourceFault, UMI_AI_MCP_TEXT_CAPACITY - 3U, 0},
    {"prompt", "name", umi_ai_mcp_encode_prompt_get_params, UmiTestMcpEncodePromptAllocationFailure, UMI_AI_SMALL_TEXT_CAPACITY - 3U, 1}
};
typedef struct Fixture {
    const Adapter *api;
    char value[UMI_AI_MCP_TEXT_CAPACITY + 1U], extra[UMI_AI_MCP_TEXT_CAPACITY + 1U];
    char output[OUTPUT_CAPACITY], previous[OUTPUT_CAPACITY], decoded[UMI_AI_MCP_TEXT_CAPACITY + 1U];
} Fixture;
static int CheckEncoded(Fixture *f)
{
    UmiJsonTree *tree = NULL;
    const UmiJsonTreeLimits limits = {OUTPUT_CAPACITY, 131072U, 256U};
    CHECK(UmiJsonTreeCreate(f->output, strlen(f->output), &limits, NULL, &tree) == UMI_STATUS_OK);
    int node = 0, valid = 1;
    if (strcmp(f->api->kind, "initialize") == 0)
        valid = UmiJsonTreeMember(tree, 0, "clientInfo", &node) == UMI_STATUS_OK;
    if (valid && f->api->field != NULL)
        valid = UmiJsonTreeMember(tree, node, f->api->field, &node) == UMI_STATUS_OK;
    if (valid) valid = UmiJsonTreeText(tree, node, f->decoded, sizeof(f->decoded)) == UMI_STATUS_OK && strcmp(f->decoded, f->value) == 0;
    if (valid && f->api->raw) {
        valid = UmiJsonTreeMember(tree, 0, "arguments", &node) == UMI_STATUS_OK && UmiJsonTreeKind(tree, node) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT;
        /* Raw spelling, including surrounding whitespace, must survive insertion. */
        if (valid) valid = strstr(f->output, f->extra[0] != '\0' ? f->extra : "{}") != NULL;
    }
    if (valid && strcmp(f->api->kind, "initialize") == 0) {
        int client = -1;
        valid = UmiJsonTreeMember(tree, 0, "clientInfo", &client) == UMI_STATUS_OK &&
            UmiJsonTreeMember(tree, client, "version", &node) == UMI_STATUS_OK &&
            UmiJsonTreeText(tree, node, f->decoded, sizeof(f->decoded)) == UMI_STATUS_OK && strcmp(f->decoded, f->extra) == 0;
    }
    UmiJsonTreeDestroy(tree); CHECK(valid); return 0;
}
static UmiStatus CountRequest(void *data, const char *method, const char *params, char *out, size_t capacity)
{
    (void)method; (void)params; (void)out; (void)capacity;
    ++*(unsigned *)data; return UMI_STATUS_INTERNAL_ERROR;
}
static int CheckNoRequest(Fixture *f, UmiStatus expected)
{
    unsigned calls = 0U; UmiAiMcpSession session = {0};
    session.state = UMI_AI_MCP_SESSION_READY; session.revision = 1U;
    session.transport = (UmiAiMcpTransport){CountRequest, NULL, &calls};
    char result[32], previous[32]; memset(result, 0x5a, sizeof(result)); memcpy(previous, result, sizeof(result));
    UmiStatus status = strcmp(f->api->kind, "tool") == 0 ?
        umi_ai_mcp_call_tool(&session, f->value, f->extra, result, sizeof(result)) :
        strcmp(f->api->kind, "prompt") == 0 ? umi_ai_mcp_get_prompt(&session, f->value, f->extra, result, sizeof(result)) :
        umi_ai_mcp_read_resource(&session, f->value, result, sizeof(result));
    CHECK(status == expected && calls == 0U && memcmp(result, previous, sizeof(result)) == 0);
    return 0;
}

static int Exercise(Fixture *f, const char *name)
{
    strcpy(f->value, "entry"); strcpy(f->extra, strcmp(f->api->kind, "initialize") == 0 ? "1.0" : "{}");
    memset(f->output, 0x5a, sizeof(f->output));
    const char *first = f->value, *second = f->extra;
    size_t capacity = sizeof(f->output);
    UmiStatus expected = UMI_STATUS_OK;
    Encode encode = f->api->encode;
    int check_transport = 0;
    if (strcmp(name, "basic") == 0) { /* Defaults describe an ordinary call. */ }
    else if (strcmp(name, "empty") == 0) f->value[0] = '\0';
    else if (strcmp(name, "escaped") == 0) strcpy(f->value, "a\"b\\c\n\r\t\b\f\1");
    else if (strcmp(name, "unicode") == 0) strcpy(f->value, "\xc2\xa3\xe2\x82\xac\xf0\x9f\x98\x80");
    else if (strcmp(name, "invalid-utf8") == 0) { strcpy(f->value, "\xc0\xaf"); expected = UMI_STATUS_INVALID_ARGUMENT; check_transport = 1; }
    else if (strcmp(name, "exact") == 0 || strcmp(name, "short") == 0) {
        CHECK(encode(first, second, f->output, sizeof(f->output)) == UMI_STATUS_OK);
        capacity = strlen(f->output) + (strcmp(name, "exact") == 0 ? 1U : 0U);
        memset(f->output, 0x5a, sizeof(f->output));
        if (strcmp(name, "short") == 0) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "alias") == 0) { strcpy(f->output, first); first = f->output; }
    else if (strcmp(name, "field-last") == 0 || strcmp(name, "field-overflow") == 0) {
        size_t length = f->api->plain_limit + (strcmp(name, "field-overflow") == 0 ? 1U : 0U);
        memset(f->value, 'n', length); f->value[length] = '\0';
        if (strcmp(name, "field-overflow") == 0) { expected = UMI_STATUS_CAPACITY_EXCEEDED; check_transport = 1; }
    } else if (strcmp(name, "allocation") == 0) { encode = f->api->allocation; expected = UMI_STATUS_OUT_OF_MEMORY; }
    else if (strcmp(name, "nulls") == 0) {
        memcpy(f->previous, f->output, sizeof(f->output));
        CHECK(encode(NULL, second, f->output, capacity) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(encode(first, second, NULL, capacity) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(encode(first, second, f->output, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        if (f->api->raw || strcmp(f->api->kind, "initialize") == 0)
            CHECK(encode(first, NULL, f->output, capacity) == UMI_STATUS_INVALID_ARGUMENT);
        if (strcmp(f->api->kind, "escape") == 0)
            CHECK(encode(first, second, f->output, 2U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(f->output, f->previous, sizeof(f->output)) == 0); return 0;
    } else if (strncmp(name, "control-", 8U) == 0) {
        char *end = NULL; unsigned long value = strtoul(name + 8, &end, 10);
        CHECK(value > 0U && value < 32U && *end == '\0');
        f->value[0] = (char)value; f->value[1] = '\0';
    } else if (strncmp(name, "utf8-", 5U) == 0) {
        const char *invalid = strcmp(name, "utf8-surrogate") == 0 ? "\xed\xa0\x80" :
            strcmp(name, "utf8-truncated") == 0 ? "\xf0\x9f" :
            strcmp(name, "utf8-continuation") == 0 ? "\x80" :
            strcmp(name, "utf8-above-max") == 0 ? "\xf4\x90\x80\x80" :
            strcmp(name, "utf8-five-byte") == 0 ? "\xf8\x88\x80\x80\x80" : NULL;
        CHECK(invalid != NULL); strcpy(f->value, invalid); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strncmp(name, "version-", 8U) == 0) {
        CHECK(strcmp(f->api->kind, "initialize") == 0);
        if (strcmp(name, "version-last") == 0 || strcmp(name, "version-overflow") == 0) {
            size_t length = strcmp(name, "version-last") == 0 ? 253U : 254U;
            memset(f->extra, 'v', length); f->extra[length] = '\0';
            if (length == 254U) expected = UMI_STATUS_CAPACITY_EXCEEDED;
        } else if (strcmp(name, "version-alias") == 0) { strcpy(f->output, second); second = f->output; }
        else if (strcmp(name, "version-utf8") == 0) { strcpy(f->extra, "\x80"); expected = UMI_STATUS_INVALID_ARGUMENT; }
        else return 2;
    } else if (strncmp(name, "raw-", 4U) == 0) {
        CHECK(f->api->raw);
        if (strcmp(name, "raw-empty") == 0) f->extra[0] = '\0';
        else if (strcmp(name, "raw-spelling") == 0) strcpy(f->extra, " \n{ \"text\" : \"\\u20ac\", \"nested\" : [true,null] } \t");
        else if (strcmp(name, "raw-duplicate") == 0) strcpy(f->extra, "{\"x\":1,\"x\":2}");
        else if (strcmp(name, "raw-alias") == 0) { strcpy(f->extra, "{\"n\":7}"); strcpy(f->output, f->extra); second = f->output; }
        else if (strcmp(name, "raw-allocation") == 0) {
            encode = strcmp(f->api->kind, "tool") == 0 ? UmiTestMcpUnusedEncodeTool : UmiTestMcpUnusedEncodePrompt;
            expected = UMI_STATUS_OUT_OF_MEMORY;
        } else if (strcmp(name, "raw-byte-last") == 0 || strcmp(name, "raw-byte-overflow") == 0) {
            size_t length = strcmp(name, "raw-byte-last") == 0 ? UMI_AI_MCP_TEXT_CAPACITY - 1U : UMI_AI_MCP_TEXT_CAPACITY;
            strcpy(f->extra, "{}"); memset(f->extra + 2U, ' ', length - 2U); f->extra[length] = '\0';
            if (length == UMI_AI_MCP_TEXT_CAPACITY) { expected = UMI_STATUS_CAPACITY_EXCEEDED; check_transport = 1; }
        } else if (strcmp(name, "raw-depth") == 0) {
            strcpy(f->extra, "{\"x\":"); size_t at = strlen(f->extra);
            for (size_t i = 0U; i < 128U; ++i) f->extra[at++] = '[';
            f->extra[at++] = '0';
            for (size_t i = 0U; i < 128U; ++i) f->extra[at++] = ']';
            f->extra[at++] = '}'; f->extra[at] = '\0'; expected = UMI_STATUS_CAPACITY_EXCEEDED; check_transport = 1;
        } else {
            const char *invalid = strcmp(name, "raw-array") == 0 ? "[]" : strcmp(name, "raw-string") == 0 ? "\"text\"" :
                strcmp(name, "raw-number") == 0 ? "1" : strcmp(name, "raw-null") == 0 ? "null" :
                strcmp(name, "raw-trailing") == 0 ? "{} {}" : strcmp(name, "raw-unclosed") == 0 ? "{\"x\":1" :
                strcmp(name, "raw-injection") == 0 ? "{},\"surprise\":true" :
                strcmp(name, "raw-escape") == 0 ? "{\"x\":\"\\q\"}" :
                strcmp(name, "raw-whitespace") == 0 ? " \t\n" : NULL;
            CHECK(invalid != NULL); strcpy(f->extra, invalid); expected = UMI_STATUS_PARSE_ERROR; check_transport = 1;
        }
    } else return 2;
    memcpy(f->previous, f->output, sizeof(f->output));
    UmiStatus status = encode(first, second, f->output, capacity);
    CHECK(status == expected);
    if (status == UMI_STATUS_OK) CHECK(CheckEncoded(f) == 0);
    else CHECK(memcmp(f->output, f->previous, sizeof(f->output)) == 0);
    if (check_transport && (f->api->raw || strcmp(f->api->kind, "resource") == 0))
        CHECK(CheckNoRequest(f, expected) == 0);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const Adapter *api = NULL;
    for (size_t i = 0U; i < sizeof(adapters) / sizeof(adapters[0]); ++i)
        if (strcmp(argv[1], adapters[i].kind) == 0) api = &adapters[i];
    if (api == NULL) return 2;
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    if (fixture == NULL) return 3;
    fixture->api = api; int result = Exercise(fixture, argv[2]); free(fixture); return result;
}
