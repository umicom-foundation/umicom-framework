/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_session_revisions.c
 *
 * PURPOSE:
 *   Verify saturated revisions and callback-owned MCP lifecycle state.
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

typedef struct Fixture {
    UmiAiMcpSession session;
    UmiAiMcpSession previous;
    UmiTestMcpTransportState protocol;
    const char *stage;
    const char *action;
    unsigned requests;
    unsigned notifications;
    int error_stage;
    int notify_unsupported;
    int notification_phase_valid;
    UmiStatus mutation_status;
} Fixture;

static void Mutate(Fixture *fixture)
{
    if (strcmp(fixture->action, "revision") == 0) ++fixture->session.revision;
    else if (strcmp(fixture->action, "exhaust") == 0) fixture->session.revision = UINT64_MAX;
    else if (strcmp(fixture->action, "disconnect") == 0)
        fixture->mutation_status = UmiAiMcpSessionDisconnectChecked(&fixture->session);
    else if (strcmp(fixture->action, "state") == 0) fixture->session.state = UMI_AI_MCP_SESSION_FAILED;
}

static UmiStatus Request(void *data, const char *method, const char *params,
    char *result, size_t capacity)
{
    Fixture *fixture = data;
    ++fixture->requests;
    UmiStatus status = umi_test_mcp_request(&fixture->protocol, method, params, result, capacity);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(fixture->stage, "transport") == 0) Mutate(fixture);
    return fixture->error_stage == 1 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}

static UmiStatus Notify(void *data, const char *method, const char *params)
{
    Fixture *fixture = data;
    ++fixture->notifications;
    fixture->notification_phase_valid = fixture->session.state == UMI_AI_MCP_SESSION_INITIALIZED &&
        fixture->session.capabilities.protocol_version[0] != '\0';
    UmiStatus status = umi_test_mcp_notify(&fixture->protocol, method, params);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(fixture->stage, "notify") == 0) Mutate(fixture);
    if (fixture->error_stage == 2) return UMI_STATUS_IO_ERROR;
    return fixture->notify_unsupported ? UMI_STATUS_NOT_IMPLEMENTED : UMI_STATUS_OK;
}

static int Setup(Fixture *fixture)
{
    fixture->stage = "none"; fixture->action = "none";
    UmiAiMcpServerDescriptor server;
    CHECK(umi_test_mcp_server(&server) == UMI_STATUS_OK);
    UmiAiMcpTransport transport = {Request, Notify, fixture};
    CHECK(umi_ai_mcp_session_init(&fixture->session, &server, &transport) == UMI_STATUS_OK);
    fixture->session.revision = 5U;
    strcpy(fixture->session.capabilities.server_name, "old-capabilities");
    memcpy(&fixture->previous, &fixture->session, sizeof(fixture->previous));
    return 0;
}

static int Callback(Fixture *fixture, const char *stage, const char *action, const char *outcome)
{
    fixture->stage = stage; fixture->action = action;
    if (strcmp(outcome, "error") == 0) fixture->error_stage = strcmp(stage, "transport") == 0 ? 1 : 2;
    UmiStatus status = umi_ai_mcp_session_initialize(&fixture->session, "fixture", "1");
    CHECK(status == (fixture->error_stage != 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_INVALID_STATE));
    CHECK(fixture->requests == 1U && fixture->notifications == (strcmp(stage, "notify") == 0 ? 1U : 0U));
    CHECK(fixture->mutation_status == UMI_STATUS_OK);
    const uint64_t revision = strcmp(action, "exhaust") == 0 ? UINT64_MAX : strcmp(action, "state") == 0 ? 5U : 6U;
    const UmiAiMcpSessionState expected = strcmp(action, "disconnect") == 0 ? UMI_AI_MCP_SESSION_DISCONNECTED :
        strcmp(action, "state") == 0 ? UMI_AI_MCP_SESSION_FAILED :
        strcmp(stage, "transport") == 0 ? UMI_AI_MCP_SESSION_CONNECTED : UMI_AI_MCP_SESSION_INITIALIZED;
    CHECK(fixture->session.revision == revision && fixture->session.state == expected);
    if (strcmp(stage, "notify") == 0) CHECK(fixture->notification_phase_valid);
    if (strcmp(stage, "transport") == 0 && strcmp(action, "disconnect") != 0)
        CHECK(memcmp(&fixture->session.capabilities, &fixture->previous.capabilities, sizeof(fixture->session.capabilities)) == 0);
    if (strcmp(action, "disconnect") == 0) {
        UmiAiMcpServerCapabilities empty = {0};
        CHECK(memcmp(&fixture->session.capabilities, &empty, sizeof(empty)) == 0);
    }
    return 0;
}

static int Normal(Fixture *fixture, const char *name)
{
    if (strcmp(name, "no-notify") == 0) fixture->session.transport.notify = NULL;
    if (strcmp(name, "unsupported") == 0) fixture->notify_unsupported = 1;
    if (strcmp(name, "transport-error") == 0) fixture->error_stage = 1;
    if (strcmp(name, "notify-error") == 0) fixture->error_stage = 2;
    if (strcmp(name, "not-connected") == 0) {
        fixture->session.state = UMI_AI_MCP_SESSION_READY;
        memcpy(&fixture->previous, &fixture->session, sizeof(fixture->previous));
        CHECK(umi_ai_mcp_session_initialize(&fixture->session, "fixture", "1") == UMI_STATUS_INVALID_STATE);
        CHECK(fixture->requests == 0U && fixture->notifications == 0U);
        CHECK(memcmp(&fixture->session, &fixture->previous, sizeof(fixture->previous)) == 0);
        return 0;
    }
    UmiStatus status = umi_ai_mcp_session_initialize(&fixture->session, "fixture", "1");
    if (fixture->error_stage != 0) {
        CHECK(status == UMI_STATUS_IO_ERROR);
        CHECK(fixture->session.state == UMI_AI_MCP_SESSION_FAILED && fixture->session.revision == 5U);
    } else {
        CHECK(status == UMI_STATUS_OK && fixture->session.state == UMI_AI_MCP_SESSION_READY);
        CHECK(fixture->session.revision == 6U);
    }
    CHECK(fixture->requests == 1U);
    const unsigned notifications = fixture->error_stage == 1 || strcmp(name, "no-notify") == 0 ? 0U : 1U;
    CHECK(fixture->notifications == notifications);
    if (notifications != 0U) CHECK(fixture->notification_phase_valid);
    return 0;
}

static int Limit(Fixture *fixture, const char *name)
{
    const int last = strstr(name, "last") != NULL;
    const int initialize = strncmp(name, "initialize-", 11U) == 0;
    const int legacy = strncmp(name, "legacy-", 7U) == 0;
    fixture->session.revision = last ? UINT64_MAX - 1U : UINT64_MAX;
    memcpy(&fixture->previous, &fixture->session, sizeof(fixture->previous));
    if (initialize) {
        UmiStatus status = umi_ai_mcp_session_initialize(&fixture->session, "fixture", "1");
        CHECK(status == (last ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(fixture->requests == (last ? 1U : 0U) && fixture->notifications == (last ? 1U : 0U));
        if (last) CHECK(fixture->session.state == UMI_AI_MCP_SESSION_READY && fixture->session.revision == UINT64_MAX);
    } else {
        if (legacy) umi_ai_mcp_session_disconnect(&fixture->session);
        else CHECK(UmiAiMcpSessionDisconnectChecked(&fixture->session) == (last ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(fixture->requests == 0U && fixture->notifications == 0U);
        if (last) {
            UmiAiMcpServerCapabilities empty = {0};
            CHECK(fixture->session.state == UMI_AI_MCP_SESSION_DISCONNECTED && fixture->session.revision == UINT64_MAX);
            CHECK(memcmp(&fixture->session.capabilities, &empty, sizeof(empty)) == 0);
            CHECK(memcmp(&fixture->session.server, &fixture->previous.server, sizeof(fixture->session.server)) == 0);
            CHECK(memcmp(&fixture->session.transport, &fixture->previous.transport, sizeof(fixture->session.transport)) == 0);
        }
    }
    if (!last) CHECK(memcmp(&fixture->session, &fixture->previous, sizeof(fixture->previous)) == 0);
    if (last) {
        memcpy(&fixture->previous, &fixture->session, sizeof(fixture->previous));
        CHECK(UmiAiMcpSessionDisconnectChecked(&fixture->session) == UMI_STATUS_CAPACITY_EXCEEDED);
        umi_ai_mcp_session_disconnect(&fixture->session);
        CHECK(memcmp(&fixture->session, &fixture->previous, sizeof(fixture->previous)) == 0);
    }
    return 0;
}

static int Nulls(Fixture *fixture)
{
    CHECK(umi_ai_mcp_session_initialize(NULL, "fixture", "1") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ai_mcp_session_initialize(&fixture->session, NULL, "1") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ai_mcp_session_initialize(&fixture->session, "fixture", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiMcpSessionDisconnectChecked(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    umi_ai_mcp_session_disconnect(NULL);
    CHECK(fixture->requests == 0U && fixture->notifications == 0U);
    CHECK(memcmp(&fixture->session, &fixture->previous, sizeof(fixture->previous)) == 0);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 4) return 2;
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    if (fixture == NULL) return 3;
    int result = Setup(fixture);
    if (result == 0) {
        if (strcmp(argv[1], "transport") == 0 || strcmp(argv[1], "notify") == 0)
            result = Callback(fixture, argv[1], argv[2], argv[3]);
        else if (strcmp(argv[1], "normal") == 0) result = Normal(fixture, argv[2]);
        else if (strcmp(argv[1], "limit") == 0) result = Limit(fixture, argv[2]);
        else if (strcmp(argv[1], "nulls") == 0) result = Nulls(fixture);
        else result = 2;
    }
    free(fixture);
    return result;
}
