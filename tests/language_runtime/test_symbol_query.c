/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_symbol_query.c
 * PURPOSE: Check document-symbol negotiation, hierarchy, captured range validation and cleanup before publication.
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
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {
        "valid",          "object",         "flat",           "hierarchy",      "empty",
        "unicode",        "self-outside",   "self-surrogate", "external",       "missing",
        "disabled",       "encoding",       "sync",           "fragmented",     "notification",
        "error",          "invalid-result", "mixed",          "shutdown-error", "close-error",
        "timeout",        "cancel-before",  "cancel-during",  "invalid-source", "state",
        "native-invalid", "read-error",     "write-error",    "wrong-id",       "arguments"};
    int known_mode = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            known_mode = 1;
    CHECK(known_mode);
    Fixture fixture = {0};
    UmiLanguageSymbolRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 500U};
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, initialize_failure = 0;
    const char *caps = "{\"documentSymbolProvider\":true,\"textDocumentSync\":2}";
    if (strcmp(mode, "object") == 0)
        caps = "{\"documentSymbolProvider\":{\"label\":\"C "
               "outline\"},\"textDocumentSync\":{\"openClose\":true}}";
    if (strcmp(mode, "missing") == 0)
    {
        caps = "{\"textDocumentSync\":2}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "disabled") == 0)
    {
        caps = "{\"documentSymbolProvider\":false,\"textDocumentSync\":2}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "encoding") == 0)
    {
        caps = "{\"documentSymbolProvider\":true,\"textDocumentSync\":2,\"positionEncoding\":\"utf-8\"}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "sync") == 0)
    {
        caps = "{\"documentSymbolProvider\":true,\"textDocumentSync\":{\"openClose\":false}}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    char response[4096];
    if (strcmp(mode, "notification") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"method\":\"window/"
                       "logMessage\",\"params\":{\"type\":3,\"message\":\"indexing\"}}");
    (void)snprintf(response, sizeof(response),
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":%s}}", caps);
    Push(&fixture, response);
    const char *symbols = "[" SYMBOL "]";
    size_t count = 1U;
    if (strcmp(mode, "empty") == 0)
    {
        symbols = "null";
        count = 0U;
    }
    if (strcmp(mode, "flat") == 0)
        symbols = "[" FLAT "]";
    if (strcmp(mode, "hierarchy") == 0)
    {
        symbols = "[{\"name\":\"outer\",\"kind\":5,\"range\":" RANGE ",\"selectionRange\":" RANGE
                  ",\"children\":[" SYMBOL "]}]";
        count = 2U;
    }
    if (strcmp(mode, "self-outside") == 0)
    {
        symbols = "[{\"name\":\"outer\",\"kind\":5,\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":"
                  "{\"line\":9,\"character\":0}},\"selectionRange\":" RANGE "}]";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "unicode") == 0 || strcmp(mode, "self-surrogate") == 0)
    {
        request.source = "a\xf0\x9f\x8c\x8d"
                         "c";
        request.source_bytes = 6U;
    }
    if (strcmp(mode, "self-surrogate") == 0)
    {
        symbols = "[{\"name\":\"abc\",\"kind\":12,\"range\":" RANGE
                  ",\"selectionRange\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,"
                  "\"character\":2}}}]";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "external") == 0)
        symbols = "[{\"name\":\"external\",\"kind\":12,\"location\":{\"uri\":\"file:///workspace/"
                  "other.c\",\"range\":{\"start\":{\"line\":8,\"character\":0},\"end\":{\"line\":9,"
                  "\"character\":0}}}}]";
    if (strcmp(mode, "invalid-result") == 0)
    {
        symbols = "[{}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "mixed") == 0)
    {
        symbols = "[" SYMBOL "," FLAT "]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "wrong-id") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":99,\"result\":null}");
    if (strcmp(mode, "error") == 0)
    {
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":2,\"error\":{\"code\":-1,\"message\":\"refused\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else
    {
        (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":%s}", symbols);
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
    UmiLanguageRuntimeServer *server = Server(&fixture, request.root_uri);
    if (strcmp(mode, "state") == 0)
    {
        CHECK(umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING) ==
              UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
        untouched = 1;
    }
    UmiLanguageSymbolReport report;
    UmiLanguageSymbolCatalogue *catalogue = NULL;
    UmiStatus status;
    if (strcmp(mode, "native-invalid") == 0)
    {
        status = UmiLanguageSymbolQueryNative(NULL, NULL, &request, cancel, &report, &catalogue);
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    else
        status = UmiLanguageSymbolQueryOnServer(server, &request, cancel, &report, &catalogue);
    CHECK(status == expected);
    if (untouched)
        CHECK(fixture.writeSize == 0U && fixture.stops == 0U && !report.started);
    else
    {
        CHECK(fixture.stops >= 1U);
        if (!fixture.failWrite)
        {
            CHECK(strstr(fixture.written, "\"documentSymbol\":{\"dynamicRegistration\":false,"
                                          "\"hierarchicalDocumentSymbolSupport\":true") != NULL);
            CHECK(strstr(fixture.written, "\"tagSupport\":{\"valueSet\":[1]}") != NULL);
        }
        if (!initialize_failure)
        {
            CHECK(report.document_opened);
            CHECK(strstr(fixture.written, "\"method\":\"textDocument/documentSymbol\"") != NULL);
            CHECK(strstr(fixture.written, "\"position\":") == NULL &&
                  strstr(fixture.written, "\"options\":") == NULL);
            if (!fixture.failClose)
                CHECK(strstr(fixture.written, "textDocument/didClose") != NULL);
        }
    }
    if (status == UMI_STATUS_OK)
    {
        CHECK(report.symbols == count && UmiLanguageSymbolCatalogueCount(catalogue) == count);
        if (count != 0U)
        {
            UmiLanguageSymbol symbol;
            CHECK(UmiLanguageSymbolCatalogueAt(catalogue, count - 1U, &symbol) == UMI_STATUS_OK);
            CHECK(symbol.depth == (strcmp(mode, "hierarchy") == 0 ? 1U : 0U));
        }
    }
    else
        CHECK(catalogue == NULL);
    if (strcmp(mode, "arguments") == 0)
    {
        UmiLanguageSymbolCatalogue *none = catalogue;
        CHECK(UmiLanguageSymbolQueryOnServer(server, NULL, cancel, &report, &none) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              none == NULL);
        CHECK(UmiLanguageSymbolQueryNative(NULL, NULL, NULL, NULL, NULL, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    UmiLanguageSymbolCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
