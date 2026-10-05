/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_completion_query.c
 * PURPOSE: Check isolated completion protocol, draft publication, cancellation and cleanup.
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
    if (fixture->cancel != NULL)
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

#include "umicom/language_runtime/completion_query.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"valid",
                           "fragmented",
                           "notification",
                           "wrong-id",
                           "server-request",
                           "encoding",
                           "sync-object",
                           "sync-disabled",
                           "sync-missing",
                           "completion-missing",
                           "duplicate",
                           "init-error",
                           "completion-error",
                           "shutdown-error",
                           "close-error",
                           "write-error",
                           "read-error",
                           "timeout",
                           "cancel-before",
                           "cancel-during",
                           "unicode",
                           "empty",
                           "no-nul-terminator",
                           "invalid-source",
                           "nul",
                           "cursor",
                           "byte-limit",
                           "escaped-limit",
                           "root",
                           "state",
                           "profile-disabled",
                           "native-invalid",
                           "arguments"};
    int found = 0;
    for (size_t i = 0; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    UmiLanguageRuntimeServer *server = Server(&fixture, "file:///workspace");
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    UmiLanguageCompletionRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "ab", 2U, 1U, 1000U};
    UmiLanguageCompletionQueryReport report;
    UmiLanguageCompletionCatalogue *catalogue = NULL;
    UmiStatus expected = UMI_STATUS_OK;
    int preflight = 0, initialization_failure = 0;
    const char *capabilities = "{\"textDocumentSync\":2,\"completionProvider\":{}}";
    if (strcmp(mode, "encoding") == 0)
    {
        capabilities = "{\"textDocumentSync\":2,\"completionProvider\":{},\"positionEncoding\":\"utf-8\"}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialization_failure = 1;
    }
    if (strcmp(mode, "sync-object") == 0)
        capabilities = "{\"textDocumentSync\":{\"openClose\":true,\"change\":2},\"completionProvider\":{}}";
    if (strcmp(mode, "sync-disabled") == 0)
    {
        capabilities = "{\"textDocumentSync\":{\"openClose\":false},\"completionProvider\":{}}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialization_failure = 1;
    }
    if (strcmp(mode, "sync-missing") == 0)
    {
        capabilities = "{\"completionProvider\":{}}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialization_failure = 1;
    }
    if (strcmp(mode, "completion-missing") == 0)
    {
        capabilities = "{\"textDocumentSync\":2}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialization_failure = 1;
    }
    if (strcmp(mode, "duplicate") == 0)
    {
        capabilities = "{\"textDocumentSync\":2,\"completionProvider\":{},\"completionProvider\":{}}";
        expected = UMI_STATUS_ALREADY_EXISTS;
        initialization_failure = 1;
    }
    if (strcmp(mode, "notification") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"method\":\"window/"
                       "logMessage\",\"params\":{\"type\":3,\"message\":\"working\"}}");
    if (strcmp(mode, "wrong-id") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":99,\"result\":null}");
    if (strcmp(mode, "server-request") == 0)
    {
        Push(&fixture,
             "{\"jsonrpc\":\"2.0\",\"id\":99,\"method\":\"window/showMessageRequest\",\"params\":{}}");
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialization_failure = 1;
    }
    char reply[1024];
    if (strcmp(mode, "init-error") == 0)
    {
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":1,\"error\":{\"code\":-32603,\"message\":\"no\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
        initialization_failure = 1;
    }
    else
    {
        (void)snprintf(reply, sizeof(reply),
                       "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":%s}}", capabilities);
        Push(&fixture, reply);
    }
    if (strcmp(mode, "completion-error") == 0)
    {
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":2,\"error\":{\"code\":-32603,\"message\":\"no\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":[{\"label\":\"puts\"}]}");
    if (strcmp(mode, "shutdown-error") == 0)
    {
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":3,\"error\":{\"code\":-32603,\"message\":\"no\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":3,\"result\":null}");
    if (strcmp(mode, "fragmented") == 0)
        fixture.fragment = 3U;
    if (strcmp(mode, "close-error") == 0)
    {
        fixture.failClose = 1;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "write-error") == 0)
    {
        fixture.failWrite = 1;
        expected = UMI_STATUS_IO_ERROR;
        initialization_failure = 1;
    }
    if (strcmp(mode, "read-error") == 0)
    {
        fixture.failRead = 1;
        expected = UMI_STATUS_IO_ERROR;
        initialization_failure = 1;
    }
    if (strcmp(mode, "timeout") == 0)
    {
        fixture.size = 0U;
        request.timeout_ms = 5U;
        expected = UMI_STATUS_TIMEOUT;
        initialization_failure = 1;
    }
    if (strcmp(mode, "cancel-before") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
        preflight = 1;
    }
    if (strcmp(mode, "cancel-during") == 0)
    {
        fixture.cancel = cancel;
        expected = UMI_STATUS_CANCELLED;
        initialization_failure = 1;
    }
    char raw[2] = {'x', 'y'};
    if (strcmp(mode, "no-nul-terminator") == 0)
        request.source = raw;
    if (strcmp(mode, "unicode") == 0)
    {
        request.source = "\xf0\x9f\x98\x80";
        request.source_bytes = 4U;
        request.cursor_offset = 4U;
    }
    if (strcmp(mode, "empty") == 0)
    {
        request.source = "";
        request.source_bytes = 0U;
        request.cursor_offset = 0U;
    }
    if (strcmp(mode, "invalid-source") == 0)
    {
        request.source = "a\xff";
        expected = UMI_STATUS_PARSE_ERROR;
        preflight = 1;
    }
    if (strcmp(mode, "nul") == 0)
    {
        request.source = "a\0";
        expected = UMI_STATUS_INVALID_ARGUMENT;
        preflight = 1;
    }
    if (strcmp(mode, "cursor") == 0)
    {
        request.source = "\xf0\x9f\x98\x80";
        request.source_bytes = 4U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        preflight = 1;
    }
    if (strcmp(mode, "byte-limit") == 0)
    {
        request.source_bytes = 65536U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        preflight = 1;
    }
    char *large = NULL;
    if (strcmp(mode, "escaped-limit") == 0)
    {
        large = malloc(16384U);
        CHECK(large != NULL);
        memset(large, 1, 16384U);
        request.source = large;
        request.source_bytes = 16384U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        preflight = 1;
    }
    if (strcmp(mode, "root") == 0)
    {
        request.root_uri = "file:///other";
        expected = UMI_STATUS_INVALID_STATE;
        preflight = 1;
    }
    if (strcmp(mode, "state") == 0)
    {
        CHECK(umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING) ==
              UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
        preflight = 1;
    }
    if (strcmp(mode, "profile-disabled") == 0 || strcmp(mode, "native-invalid") == 0)
    {
        UmiLanguageServerProfile profile = {0};
        strcpy(profile.id, "disabled");
        strcpy(profile.executable, "unused");
        if (strcmp(mode, "native-invalid") == 0)
            request.source = NULL;
        CHECK(UmiLanguageCompletionQueryNative(&profile, NULL, &request, cancel, &report, &catalogue) ==
              (strcmp(mode, "native-invalid") == 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_UNAVAILABLE));
        CHECK(!report.started && catalogue == NULL);
        goto cleanup;
    }
    CHECK(UmiLanguageCompletionQueryOnServer(server, &request, cancel, &report, &catalogue) == expected);
    CHECK(report.started == !preflight && report.initialized == (!preflight && !initialization_failure));
    CHECK(fixture.stops == (preflight ? 0U : 1U));
    if (preflight)
        CHECK(fixture.writeSize == 0U);
    else if (!fixture.failWrite)
    {
        CHECK(strstr(fixture.written, "\"snippetSupport\":false") != NULL);
        CHECK(strstr(fixture.written, "\"positionEncodings\":[\"utf-16\"]") != NULL);
        if (!initialization_failure)
        {
            CHECK(strstr(fixture.written, "textDocument/didOpen") != NULL);
            CHECK(strstr(fixture.written, "textDocument/completion") != NULL);
            CHECK(strstr(fixture.written, "textDocument/didClose") != NULL || fixture.failClose);
            CHECK(strstr(fixture.written, "\"shutdown\"") != NULL);
            if (strcmp(mode, "unicode") == 0)
                CHECK(strstr(fixture.written, "\"character\":2") != NULL);
            if (strcmp(mode, "no-nul-terminator") == 0)
                CHECK(strstr(fixture.written, "\"text\":\"xy\"") != NULL);
        }
    }
    if (expected == UMI_STATUS_OK)
    {
        CHECK(catalogue != NULL && report.choices == 1U && report.query_status == UMI_STATUS_OK);
        UmiLanguageCompletionChoice choice;
        CHECK(UmiLanguageCompletionCatalogueAt(catalogue, 0U, &choice) == UMI_STATUS_OK &&
              strcmp(choice.label, "puts") == 0);
    }
    else
        CHECK(catalogue == NULL);
    if (strcmp(mode, "close-error") == 0)
        CHECK(report.query_status == UMI_STATUS_OK && report.close_status == UMI_STATUS_IO_ERROR &&
              report.shutdown_status == UMI_STATUS_OK);
    if (strcmp(mode, "shutdown-error") == 0)
        CHECK(report.query_status == UMI_STATUS_OK && report.shutdown_status == UMI_STATUS_UNAVAILABLE);
    if (strcmp(mode, "arguments") == 0)
    {
        UmiLanguageCompletionCatalogue *invalid = NULL;
        CHECK(UmiLanguageCompletionQueryOnServer(NULL, &request, NULL, &report, &invalid) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLanguageCompletionQueryOnServer(NULL, &request, NULL, NULL, &invalid) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
cleanup:
    free(large);
    UmiLanguageCompletionCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    umi_cancellation_token_destroy(cancel);
    CHECK(fixture.destroys == 1U);
    return 0;
}
