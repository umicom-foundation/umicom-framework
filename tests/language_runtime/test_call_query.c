/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_call_query.c
 * PURPOSE: Check same-session call preparation, complete identities, bounded expansion and cleanup before publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language_runtime/server_manager.h"
#include "umicom/platform/clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                                         \
    do                                                                                                       \
    {                                                                                                        \
        if (!(value))                                                                                        \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                                      \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)

typedef struct Fixture
{
    char input[16384], written[16384];
    size_t size, offset, writeSize, fragment;
    unsigned reads, stops, destroys, delay;
    uint32_t largestTimeout, firstTimeout, lastTimeout;
    int running, failWrite, failRead, oversize, failClose;
    int follow_fault, follow_count;
    UmiCancellationToken *cancel;
} Fixture;

static UmiStatus Write(void *instance, const void *bytes, size_t size)
{
    Fixture *fixture = instance;
    if (fixture->failWrite ||
        (fixture->failClose && strstr((const char *)bytes, "textDocument/didClose") != NULL))
        return UMI_STATUS_IO_ERROR;
    CHECK(size < sizeof(fixture->written) - fixture->writeSize);
    memcpy(fixture->written + fixture->writeSize, bytes, size);
    fixture->writeSize += size;
    fixture->written[fixture->writeSize] = '\0';
    if (strstr(fixture->written, "callHierarchy/incomingCalls") != NULL ||
        strstr(fixture->written, "callHierarchy/outgoingCalls") != NULL)
    {
        if (fixture->follow_count++ == 0)
        {
            if (fixture->follow_fault == 1 && fixture->cancel != NULL)
                umi_cancellation_token_request(fixture->cancel);
            if (fixture->follow_fault == 2)
            {
                fixture->delay = 20U;
                fixture->fragment = 1U;
            }
            if (fixture->follow_fault == 3)
                fixture->failRead = 1;
            if (fixture->follow_fault == 4)
                return UMI_STATUS_IO_ERROR;
        }
    }

    return UMI_STATUS_OK;
}
static UmiStatus Read(void *instance, void *bytes, size_t capacity, uint32_t timeout, size_t *out)
{
    Fixture *fixture = instance;
    if (fixture->reads == 0U)
        fixture->firstTimeout = timeout;
    fixture->lastTimeout = timeout;
    ++fixture->reads;
    if (timeout > fixture->largestTimeout)
        fixture->largestTimeout = timeout;
    *out = 0U;
    if (fixture->cancel != NULL && fixture->follow_fault == 1 && fixture->follow_count != 0)
        umi_cancellation_token_request(fixture->cancel);
    if (fixture->failRead)
        return UMI_STATUS_IO_ERROR;
    if (fixture->oversize)
    {
        *out = capacity + 1U;
        return UMI_STATUS_OK;
    }
    if (fixture->delay != 0U)
    {
        UmiClock clock = umi_clock_system();
        (void)clock.sleep_milliseconds(&clock, fixture->delay);
    }
    if (fixture->offset == fixture->size)
        return UMI_STATUS_NOT_FOUND;
    size_t count = fixture->size - fixture->offset;
    const char *body = strstr(fixture->input + fixture->offset, "\r\n\r\n");
    if (body != NULL)
    {
        unsigned payload = 0U;
        if (sscanf(fixture->input + fixture->offset, "Content-Length: %u", &payload) == 1)
        {
            size_t frame = (size_t)(body - (fixture->input + fixture->offset)) + 4U + payload;
            if (frame < count)
                count = frame;
        }
    }
    if (count > capacity)
        count = capacity;
    if (fixture->fragment != 0U && count > fixture->fragment)
        count = fixture->fragment;
    memcpy(bytes, fixture->input + fixture->offset, count);
    fixture->offset += count;
    *out = count;
    return UMI_STATUS_OK;
}
static UmiStatus Stop(void *instance, uint32_t timeout)
{
    Fixture *fixture = instance;
    (void)timeout;
    ++fixture->stops;
    fixture->running = 0;
    return UMI_STATUS_OK;
}
static int Running(void *instance) { return ((Fixture *)instance)->running; }
static void Destroy(void *instance) { ++((Fixture *)instance)->destroys; }

static void Push(Fixture *fixture, const char *json)
{
    size_t size = 0U;
    CHECK(umi_language_runtime_frame_encode(json, fixture->input + fixture->size,
                                            sizeof(fixture->input) - fixture->size, &size) == UMI_STATUS_OK);
    fixture->size += size;
}
static UmiLanguageRuntimeServer *Server(Fixture *fixture, const char *root)
{
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "fixture");
    strcpy(profile.executable, "fixture");
    profile.enabled = 1;
    UmiLanguageRuntimeTransport transport = {fixture, Write, Read, Stop, Running, Destroy};
    UmiLanguageRuntimeServer *server = NULL;
    fixture->running = 1;
    CHECK(umi_language_runtime_server_create_with_transport("fixture-server", &profile, root, &transport,
                                                            &server) == UMI_STATUS_OK);
    CHECK(transport.instance == NULL);
    return server;
}

#include "umicom/language_runtime/call_query.h"
static void Reply(Fixture *fixture, unsigned id, const char *result, int error)
{
    char *json = malloc(strlen(result) + 128U);
    CHECK(json != NULL);
    if (error)
        (void)sprintf(
            json, "{\"jsonrpc\":\"2.0\",\"id\":%u,\"error\":{\"code\":-1,\"message\":\"unavailable\"}}", id);
    else
        (void)sprintf(json, "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":%s}", id, result);
    Push(fixture, json);
    free(json);
}
static void Item(char *out, size_t capacity, const char *uri, unsigned end)
{
    (void)snprintf(
        out, capacity,
        "{\"name\":\"function\",\"kind\":12,\"uri\":\"%s\","
        "\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":%u}},"
        "\"selectionRange\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":1}},"
        "\"data\":{\"session\":\"original\"},\"extension\":[true,3]}",
        uri, end);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"incoming",
                           "outgoing",
                           "object",
                           "missing",
                           "disabled",
                           "encoding",
                           "sync",
                           "empty",
                           "null",
                           "no-calls",
                           "null-calls",
                           "no-sites",
                           "multiple",
                           "roots-limit",
                           "roots-over",
                           "fragmented",
                           "notification",
                           "wrong-id",
                           "prepare-error",
                           "call-error",
                           "second-error",
                           "malformed-root",
                           "malformed-call",
                           "root-outside",
                           "site-outside",
                           "related-outside",
                           "external-root",
                           "unicode",
                           "surrogate-site",
                           "close-error",
                           "shutdown-error",
                           "cancel-before",
                           "cancel-followup",
                           "timeout-followup",
                           "read-followup",
                           "write-followup",
                           "invalid-direction",
                           "invalid-source",
                           "invalid-caret",
                           "state",
                           "native-invalid",
                           "invalid-output"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    CHECK(fixture != NULL);
    UmiLanguageCallRequest request = {
        {"file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 1U, 2000U},
        UMI_LANGUAGE_CALL_INCOMING};
    if (strcmp(mode, "outgoing") == 0 || strcmp(mode, "site-outside") == 0 ||
        strcmp(mode, "external-root") == 0 || strcmp(mode, "surrogate-site") == 0)
        request.direction = UMI_LANGUAGE_CALL_OUTGOING;
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, initialize_failure = 0;
    if (strcmp(mode, "unicode") == 0 || strcmp(mode, "surrogate-site") == 0)
    {
        request.source.source = "a\xf0\x9f\x98\x80"
                                "b";
        request.source.source_bytes = 6U;
        request.source.cursor_offset = 5U;
    }
    char caps[512];
    (void)snprintf(caps, sizeof(caps),
                   "{\"capabilities\":{\"callHierarchyProvider\":%s,\"textDocumentSync\":%s,"
                   "\"positionEncoding\":\"%s\"}}",
                   strcmp(mode, "object") == 0     ? "{}"
                   : strcmp(mode, "disabled") == 0 ? "false"
                                                   : "true",
                   strcmp(mode, "sync") == 0 ? "{\"openClose\":false}" : "2",
                   strcmp(mode, "encoding") == 0 ? "utf-8" : "utf-16");
    if (strcmp(mode, "missing") == 0)
        strcpy(caps, "{\"capabilities\":{\"definitionProvider\":true,\"textDocumentSync\":2}}");
    if (strcmp(mode, "missing") == 0 || strcmp(mode, "disabled") == 0 || strcmp(mode, "sync") == 0 ||
        strcmp(mode, "encoding") == 0)
    {
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    Reply(fixture, 1U, caps, 0);
    size_t roots = strcmp(mode, "multiple") == 0 || strcmp(mode, "second-error") == 0 ? 2U
                   : strcmp(mode, "roots-limit") == 0                                 ? 16U
                   : strcmp(mode, "roots-over") == 0                                  ? 17U
                                                                                      : 1U;
    if (strcmp(mode, "empty") == 0 || strcmp(mode, "null") == 0)
        roots = 0U;
    char item[1024], related[1024], prepared[8192], calls[4096];
    const char *root_uri =
        strcmp(mode, "external-root") == 0 ? "file:///workspace/definition.c" : request.source.document_uri;
    Item(item, sizeof(item), root_uri, strcmp(mode, "root-outside") == 0 ? 9U : 3U);
    Item(related, sizeof(related),
         strcmp(mode, "related-outside") == 0 ? request.source.document_uri : "file:///workspace/related.c",
         strcmp(mode, "related-outside") == 0 ? 9U : 3U);
    strcpy(prepared, "[");
    for (size_t i = 0U; i < roots; ++i)
    {
        if (i != 0U)
            strcat(prepared, ",");
        strcat(prepared, item);
    }
    strcat(prepared, "]");
    if (strcmp(mode, "null") == 0)
        strcpy(prepared, "null");
    if (strcmp(mode, "malformed-root") == 0)
        strcpy(prepared, "[{}]");
    if (strcmp(mode, "notification") == 0)
        Push(fixture,
             "{\"jsonrpc\":\"2.0\",\"method\":\"window/logMessage\",\"params\":{\"message\":\"indexing\"}}");
    if (strcmp(mode, "wrong-id") == 0)
        Reply(fixture, 97U, "null", 0);
    Reply(fixture, 2U, prepared, strcmp(mode, "prepare-error") == 0);
    unsigned site_end = strcmp(mode, "site-outside") == 0     ? 9U
                        : strcmp(mode, "surrogate-site") == 0 ? 2U
                                                              : 1U;
    (void)snprintf(calls, sizeof(calls),
                   "[{\"%s\":%s,\"fromRanges\":[{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                   "\"character\":%u}}]}]",
                   request.direction == UMI_LANGUAGE_CALL_INCOMING ? "from" : "to", related, site_end);
    if (strcmp(mode, "no-sites") == 0)
        (void)snprintf(calls, sizeof(calls), "[{\"from\":%s,\"fromRanges\":[]}]", related);
    if (strcmp(mode, "no-calls") == 0)
        strcpy(calls, "[]");
    if (strcmp(mode, "null-calls") == 0)
        strcpy(calls, "null");
    if (strcmp(mode, "malformed-call") == 0)
        strcpy(calls, "[{}]");
    for (size_t i = 0U; i < roots; ++i)
        Reply(fixture, (unsigned)i + 3U, calls,
              strcmp(mode, "call-error") == 0 || (strcmp(mode, "second-error") == 0 && i == 1U));
    Reply(fixture, (unsigned)roots + 3U, "null", strcmp(mode, "shutdown-error") == 0);
    if (strcmp(mode, "roots-over") == 0)
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(mode, "prepare-error") == 0 || strcmp(mode, "call-error") == 0 ||
        strcmp(mode, "second-error") == 0 || strcmp(mode, "shutdown-error") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(mode, "malformed-root") == 0 || strcmp(mode, "malformed-call") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(mode, "root-outside") == 0 || strcmp(mode, "related-outside") == 0 ||
        strcmp(mode, "site-outside") == 0 || strcmp(mode, "surrogate-site") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "close-error") == 0)
    {
        fixture->failClose = 1;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "fragmented") == 0)
        fixture->fragment = 3U;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel-before") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
        untouched = 1;
    }
    if (strcmp(mode, "cancel-followup") == 0)
    {
        fixture->follow_fault = 1;
        fixture->cancel = cancel;
        expected = UMI_STATUS_CANCELLED;
    }
    if (strcmp(mode, "timeout-followup") == 0)
    {
        fixture->follow_fault = 2;
        request.source.timeout_ms = 5U;
        expected = UMI_STATUS_TIMEOUT;
    }
    if (strcmp(mode, "read-followup") == 0)
    {
        fixture->follow_fault = 3;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "write-followup") == 0)
    {
        fixture->follow_fault = 4;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "invalid-direction") == 0)
    {
        request.direction = (UmiLanguageCallDirection)0;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-caret") == 0)
    {
        request.source.cursor_offset = 9U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-source") == 0)
    {
        request.source.source = "a\xc0\x80";
        expected = UMI_STATUS_PARSE_ERROR;
        untouched = 1;
    }
    UmiLanguageRuntimeServer *server = Server(fixture, request.source.root_uri);
    if (strcmp(mode, "state") == 0)
    {
        CHECK(umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING) ==
              UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
        untouched = 1;
    }
    UmiLanguageCompletionQueryReport report;
    UmiLanguageCallResult *result = NULL;
    UmiStatus status;
    if (strcmp(mode, "native-invalid") == 0)
    {
        UmiLanguageServerProfile profile = {0};
        strcpy(profile.id, "invalid");
        profile.enabled = 1;
        status = UmiLanguageCallQueryNative(&profile, NULL, &request, cancel, &report, &result);
        CHECK(status != UMI_STATUS_OK && !report.started && result == NULL);
        untouched = 1;
    }
    else if (strcmp(mode, "invalid-output") == 0)
    {
        status = UmiLanguageCallQueryOnServer(server, &request, cancel, &report, NULL);
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
        untouched = 1;
    }
    else
    {
        status = UmiLanguageCallQueryOnServer(server, &request, cancel, &report, &result);
        CHECK(status == expected);
        if (status == UMI_STATUS_OK)
        {
            size_t relations = strcmp(mode, "no-calls") == 0 || strcmp(mode, "null-calls") == 0 ? 0U : roots;
            CHECK(result != NULL && report.choices == relations &&
                  UmiLanguageCallResultCount(result) == relations &&
                  UmiLanguageCallResultRootCount(result) == roots);
            CHECK(report.initialized && report.document_opened && report.close_status == UMI_STATUS_OK &&
                  report.shutdown_status == UMI_STATUS_OK);
            CHECK(strstr(fixture->written, "\"callHierarchy\":{\"dynamicRegistration\":false}") != NULL);
            CHECK(strstr(fixture->written, "textDocument/prepareCallHierarchy") != NULL);
            if (roots != 0U)
            {
                char complete[1200];
                (void)snprintf(complete, sizeof(complete), "\"params\":{\"item\":%s}", item);
                CHECK(strstr(fixture->written, complete) != NULL);
                CHECK(strstr(fixture->written, request.direction == UMI_LANGUAGE_CALL_INCOMING
                                                   ? "callHierarchy/incomingCalls"
                                                   : "callHierarchy/outgoingCalls") != NULL);
                UmiLanguageCallItem prepared_item;
                CHECK(UmiLanguageCallResultRootAt(result, 0U, &prepared_item) == UMI_STATUS_OK);
                CHECK(strcmp(prepared_item.location.uri, root_uri) == 0);
            }
            for (size_t i = 0U; i < relations; ++i)
            {
                UmiLanguageCallRelation relation;
                CHECK(UmiLanguageCallResultAt(result, i, &relation) == UMI_STATUS_OK &&
                      relation.root_index == i);
                UmiLanguageSourceLocation location;
                CHECK(UmiLanguageCallResultLocation(result, i, 0U, &location) == UMI_STATUS_OK);
                CHECK(strcmp(location.uri, "file:///workspace/related.c") == 0);
                if (relation.call_sites != 0U)
                {
                    CHECK(UmiLanguageCallResultLocation(result, i, 1U, &location) == UMI_STATUS_OK);
                    CHECK(strcmp(location.uri, request.direction == UMI_LANGUAGE_CALL_INCOMING
                                                   ? "file:///workspace/related.c"
                                                   : root_uri) == 0);
                }
            }
            UmiLanguageCallRelation sentinel = {.root_index = 99U};
            CHECK(UmiLanguageCallResultAt(result, SIZE_MAX, &sentinel) == UMI_STATUS_NOT_FOUND &&
                  sentinel.root_index == 99U);
        }
        else
            CHECK(result == NULL);
    }
    if (untouched)
        CHECK(fixture->stops == 0U && fixture->writeSize == 0U && !report.started);
    else
        CHECK(fixture->stops == 1U && report.started && report.initialized == !initialize_failure);
    if (strcmp(mode, "roots-over") == 0 || strcmp(mode, "root-outside") == 0)
        CHECK(strstr(fixture->written, "callHierarchy/incomingCalls") == NULL);
    if (strcmp(mode, "second-error") == 0)
        CHECK(report.query_status == UMI_STATUS_UNAVAILABLE);
    UmiLanguageCallResultDestroy(result);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture->destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    free(fixture);
    return 0;
}
