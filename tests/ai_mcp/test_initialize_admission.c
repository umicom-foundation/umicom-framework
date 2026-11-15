/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_initialize_admission.c
 *
 * PURPOSE:
 *   Verify complete initialization replies, capability scope and negotiated-version admission.
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
#include <stdint.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

UmiStatus UmiTestMcpInitializeAllocationFailure(const char *json, UmiAiMcpServerCapabilities *out);

typedef struct ReplyCase { const char *name; const char *json; UmiStatus status; uint32_t flags; } ReplyCase;
static const ReplyCase cases[] = {
    {"valid", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{\"tools\":{},\"resources\":{},\"prompts\":{},\"logging\":{}},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_OK, 15U},
    {"empty-capabilities", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_OK, 0U},
    {"nested-capabilities", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{\"experimental\":{\"tools\":{},\"resources\":{},\"prompts\":{},\"logging\":{}}},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_OK, 0U},
    {"root-capabilities", "{\"protocolVersion\":\"2025-06-18\",\"tools\":{},\"resources\":{},\"prompts\":{},\"logging\":{},\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_OK, 0U},
    {"unknown-duplicates", "{\"protocolVersion\":\"2025-06-18\",\"extra\":1,\"extra\":2,\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_OK, 0U},
    {"unicode", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"Caf\\u00e9\",\"version\":\"1\\uD83D\\uDE00\"}}", UMI_STATUS_OK, 0U},
    {"escaped-keys", "{\"protocol\\u0056ersion\":\"2025-06-18\",\"capabilities\":{\"to\\u006fls\":{}},\"serverInfo\":{\"na\\u006de\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_OK, 1U},
    {"empty-version", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"\"}}", UMI_STATUS_OK, 0U},
    {"other-version", "{\"protocolVersion\":\"2099-01-01\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_OK, 0U},
    {"empty-object", "{}", UMI_STATUS_PARSE_ERROR, 0U},
    {"array-root", "[]", UMI_STATUS_PARSE_ERROR, 0U},
    {"scalar-root", "true", UMI_STATUS_PARSE_ERROR, 0U},
    {"missing-protocol", "{\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"nested-protocol", "{\"extra\":{\"protocolVersion\":\"2025-06-18\"},\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"protocol-kind", "{\"protocolVersion\":1,\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"protocol-empty", "{\"protocolVersion\":\"\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"missing-capabilities", "{\"protocolVersion\":\"2025-06-18\",\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"capabilities-kind", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":[],\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"missing-server", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"server-kind", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":null}", UMI_STATUS_PARSE_ERROR, 0U},
    {"missing-name", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"name-empty", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"\",\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"name-kind", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":[],\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"missing-version", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"version-kind", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":1}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"duplicate-protocol", "{\"protocolVersion\":\"2025-06-18\",\"protocol\\u0056ersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_ALREADY_EXISTS, 0U},
    {"duplicate-capabilities", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_ALREADY_EXISTS, 0U},
    {"duplicate-server", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"},\"serverInfo\":{}}", UMI_STATUS_ALREADY_EXISTS, 0U},
    {"duplicate-name", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"name\":\"other\",\"version\":\"1\"}}", UMI_STATUS_ALREADY_EXISTS, 0U},
    {"duplicate-version", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\",\"version\":\"2\"}}", UMI_STATUS_ALREADY_EXISTS, 0U},
    {"invalid-extension", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"},\"extra\":01}", UMI_STATUS_PARSE_ERROR, 0U},
    {"nul-extension", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"},\"extra\":\"\\u0000\"}", UMI_STATUS_PARSE_ERROR, 0U},
    {"surrogate-extension", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"},\"extra\":\"\\uD800\"}", UMI_STATUS_PARSE_ERROR, 0U},
    {"trailing", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}} false", UMI_STATUS_PARSE_ERROR, 0U},
    {"truncated", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}", UMI_STATUS_PARSE_ERROR, 0U},
    {"tools-kind", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{\"tools\":false},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"tools-duplicate", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{\"tools\":{},\"tools\":{}},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_ALREADY_EXISTS, 0U},
    {"resources-kind", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{\"resources\":false},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"resources-duplicate", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{\"resources\":{},\"resources\":{}},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_ALREADY_EXISTS, 0U},
    {"prompts-kind", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{\"prompts\":false},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"prompts-duplicate", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{\"prompts\":{},\"prompts\":{}},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_ALREADY_EXISTS, 0U},
    {"logging-kind", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{\"logging\":false},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_PARSE_ERROR, 0U},
    {"logging-duplicate", "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{\"logging\":{},\"logging\":{}},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}", UMI_STATUS_ALREADY_EXISTS, 0U}
};
typedef struct Fixture {
    UmiAiMcpSession session;
    UmiAiMcpServerCapabilities output, previous;
    char reply[UMI_AI_MCP_TEXT_CAPACITY + 1U];
    size_t length;
    unsigned requests, notifications;
} Fixture;

static UmiStatus Request(void *data, const char *method, const char *params, char *out, size_t capacity)
{
    Fixture *fixture = data;
    (void)params;
    if (strcmp(method, "initialize") != 0) return UMI_STATUS_INVALID_ARGUMENT;
    ++fixture->requests;
    if (fixture->length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, fixture->reply, fixture->length + 1U);
    return UMI_STATUS_OK;
}
static UmiStatus Notify(void *data, const char *method, const char *params)
{
    Fixture *fixture = data;
    (void)params;
    if (strcmp(method, "notifications/initialized") != 0) return UMI_STATUS_INVALID_ARGUMENT;
    ++fixture->notifications;
    return UMI_STATUS_OK;
}

/* Bounded generated values exercise exact inline storage and transport limits. */
static int Prepare(Fixture *fixture, const char *name, UmiStatus *expected, uint32_t *flags)
{
    *flags = 0U; *expected = UMI_STATUS_OK;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        if (strcmp(name, cases[i].name) == 0) {
            strcpy(fixture->reply, cases[i].json); *expected = cases[i].status; *flags = cases[i].flags;
            fixture->length = strlen(fixture->reply); return 0;
        }
    }
    const char *protocol = "2025-06-18", *server = "fixture", *version = "1";
    char long_value[UMI_AI_SMALL_TEXT_CAPACITY + 1U];
    size_t size = 0U;
    if (strncmp(name, "protocol-", 9U) == 0) size = sizeof(fixture->output.protocol_version);
    else if (strncmp(name, "name-", 5U) == 0) size = sizeof(fixture->output.server_name);
    else if (strncmp(name, "version-", 8U) == 0) size = sizeof(fixture->output.server_version);
    if (size != 0U) {
        const int overflow = strstr(name, "overflow") != NULL;
        size_t length = overflow ? size : size - 1U;
        memset(long_value, 'x', length); long_value[length] = '\0';
        if (strncmp(name, "protocol-", 9U) == 0) protocol = long_value;
        else if (strncmp(name, "name-", 5U) == 0) server = long_value;
        else version = long_value;
        if (overflow) *expected = UMI_STATUS_CAPACITY_EXCEEDED;
        int count = snprintf(fixture->reply, sizeof(fixture->reply),
            "{\"protocolVersion\":\"%s\",\"capabilities\":{},\"serverInfo\":{\"name\":\"%s\",\"version\":\"%s\"}}",
            protocol, server, version);
        CHECK(count > 0 && (size_t)count < sizeof(fixture->reply));
    } else if (strcmp(name, "byte-last") == 0 || strcmp(name, "byte-overflow") == 0) {
        strcpy(fixture->reply, cases[1].json);
        size_t length = strlen(fixture->reply);
        size_t end = strcmp(name, "byte-last") == 0 ? UMI_AI_MCP_TEXT_CAPACITY - 1U : UMI_AI_MCP_TEXT_CAPACITY;
        memset(fixture->reply + length, ' ', end - length); fixture->reply[end] = '\0';
        if (end == UMI_AI_MCP_TEXT_CAPACITY) *expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "depth-overflow") == 0) {
        strcpy(fixture->reply, "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"},\"extra\":");
        size_t length = strlen(fixture->reply);
        memset(fixture->reply + length, '[', 129U); length += 129U;
        fixture->reply[length++] = '0'; memset(fixture->reply + length, ']', 129U); length += 129U;
        fixture->reply[length++] = '}'; fixture->reply[length] = '\0';
        *expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "invalid-utf8") == 0) {
        strcpy(fixture->reply, "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"},\"extra\":\"x\"}");
        char *extra = strstr(fixture->reply, "\"x\""); CHECK(extra != NULL); extra[1] = (char)0xc0;
        *expected = UMI_STATUS_PARSE_ERROR;
    } else return 2;
    fixture->length = strlen(fixture->reply); return 0;
}

static int Decode(Fixture *fixture, const char *name)
{
    UmiStatus expected; uint32_t flags;
    CHECK(Prepare(fixture, name, &expected, &flags) == 0);
    UmiStatus status = umi_ai_mcp_decode_initialize_result(fixture->reply, &fixture->output);
    CHECK(status == expected);
    if (status != UMI_STATUS_OK) CHECK(memcmp(&fixture->output, &fixture->previous, sizeof(fixture->output)) == 0);
    else {
        CHECK(fixture->output.flags == flags);
        CHECK(fixture->output.protocol_version[0] != '\0' && fixture->output.server_name[0] != '\0');
        if (strcmp(name, "unicode") == 0) {
            CHECK(strcmp(fixture->output.server_name, "Caf\xc3\xa9") == 0);
            CHECK(strcmp(fixture->output.server_version, "1\xf0\x9f\x98\x80") == 0);
        }
        if (strcmp(name, "empty-version") == 0) CHECK(fixture->output.server_version[0] == '\0');
    }
    return 0;
}
static int Session(Fixture *fixture, const char *name)
{
    UmiStatus expected; uint32_t flags;
    CHECK(Prepare(fixture, name, &expected, &flags) == 0);
    UmiAiMcpServerDescriptor server;
    CHECK(umi_test_mcp_server(&server) == UMI_STATUS_OK);
    UmiAiMcpTransport transport = {Request, Notify, fixture};
    CHECK(umi_ai_mcp_session_init(&fixture->session, &server, &transport) == UMI_STATUS_OK);
    fixture->session.revision = 7U; fixture->session.capabilities = fixture->previous;
    if (strcmp(name, "other-version") == 0 || strcmp(name, "protocol-boundary") == 0) expected = UMI_STATUS_NOT_IMPLEMENTED;
    CHECK(umi_ai_mcp_session_initialize(&fixture->session, "fixture", "1") == expected);
    CHECK(fixture->requests == 1U);
    if (expected == UMI_STATUS_OK) {
        CHECK(fixture->session.state == UMI_AI_MCP_SESSION_READY && fixture->session.revision == 8U);
        CHECK(fixture->notifications == 1U && fixture->session.capabilities.flags == flags);
    } else {
        CHECK(fixture->session.state == UMI_AI_MCP_SESSION_FAILED && fixture->session.revision == 7U);
        CHECK(fixture->notifications == 0U);
        CHECK(memcmp(&fixture->session.capabilities, &fixture->previous, sizeof(fixture->previous)) == 0);
    }
    return 0;
}
static int Special(Fixture *fixture, const char *name)
{
    if (strcmp(name, "nulls") == 0) {
        CHECK(umi_ai_mcp_decode_initialize_result(NULL, &fixture->output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_mcp_decode_initialize_result(cases[0].json, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&fixture->output, &fixture->previous, sizeof(fixture->output)) == 0);
    } else if (strcmp(name, "allocation") == 0) {
        CHECK(UmiTestMcpInitializeAllocationFailure(cases[0].json, &fixture->output) == UMI_STATUS_OUT_OF_MEMORY);
        CHECK(memcmp(&fixture->output, &fixture->previous, sizeof(fixture->output)) == 0);
    } else if (strcmp(name, "alias") == 0) {
        union { UmiAiMcpServerCapabilities value; char input[UMI_AI_MCP_TEXT_CAPACITY]; } *storage = calloc(1U, sizeof(*storage));
        CHECK(storage != NULL);
        strcpy(storage->input, cases[0].json);
        UmiStatus status = umi_ai_mcp_decode_initialize_result(storage->input, &storage->value);
        int valid = status == UMI_STATUS_OK && storage->value.flags == 15U && strcmp(storage->value.server_name, "fixture") == 0;
        free(storage); CHECK(valid);
    } else return 2;
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    if (fixture == NULL) return 3;
    memset(&fixture->output, 0x5a, sizeof(fixture->output));
    memcpy(&fixture->previous, &fixture->output, sizeof(fixture->previous));
    int result = strcmp(argv[1], "codec") == 0 ? Decode(fixture, argv[2]) :
        strcmp(argv[1], "session") == 0 ? Session(fixture, argv[2]) :
        strcmp(argv[1], "special") == 0 ? Special(fixture, argv[2]) : 2;
    free(fixture); return result;
}
