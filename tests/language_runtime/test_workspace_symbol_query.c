/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_workspace_symbol_query.c
 * PURPOSE: Check explicit project symbol search, bounded query input and complete location ownership.
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

#include "umicom/language_runtime/symbol_query.h"
#define RANGE "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}}"
#define SYMBOL "{\"name\":\"abc\",\"kind\":12,\"range\":" RANGE ",\"selectionRange\":" RANGE "}"
#define FLAT                                                                                                 \
    "{\"name\":\"abc\",\"kind\":12,\"location\":{\"uri\":\"file:///workspace/main.c\",\"range\":" RANGE "}}"

/* A project query must use workspace capability negotiation. The fake transport
 * records the entire connection so a valid result also proves wire ordering. */
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {
        "valid",          "object",       "empty",          "empty-query",    "escaped-query",
        "unicode-query",  "query-null",   "query-invalid",  "query-capacity", "query-escape-capacity",
        "external",       "self-outside", "self-surrogate", "missing",        "disabled",
        "document-only",  "encoding",     "sync",           "fragmented",     "notification",
        "error",          "hierarchy",    "mixed",          "missing-range",  "invalid-result",
        "shutdown-error", "close-error",  "timeout",        "cancel-before",  "cancel-during",
        "invalid-source", "state",        "read-error",     "write-error",    "wrong-id"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    UmiLanguageWorkspaceSymbolRequest request = {
        {"file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 500U}, "abc"};
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, initialize_failure = 0;
    char large_query[4098];
    memset(large_query, 'x', sizeof(large_query) - 1U);
    large_query[sizeof(large_query) - 1U] = '\0';
    if (strcmp(mode, "empty-query") == 0)
        request.query = "";
    if (strcmp(mode, "escaped-query") == 0)
        request.query = "a\"\\\nb";
    if (strcmp(mode, "unicode-query") == 0)
        request.query = "caf\xc3\xa9";
    if (strcmp(mode, "query-null") == 0)
    {
        request.query = NULL;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "query-invalid") == 0)
    {
        request.query = "\xc0\x80";
        expected = UMI_STATUS_PARSE_ERROR;
        untouched = 1;
    }
    if (strcmp(mode, "query-capacity") == 0)
    {
        request.query = large_query;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        untouched = 1;
    }
    if (strcmp(mode, "query-escape-capacity") == 0)
    {
        memset(large_query, '\n', 4096U);
        large_query[4096] = '\0';
        request.query = large_query;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        untouched = 1;
    }
    const char *caps = "{\"workspaceSymbolProvider\":true,\"textDocumentSync\":2}";
    if (strcmp(mode, "object") == 0)
        caps = "{\"workspaceSymbolProvider\":{\"resolveProvider\":true},\"textDocumentSync\":2}";
    if (strcmp(mode, "missing") == 0)
        caps = "{\"textDocumentSync\":2}";
    if (strcmp(mode, "document-only") == 0)
        caps = "{\"documentSymbolProvider\":true,\"textDocumentSync\":2}";
    if (strcmp(mode, "disabled") == 0)
        caps = "{\"workspaceSymbolProvider\":false,\"textDocumentSync\":2}";
    if (strcmp(mode, "encoding") == 0)
        caps = "{\"workspaceSymbolProvider\":true,\"positionEncoding\":\"utf-8\",\"textDocumentSync\":2}";
    if (strcmp(mode, "sync") == 0)
        caps = "{\"workspaceSymbolProvider\":true,\"textDocumentSync\":{\"openClose\":false}}";
    if (strcmp(mode, "missing") == 0 || strcmp(mode, "document-only") == 0 || strcmp(mode, "disabled") == 0 ||
        strcmp(mode, "encoding") == 0 || strcmp(mode, "sync") == 0)
    {
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    char response[4096];
    (void)snprintf(response, sizeof(response),
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":%s}}", caps);
    Push(&fixture, response);
    const char *rows = "[" FLAT "]";
    size_t count = 1U;
    if (strcmp(mode, "empty") == 0)
    {
        rows = "null";
        count = 0U;
    }
    if (strcmp(mode, "external") == 0)
        rows = "[{\"name\":\"external\",\"kind\":12,\"containerName\":\"other "
               "file\",\"location\":{\"uri\":\"file:///workspace/"
               "other.c\",\"range\":{\"start\":{\"line\":8,\"character\":0},\"end\":{\"line\":9,"
               "\"character\":0}}}}]";
    if (strcmp(mode, "self-outside") == 0)
    {
        request.source.source_bytes = 2U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "self-surrogate") == 0)
    {
        request.source.source = "ab\xf0\x9f\x98\x80";
        request.source.source_bytes = 6U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "hierarchy") == 0)
    {
        rows = "[" SYMBOL "]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "mixed") == 0)
    {
        rows = "[" FLAT "," SYMBOL "]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "missing-range") == 0)
    {
        rows = "[{\"name\":\"abc\",\"kind\":12,\"location\":{\"uri\":\"file:///workspace/main.c\"}}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "invalid-result") == 0)
    {
        rows = "[{}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "notification") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"method\":\"window/"
                       "logMessage\",\"params\":{\"type\":3,\"message\":\"indexing\"}}");
    if (strcmp(mode, "wrong-id") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":99,\"result\":[]}");
    (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":%s}", rows);
    if (strcmp(mode, "error") == 0)
    {
        strcpy(response,
               "{\"jsonrpc\":\"2.0\",\"id\":2,\"error\":{\"code\":-32603,\"message\":\"refused\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    if (strcmp(mode, "timeout") != 0)
        Push(&fixture, response);
    if (strcmp(mode, "shutdown-error") == 0)
    {
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":3,\"error\":{\"code\":-32603,\"message\":\"shutdown\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (strcmp(mode, "timeout") != 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":3,\"result\":null}");
    if (strcmp(mode, "fragmented") == 0)
        fixture.fragment = 1U;
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
        request.source.timeout_ms = 5U;
        expected = UMI_STATUS_TIMEOUT;
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
        request.source.source = "a\xc0\x80";
        expected = UMI_STATUS_PARSE_ERROR;
        untouched = 1;
    }
    UmiLanguageRuntimeServer *server = Server(&fixture, request.source.root_uri);
    if (strcmp(mode, "state") == 0)
    {
        CHECK(umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING) ==
              UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
        untouched = 1;
    }
    UmiLanguageSymbolReport report;
    UmiLanguageSymbolCatalogue *catalogue = NULL;
    CHECK(UmiLanguageWorkspaceSymbolQueryOnServer(server, &request, cancel, &report, &catalogue) == expected);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(report.symbols == count && UmiLanguageSymbolCatalogueCount(catalogue) == count);
        if (count != 0U)
        {
            UmiLanguageSymbol item;
            CHECK(UmiLanguageSymbolCatalogueAt(catalogue, 0U, &item) == UMI_STATUS_OK);
            CHECK(!item.hierarchical && item.depth == 0U);
        }
        const char *opened = strstr(fixture.written, "textDocument/didOpen"),
                   *query = strstr(fixture.written, "\"method\":\"workspace/symbol\""),
                   *closed = strstr(fixture.written, "textDocument/didClose"),
                   *shutdown = strstr(fixture.written, "\"method\":\"shutdown\"");
        CHECK(opened != NULL && query != NULL && closed != NULL && shutdown != NULL && opened < query &&
              query < closed && closed < shutdown);
        CHECK(strstr(fixture.written, "resolveSupport") == NULL &&
              strstr(fixture.written, "hierarchicalDocumentSymbolSupport") == NULL);
        CHECK(strstr(fixture.written, "textDocument/documentSymbol") == NULL);
        if (strcmp(mode, "empty-query") == 0)
            CHECK(strstr(query, "\"query\":\"\"") != NULL);
        if (strcmp(mode, "escaped-query") == 0)
            CHECK(strstr(query, "\"query\":\"a\\\"\\\\\\nb\"") != NULL);
        if (strcmp(mode, "unicode-query") == 0)
            CHECK(strstr(query, "caf\xc3\xa9") != NULL);
    }
    else
        CHECK(catalogue == NULL);
    if (untouched)
        CHECK(fixture.writeSize == 0U && fixture.stops == 0U && !report.started);
    else
        CHECK(fixture.stops == 1U && report.initialized == !initialize_failure &&
              !umi_language_runtime_server_is_running(server));
    if (initialize_failure)
        CHECK(strstr(fixture.written, "textDocument/didOpen") == NULL);
    if (strcmp(mode, "shutdown-error") == 0 || strcmp(mode, "close-error") == 0)
        CHECK(report.query_status == UMI_STATUS_OK);
    UmiLanguageSymbolCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
