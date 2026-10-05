/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_folding_query.c
 * PURPOSE: Check folding negotiation, document-only parameters and cleanup before publishing complete regions.
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

#include "umicom/language_runtime/navigation_query.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {
        "valid",        "object",     "missing",       "disabled",      "wrong-provider", "encoding",
        "sync",         "fragmented", "notification",  "error",         "invalid-result", "shutdown-error",
        "close-error",  "timeout",    "cancel-before", "cancel-during", "invalid-source", "invalid-caret",
        "invalid-kind", "state",      "empty",         "empty-array",   "unicode",        "crlf",
        "outside",      "wrong-id",   "read-error",    "write-error"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    const char *source = "head\nbody\nend\ntail";
    if (strcmp(mode, "unicode") == 0)
        source = "h\n\xf0\x9f\x8c\x8d\nend\ntail";
    if (strcmp(mode, "crlf") == 0)
        source = "head\r\nbody\r\nend\r\ntail";
    UmiLanguageNavigationRequest request = {"file:///workspace",
                                            "file:///workspace/main.c",
                                            "c",
                                            source,
                                            strlen(source),
                                            1U,
                                            500U,
                                            UMI_LANGUAGE_NAVIGATION_FOLDING_RANGES,
                                            0};
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, initialize_failure = 0;
    char caps[1024], response[2048];
    (void)snprintf(caps, sizeof(caps),
                   "{\"foldingRangeProvider\":%s,\"textDocumentSync\":%s,\"positionEncoding\":\"%s\"}",
                   strcmp(mode, "object") == 0     ? "{}"
                   : strcmp(mode, "disabled") == 0 ? "false"
                                                   : "true",
                   strcmp(mode, "sync") == 0 ? "{\"openClose\":false}" : "2",
                   strcmp(mode, "encoding") == 0 ? "utf-8" : "utf-16");
    if (strcmp(mode, "missing") == 0)
        strcpy(caps, "{\"textDocumentSync\":2}");
    if (strcmp(mode, "wrong-provider") == 0)
        strcpy(caps, "{\"definitionProvider\":true,\"textDocumentSync\":2}");
    if (strcmp(mode, "missing") == 0 || strcmp(mode, "disabled") == 0 ||
        strcmp(mode, "wrong-provider") == 0 || strcmp(mode, "sync") == 0 || strcmp(mode, "encoding") == 0)
    {
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    (void)snprintf(response, sizeof(response),
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":%s}}", caps);
    Push(&fixture, response);
    if (strcmp(mode, "notification") == 0)
        Push(&fixture,
             "{\"jsonrpc\":\"2.0\",\"method\":\"window/logMessage\",\"params\":{\"message\":\"working\"}}");
    if (strcmp(mode, "wrong-id") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":99,\"result\":null}");
    const char *ranges = "[{\"startLine\":0,\"endLine\":2}]";
    size_t count = 1U;
    if (strcmp(mode, "empty") == 0 || strcmp(mode, "empty-array") == 0)
    {
        ranges = strcmp(mode, "empty") == 0 ? "null" : "[]";
        count = 0U;
    }
    if (strcmp(mode, "invalid-result") == 0)
    {
        ranges = "[{\"startLine\":0,\"endLine\":2},{}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "outside") == 0)
    {
        ranges = "[{\"startLine\":0,\"endLine\":4}]";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "error") == 0)
    {
        strcpy(response,
               "{\"jsonrpc\":\"2.0\",\"id\":2,\"error\":{\"code\":-32603,\"message\":\"refused\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else
        (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":%s}", ranges);
    Push(&fixture, response);
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
    if (strcmp(mode, "timeout") == 0)
    {
        fixture.size = 0U;
        request.timeout_ms = 5U;
        initialize_failure = 1;
        expected = UMI_STATUS_TIMEOUT;
    }
    if (strcmp(mode, "read-error") == 0)
    {
        fixture.failRead = 1;
        initialize_failure = 1;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "write-error") == 0)
    {
        fixture.failWrite = 1;
        initialize_failure = 1;
        expected = UMI_STATUS_IO_ERROR;
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
        request.source_bytes = 3U;
        expected = UMI_STATUS_PARSE_ERROR;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-caret") == 0)
    {
        request.caret = request.source_bytes + 1U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-kind") == 0)
    {
        request.kind = (UmiLanguageNavigationKind)99;
        expected = UMI_STATUS_INVALID_ARGUMENT;
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
    UmiLanguageNavigationReport report;
    UmiLanguageLocationCatalogue *catalogue = NULL;
    CHECK(UmiLanguageNavigationQueryOnServer(server, &request, cancel, &report, &catalogue) == expected);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(catalogue != NULL && report.locations == count &&
              UmiLanguageLocationCatalogueCount(catalogue) == count);
        if (count != 0U)
        {
            UmiLanguageSourceLocation item;
            CHECK(UmiLanguageLocationCatalogueAt(catalogue, 0U, &item) == UMI_STATUS_OK);
            CHECK(item.selection.start.line == 0U && item.selection.end.line == 3U &&
                  item.selection.end.utf16_column == 0U);
        }
        CHECK(strstr(fixture.written, "\"lineFoldingOnly\":true") != NULL &&
              strstr(fixture.written, "\"rangeLimit\":4096") != NULL);
        CHECK(strstr(fixture.written, "\"method\":\"textDocument/foldingRange\"") != NULL);
        CHECK(strstr(fixture.written, "\"position\":") == NULL &&
              strstr(fixture.written, "\"positions\":") == NULL &&
              strstr(fixture.written, "includeDeclaration") == NULL);
        CHECK(report.initialized && report.document_opened && report.close_status == UMI_STATUS_OK &&
              report.shutdown_status == UMI_STATUS_OK);
    }
    else
        CHECK(catalogue == NULL);
    if (untouched)
        CHECK(fixture.stops == 0U && fixture.writeSize == 0U && !report.started);
    else
        CHECK(fixture.stops == 1U && !umi_language_runtime_server_is_running(server) && report.started &&
              report.initialized == !initialize_failure);
    if (strcmp(mode, "shutdown-error") == 0 || strcmp(mode, "close-error") == 0)
        CHECK(report.query_status == UMI_STATUS_OK);
    UmiLanguageLocationCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
