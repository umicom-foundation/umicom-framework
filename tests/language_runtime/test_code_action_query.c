/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_code_action_query.c
 * PURPOSE: Check code-action negotiation, exact positions and cleanup before publication.
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

#include "umicom/language_runtime/code_action_query.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"valid",
                           "object",
                           "missing",
                           "disabled",
                           "encoding",
                           "sync",
                           "fragmented",
                           "notification",
                           "error",
                           "invalid-content",
                           "range-outside",
                           "range-surrogate",
                           "shutdown-error",
                           "close-error",
                           "timeout",
                           "cancel-before",
                           "cancel-during",
                           "invalid-source",
                           "invalid-caret",
                           "state",
                           "empty",
                           "unicode",
                           "native-invalid",
                           "read-error",
                           "write-error",
                           "escaped-limit",
                           "choices",
                           "disabled-action",
                           "wrong-id",
                           "command",
                           "edit-and-command",
                           "deferred",
                           "annotations",
                           "range-reversed",
                           "range-multiline",
                           "empty-range"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    UmiLanguageCodeActionRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 1U, 2U, 500U};
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, initialize_failure = 0;
    const char *caps = "{\"codeActionProvider\":true,\"textDocumentSync\":2}";
    if (strcmp(mode, "object") == 0)
        caps = "{\"codeActionProvider\":{},\"textDocumentSync\":{\"openClose\":true}}";
    if (strcmp(mode, "missing") == 0)
    {
        caps = "{\"textDocumentSync\":2}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "disabled") == 0)
    {
        caps = "{\"codeActionProvider\":false,\"textDocumentSync\":2}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "encoding") == 0)
    {
        caps = "{\"codeActionProvider\":true,\"textDocumentSync\":2,\"positionEncoding\":\"utf-8\"}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "sync") == 0)
    {
        caps = "{\"codeActionProvider\":true,\"textDocumentSync\":{\"openClose\":false}}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "notification") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"method\":\"window/"
                       "logMessage\",\"params\":{\"type\":3,\"message\":\"working\"}}");
    char response[2048];
    (void)snprintf(response, sizeof(response),
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":%s}}", caps);
    Push(&fixture, response);

    const char *contents = "[{\"title\":\"Extract "
                           "variable\",\"kind\":\"refactor.extract\",\"isPreferred\":true,\"edit\":{"
                           "\"changes\":{\"file:///workspace/"
                           "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                           "\"character\":3}},\"newText\":\"total\"}]}}}]";
    size_t count = 1U;
    if (strcmp(mode, "empty") == 0)
    {
        contents = "null";
        count = 0U;
    }
    if (strcmp(mode, "choices") == 0)
    {
        contents = "[{\"title\":\"First\",\"edit\":{}},{\"title\":\"Second\",\"edit\":{}}]";
        count = 2U;
    }
    if (strcmp(mode, "disabled-action") == 0)
        contents =
            "[{\"title\":\"Unavailable\",\"disabled\":{\"reason\":\"Select an expression\"},\"edit\":{}}]";
    if (strcmp(mode, "command") == 0)
        contents = "[{\"title\":\"Server command\",\"command\":\"server.run\",\"arguments\":[1]}]";
    if (strcmp(mode, "edit-and-command") == 0)
        contents = "[{\"title\":\"Edit then "
                   "command\",\"edit\":{},\"command\":{\"title\":\"Finish\",\"command\":\"server.run\"}}]";
    if (strcmp(mode, "deferred") == 0)
        contents = "[{\"title\":\"Resolve later\",\"data\":{\"id\":7}}]";
    if (strcmp(mode, "annotations") == 0)
        contents = "[{\"title\":\"Annotated\",\"edit\":{\"documentChanges\":[{\"textDocument\":{\"uri\":"
                   "\"file:///workspace/"
                   "main.c\",\"version\":1},\"edits\":[]}],\"changeAnnotations\":{\"note\":{\"label\":\"Read "
                   "this reason\",\"needsConfirmation\":true}}}}]";
    if (strcmp(mode, "invalid-content") == 0)
    {
        contents = "[{\"title\":7}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "unicode") == 0)
    {
        request.source = "a\xf0\x9f\x8c\x8d"
                         "c";
        request.source_bytes = 6U;
        request.range_start = 1U;
        request.range_end = 5U;
    }
    if (strcmp(mode, "range-outside") == 0)
    {
        request.range_end = 4U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "range-reversed") == 0)
    {
        request.range_end = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "range-surrogate") == 0)
    {
        request.source = "a\xf0\x9f\x8c\x8d"
                         "c";
        request.source_bytes = 6U;
        request.range_end = 2U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "range-multiline") == 0)
    {
        request.source = "a\r\nbc";
        request.source_bytes = 5U;
        request.range_start = 0U;
        request.range_end = 4U;
    }
    if (strcmp(mode, "empty-range") == 0)
        request.range_end = request.range_start;
    if (strcmp(mode, "wrong-id") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":99,\"result\":{\"ignored\":true}}");
    if (strcmp(mode, "error") == 0)
    {
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":2,\"error\":{\"code\":-1,\"message\":\"refused\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else
    {
        (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":%s}", contents);
        Push(&fixture, response);
    }
    if (strcmp(mode, "shutdown-error") == 0)
    {
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":3,\"error\":{\"code\":-1,\"message\":\"shutdown\"}}");
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
    if (strcmp(mode, "read-error") == 0)
    {
        fixture.failRead = 1;
        expected = UMI_STATUS_IO_ERROR;
        initialize_failure = 1;
    }
    if (strcmp(mode, "write-error") == 0)
    {
        fixture.failWrite = 1;
        expected = UMI_STATUS_IO_ERROR;
        initialize_failure = 1;
    }
    if (strcmp(mode, "timeout") == 0)
    {
        fixture.size = 0U;
        request.timeout_ms = 5U;
        expected = UMI_STATUS_TIMEOUT;
        initialize_failure = 1;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel-before") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
        untouched = 1;
    }
    if (strcmp(mode, "cancel-during") == 0)
    {
        fixture.cancel = cancel;
        expected = UMI_STATUS_CANCELLED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "invalid-source") == 0)
    {
        request.source = "a\xc0\x80";
        expected = UMI_STATUS_PARSE_ERROR;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-caret") == 0)
    {
        request.range_start = 4U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    char *large = NULL;
    if (strcmp(mode, "escaped-limit") == 0)
    {
        /* Expansion of a valid draft is checked before the transport is used. */
        large = malloc(40001U);
        CHECK(large != NULL);
        memset(large, '"', 40000U);
        large[40000] = '\0';
        request.source = large;
        request.source_bytes = 40000U;
        request.range_start = 0U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        untouched = 1;
    }
    UmiLanguageRuntimeServer *server = Server(&fixture, request.root_uri);
    if (strcmp(mode, "state") == 0)
    {
        CHECK(umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING) ==
              UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
        untouched = 1;
    }
    UmiLanguageCodeActionReport report;
    UmiLanguageCodeActionCatalogue *document = NULL;
    UmiStatus status;
    if (strcmp(mode, "native-invalid") == 0)
    {
        UmiLanguageServerProfile profile = {0};
        strcpy(profile.id, "invalid");
        profile.enabled = 1;
        status = UmiLanguageCodeActionQueryNative(&profile, NULL, &request, cancel, &report, &document);
        CHECK(status != UMI_STATUS_OK && !report.started && document == NULL);
        untouched = 1;
    }
    else
    {
        status = UmiLanguageCodeActionQueryOnServer(server, &request, cancel, &report, &document);
        CHECK(status == expected);
        if (status == UMI_STATUS_OK)
        {

            CHECK(UmiLanguageCodeActionCatalogueCount(document) == count && report.actions == count);
            CHECK(strstr(fixture.written, "\"codeActionLiteralSupport\":") != NULL);
            CHECK(strstr(fixture.written, "\"disabledSupport\":true") != NULL);
            CHECK(strstr(fixture.written, "\"dataSupport\":false") != NULL);
            CHECK(strstr(fixture.written, "\"diagnostics\":[],\"triggerKind\":1") != NULL);
            CHECK(strstr(fixture.written, "\"method\":\"textDocument/codeAction\"") != NULL);
            CHECK(strstr(fixture.written, "workspace/executeCommand") == NULL &&
                  strstr(fixture.written, "codeAction/resolve") == NULL &&
                  strstr(fixture.written, "textDocument/completion") == NULL);
            if (strcmp(mode, "unicode") == 0)
                CHECK(strstr(fixture.written, "\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{"
                                              "\"line\":0,\"character\":3}}") != NULL);
            else if (strcmp(mode, "range-multiline") == 0)
                CHECK(strstr(fixture.written, "\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
                                              "\"line\":1,\"character\":1}}") != NULL);
            else if (strcmp(mode, "empty-range") == 0)
                CHECK(strstr(fixture.written, "\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{"
                                              "\"line\":0,\"character\":1}}") != NULL);
            else
                CHECK(strstr(fixture.written, "\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{"
                                              "\"line\":0,\"character\":2}}") != NULL);
            if (count != 0U)
            {
                UmiLanguageCodeAction action;
                CHECK(UmiLanguageCodeActionCatalogueAt(document, 0U, &action) == UMI_STATUS_OK);
                if (strcmp(mode, "disabled-action") == 0)
                    CHECK(action.disabled && strcmp(action.disabled_reason, "Select an expression") == 0);
                if (strcmp(mode, "command") == 0 || strcmp(mode, "edit-and-command") == 0)
                    CHECK(action.has_command && strcmp(action.command, "server.run") == 0);
                if (strcmp(mode, "deferred") == 0)
                    CHECK(action.has_data && !action.has_edit);
            }
            CHECK(report.initialized && report.document_opened && report.close_status == UMI_STATUS_OK &&
                  report.shutdown_status == UMI_STATUS_OK);
        }
        else
            CHECK(document == NULL);
    }
    if (untouched)
        CHECK(fixture.stops == 0U && fixture.writeSize == 0U && !report.started);
    else
    {
        CHECK(fixture.stops == 1U && !umi_language_runtime_server_is_running(server));
        CHECK(report.started && report.initialized == !initialize_failure);
    }
    if (strcmp(mode, "shutdown-error") == 0 || strcmp(mode, "close-error") == 0)
        CHECK(report.query_status == UMI_STATUS_OK);
    UmiLanguageCodeActionCatalogueDestroy(document);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    free(large);
    return 0;
}
