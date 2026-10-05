/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_range_formatting_query.c
 * PURPOSE: Check exact formatting ranges, independent preview carets, complete edits and cleanup ownership.
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

#include "umicom/language_runtime/formatting_query.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"valid",
                           "options",
                           "object",
                           "missing",
                           "disabled",
                           "wrong-provider",
                           "encoding",
                           "sync",
                           "fragmented",
                           "notification",
                           "error",
                           "invalid-edits",
                           "overlap",
                           "shutdown-error",
                           "close-error",
                           "timeout",
                           "cancel-before",
                           "cancel-during",
                           "invalid-source",
                           "invalid-options",
                           "state",
                           "empty",
                           "unicode",
                           "native-invalid",
                           "read-error",
                           "write-error",
                           "escaped-limit",
                           "range-reversed",
                           "range-outside",
                           "range-start-scalar",
                           "range-end-scalar",
                           "range-empty",
                           "range-surrounding",
                           "caret-independent"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    UmiLanguageFormattingRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 1U, 500U, 4U, 1};
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, initialize_failure = 0;
    const char *capabilities = "{\"documentRangeFormattingProvider\":true,\"textDocumentSync\":2}";
    if (strcmp(mode, "object") == 0)
        capabilities = "{\"documentRangeFormattingProvider\":{},\"textDocumentSync\":{\"openClose\":true}}";
    if (strcmp(mode, "missing") == 0)
    {
        capabilities = "{\"textDocumentSync\":2}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "disabled") == 0)
    {
        capabilities = "{\"documentRangeFormattingProvider\":false,\"textDocumentSync\":2}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "encoding") == 0)
    {
        capabilities = "{\"documentRangeFormattingProvider\":true,\"textDocumentSync\":2,"
                       "\"positionEncoding\":\"utf-8\"}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "sync") == 0)
    {
        capabilities =
            "{\"documentRangeFormattingProvider\":true,\"textDocumentSync\":{\"openClose\":false}}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "wrong-provider") == 0)
    {
        capabilities = "{\"documentFormattingProvider\":true,\"textDocumentSync\":2}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    char response[2048];
    if (strcmp(mode, "notification") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"method\":\"window/"
                       "logMessage\",\"params\":{\"type\":3,\"message\":\"working\"}}");
    (void)snprintf(response, sizeof(response),
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":%s}}", capabilities);
    Push(&fixture, response);
    const char *edits = "[{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,"
                        "\"character\":2}},\"newText\":\"B\"}]";
    if (strcmp(mode, "empty") == 0)
        edits = "null";
    if (strcmp(mode, "unicode") == 0)
    {
        request.source = "a\xf0\x9f\x8c\x8d"
                         "c";
        request.source_bytes = 6U;
        request.caret = 5U;
        edits = "[{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},"
                "\"newText\":\"B\"}]";
    }
    if (strcmp(mode, "invalid-edits") == 0)
    {
        edits = "{}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "overlap") == 0)
    {
        edits = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}},"
                "\"newText\":\"X\"},{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,"
                "\"character\":3}},\"newText\":\"Y\"}]";
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "error") == 0)
    {
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":2,\"error\":{\"code\":-1,\"message\":\"refused\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else
    {
        (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":%s}", edits);
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
    if (strcmp(mode, "options") == 0)
    {
        request.tab_size = 2U;
        request.insert_spaces = 0;
    }
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
    if (strcmp(mode, "invalid-options") == 0)
    {
        request.tab_size = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    char *large = NULL;
    if (strcmp(mode, "escaped-limit") == 0)
    {
        large = malloc(20001U);
        CHECK(large != NULL);
        memset(large, '\t', 20000U);
        large[20000] = '\0';
        /* Tabs escape to two bytes. Use enough quotes to overflow only once the
         * complete source notification is framed, without launching a child. */
        free(large);
        large = malloc(40001U);
        CHECK(large != NULL);
        memset(large, '"', 40000U);
        large[40000] = '\0';
        request.source = large;
        request.source_bytes = 40000U;
        request.caret = 0U;
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
    UmiLanguageRangeFormattingRequest range = {request, 1U, 2U};
    if (strcmp(mode, "unicode") == 0)
        range.end = 5U;
    if (strcmp(mode, "range-empty") == 0)
        range.end = range.start;
    if (strcmp(mode, "range-surrounding") == 0)
    {
        range.start = 2U;
        range.end = 3U;
    }
    if (strcmp(mode, "caret-independent") == 0)
        range.source.caret = 0U;
    if (strcmp(mode, "range-reversed") == 0)
    {
        range.start = 2U;
        range.end = 1U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "range-outside") == 0)
    {
        range.end = 4U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "range-start-scalar") == 0 || strcmp(mode, "range-end-scalar") == 0)
    {
        range.source.source = "a\xf0\x9f\x98\x80";
        range.source.source_bytes = 5U;
        range.start = strcmp(mode, "range-start-scalar") == 0 ? 2U : 1U;
        range.end = strcmp(mode, "range-end-scalar") == 0 ? 3U : 5U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    UmiLanguageFormattingReport report;
    UmiLanguageTextEditPreview *preview = NULL;
    UmiStatus status;
    if (strcmp(mode, "native-invalid") == 0)
    {
        UmiLanguageServerProfile profile = {0};
        strcpy(profile.id, "invalid");
        profile.enabled = 1;
        status = UmiLanguageRangeFormattingQueryNative(&profile, NULL, &range, cancel, &report, &preview);
        CHECK(status != UMI_STATUS_OK && !report.started && preview == NULL);
        untouched = 1;
    }
    else
    {
        status = UmiLanguageRangeFormattingQueryOnServer(server, &range, cancel, &report, &preview);
        CHECK(status == expected);
        if (status == UMI_STATUS_OK)
        {
            const char *text = NULL;
            size_t size = 0U, caret = 0U;
            CHECK(UmiLanguageTextEditPreviewRead(preview, &text, &size, &caret) == UMI_STATUS_OK);
            CHECK(strcmp(text, strcmp(mode, "empty") == 0 ? "abc" : "aBc") == 0 && size == 3U);
            CHECK(caret == (strcmp(mode, "caret-independent") == 0 ? 0U
                            : strcmp(mode, "empty") == 0           ? 1U
                                                                   : 2U));
            CHECK(strstr(fixture.written, "\"method\":\"textDocument/rangeFormatting\"") != NULL);
            const char *wire_range =
                strcmp(mode, "unicode") == 0 ? "\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{"
                                               "\"line\":0,\"character\":3}}"
                : strcmp(mode, "range-empty") == 0 ? "\"range\":{\"start\":{\"line\":0,\"character\":1},"
                                                     "\"end\":{\"line\":0,\"character\":1}}"
                : strcmp(mode, "range-surrounding") == 0 ? "\"range\":{\"start\":{\"line\":0,\"character\":2}"
                                                           ",\"end\":{\"line\":0,\"character\":3}}"
                                                         : "\"range\":{\"start\":{\"line\":0,\"character\":1}"
                                                           ",\"end\":{\"line\":0,\"character\":2}}";
            CHECK(strstr(fixture.written, wire_range) != NULL);
            CHECK(strstr(fixture.written, "\"method\":\"textDocument/formatting\"") == NULL);
            CHECK(report.edit_count == (strcmp(mode, "empty") == 0 ? 0U : 1U));
            CHECK(strstr(fixture.written, "\"rangeFormatting\":{\"dynamicRegistration\":false}") != NULL);
            CHECK(strstr(fixture.written, "\"completion\"") == NULL &&
                  strstr(fixture.written, "textDocument/completion") == NULL);
            CHECK(strstr(fixture.written, strcmp(mode, "options") == 0
                                              ? "\"tabSize\":2,\"insertSpaces\":false"
                                              : "\"tabSize\":4,\"insertSpaces\":true") != NULL);
            CHECK(report.initialized && report.document_opened && report.close_status == UMI_STATUS_OK &&
                  report.shutdown_status == UMI_STATUS_OK);
        }
        else
            CHECK(preview == NULL);
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
    UmiLanguageTextEditPreviewDestroy(preview);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    free(large);
    return 0;
}
