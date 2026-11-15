/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_checked_transport.c
 *
 * PURPOSE:
 *   Verify staged transport text and high-level MCP decoder admission.
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

UmiStatus UmiTestMcpTransportAllocationFailure(const UmiAiMcpTransport *transport,
    const char *method, const char *params, char *output, size_t capacity);

typedef struct Fixture {
    UmiAiMcpTransport transport;
    UmiAiMcpSession session;
    UmiAiMcpToolCatalogue catalogue;
    UmiTestMcpTransportState setup_transport;
    const char *mode;
    unsigned calls;
    char seen_params[64];
} Fixture;

static UmiStatus Request(void *data, const char *method, const char *params,
    char *output, size_t capacity)
{
    Fixture *fixture = data;
    ++fixture->calls;
    (void)snprintf(fixture->seen_params, sizeof(fixture->seen_params), "%s", params);
    if (strcmp(fixture->mode, "no-write") == 0) return UMI_STATUS_OK;
    if (strcmp(fixture->mode, "partial") == 0) { output[0] = 'x'; return UMI_STATUS_OK; }
    if (strcmp(fixture->mode, "malformed") == 0 || strcmp(fixture->mode, "error-malformed") == 0) {
        memset(output, 'x', capacity);
        return strcmp(fixture->mode, "malformed") == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    }
    if (strcmp(fixture->mode, "last-byte") == 0) {
        memset(output, 'x', capacity); output[capacity - 1U] = '\0'; return UMI_STATUS_OK;
    }
    if (strcmp(fixture->mode, "empty") == 0) { output[0] = '\0'; return UMI_STATUS_OK; }
    const char *reply = "{}";
    if (strcmp(method, "initialize") == 0)
        reply = "{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{},\"serverInfo\":{\"name\":\"fixture\",\"version\":\"1\"}}";
    else if (strcmp(method, "tools/list") == 0) reply = "{\"tools\":[]}";
    if (strlen(reply) >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(output, reply, strlen(reply) + 1U);
    if (strcmp(fixture->mode, "tuple-change") == 0) fixture->transport.request = NULL;
    return strcmp(fixture->mode, "error") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}

static int Unit(Fixture *fixture, const char *name)
{
    char output[64]; char before[64];
    memset(output, 'p', sizeof(output)); output[sizeof(output) - 1U] = '\0';
    memcpy(before, output, sizeof(before));
    fixture->mode = name;
    if (strcmp(name, "allocation") == 0) {
        CHECK(UmiTestMcpTransportAllocationFailure(&fixture->transport, "test", "{}", output, sizeof(output)) == UMI_STATUS_OUT_OF_MEMORY);
        CHECK(fixture->calls == 0U && memcmp(output, before, sizeof(output)) == 0);
        return 0;
    }
    if (strcmp(name, "legacy-error") == 0) {
        fixture->mode = "error";
        CHECK(umi_ai_mcp_transport_request(&fixture->transport, "test", "{}", output, sizeof(output)) == UMI_STATUS_IO_ERROR);
        CHECK(fixture->calls == 1U && strcmp(output, "{}") == 0);
        return 0;
    }
    if (strcmp(name, "alias-input") == 0) {
        strcpy(output, "{\"old\":1}"); fixture->mode = "success";
        CHECK(UmiAiMcpTransportRequestChecked(&fixture->transport, "test", output, output, sizeof(output)) == UMI_STATUS_OK);
        CHECK(strcmp(fixture->seen_params, "{\"old\":1}") == 0 && strcmp(output, "{}") == 0);
        return 0;
    }
    size_t capacity = strcmp(name, "one-byte") == 0 ? 1U : sizeof(output);
    if (strcmp(name, "one-byte") == 0) fixture->mode = "empty";
    UmiStatus status = UmiAiMcpTransportRequestChecked(&fixture->transport, "test", "{}", output, capacity);
    CHECK(fixture->calls == 1U);
    if (strcmp(name, "malformed") == 0 || strcmp(name, "no-write") == 0 || strcmp(name, "partial") == 0) {
        CHECK(status == UMI_STATUS_INTERNAL_ERROR && memcmp(output, before, sizeof(output)) == 0);
    } else if (strcmp(name, "error") == 0 || strcmp(name, "error-malformed") == 0) {
        CHECK(status == UMI_STATUS_IO_ERROR && memcmp(output, before, sizeof(output)) == 0);
    } else if (strcmp(name, "last-byte") == 0) {
        CHECK(status == UMI_STATUS_OK && strlen(output) == sizeof(output) - 1U);
        for (size_t index = 0U; index + 1U < sizeof(output); ++index) CHECK(output[index] == 'x');
    } else if (strcmp(name, "empty") == 0 || strcmp(name, "one-byte") == 0) {
        CHECK(status == UMI_STATUS_OK && output[0] == '\0');
        CHECK(memcmp(output + 1U, before + 1U, sizeof(output) - 1U) == 0);
    } else {
        CHECK(status == UMI_STATUS_OK && strcmp(output, "{}") == 0);
        CHECK(memcmp(output + 3U, before + 3U, sizeof(output) - 3U) == 0);
        if (strcmp(name, "tuple-change") == 0) CHECK(fixture->transport.request == NULL);
    }
    return 0;
}

static int Nulls(Fixture *fixture)
{
    char output[64] = "previous";
    CHECK(UmiAiMcpTransportRequestChecked(NULL, "test", "{}", output, sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiMcpTransportRequestChecked(&fixture->transport, NULL, "{}", output, sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiMcpTransportRequestChecked(&fixture->transport, "test", NULL, output, sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiMcpTransportRequestChecked(&fixture->transport, "test", "{}", NULL, sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiMcpTransportRequestChecked(&fixture->transport, "test", "{}", output, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    fixture->transport.request = NULL;
    CHECK(UmiAiMcpTransportRequestChecked(&fixture->transport, "test", "{}", output, sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(fixture->calls == 0U && strcmp(output, "previous") == 0);
    return 0;
}

static int Consumer(Fixture *fixture, const char *entry, const char *mode)
{
    CHECK(umi_test_mcp_ready_session(&fixture->session, &fixture->setup_transport) == UMI_STATUS_OK);
    fixture->session.transport = fixture->transport;
    fixture->mode = mode;
    char output[64] = "previous";
    UmiStatus status;
    if (strcmp(entry, "tool") == 0)
        status = umi_ai_mcp_call_tool(&fixture->session, "read", "{}", output, sizeof(output));
    else if (strcmp(entry, "resource") == 0)
        status = umi_ai_mcp_read_resource(&fixture->session, "fixture:resource", output, sizeof(output));
    else if (strcmp(entry, "prompt") == 0)
        status = umi_ai_mcp_get_prompt(&fixture->session, "explain", "{}", output, sizeof(output));
    else if (strcmp(entry, "discovery") == 0)
        status = umi_ai_mcp_discover_tools(&fixture->session, &fixture->catalogue);
    else if (strcmp(entry, "initialize") == 0) {
        fixture->session.state = UMI_AI_MCP_SESSION_CONNECTED;
        fixture->session.transport.notify = NULL;
        status = umi_ai_mcp_session_initialize(&fixture->session, "fixture", "1");
    } else if (strcmp(entry, "ping") == 0) status = umi_ai_mcp_session_ping(&fixture->session);
    else return 1;
    const UmiStatus expected = strcmp(mode, "success") == 0 ? UMI_STATUS_OK :
        strcmp(mode, "error") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_INTERNAL_ERROR;
    CHECK(status == expected && fixture->calls == 1U);
    if (strcmp(entry, "tool") == 0 || strcmp(entry, "resource") == 0 || strcmp(entry, "prompt") == 0)
        CHECK(strcmp(output, status == UMI_STATUS_OK ? "{}" : "previous") == 0);
    if (strcmp(entry, "initialize") == 0)
        CHECK(fixture->session.state == (status == UMI_STATUS_OK ? UMI_AI_MCP_SESSION_READY : UMI_AI_MCP_SESSION_FAILED));
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 4) return 2;
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    if (fixture == NULL) return 3;
    fixture->transport.request = Request; fixture->transport.user_data = fixture;
    int result;
    if (strcmp(argv[1], "unit") == 0) result = Unit(fixture, argv[2]);
    else if (strcmp(argv[1], "nulls") == 0) result = Nulls(fixture);
    else if (strcmp(argv[1], "consumer") == 0 && argc == 4) result = Consumer(fixture, argv[2], argv[3]);
    else result = 2;
    free(fixture);
    return result;
}
