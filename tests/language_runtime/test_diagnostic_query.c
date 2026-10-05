/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_diagnostic_query.c
 * PURPOSE: Check diagnostic publication freshness, exact positions and cleanup before publication.
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

#include "umicom/language_runtime/diagnostic_query.h"

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"valid",         "object",          "encoding",
                           "sync",          "fragmented",      "noise",
                           "wrong-uri",     "stale-version",   "unversioned",
                           "empty",         "invalid-content", "outside",
                           "surrogate",     "related",         "shutdown-error",
                           "close-error",   "timeout",         "publication-timeout",
                           "cancel-before", "cancel-during",   "invalid-source",
                           "state",         "native-invalid",  "read-error",
                           "write-error",   "escaped-limit",   "server-request",
                           "unknown-tag",   "future-version"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    UmiLanguageDiagnosticRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 500U};
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, initialize_failure = 0;
    const char *caps = "{\"textDocumentSync\":2}";
    if (strcmp(mode, "object") == 0)
        caps = "{\"textDocumentSync\":{\"openClose\":true}}";
    if (strcmp(mode, "encoding") == 0)
    {
        caps = "{\"textDocumentSync\":2,\"positionEncoding\":\"utf-8\"}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "sync") == 0)
    {
        caps = "{\"textDocumentSync\":{\"openClose\":false}}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    char response[4096];
    (void)snprintf(response, sizeof(response),
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":%s}}", caps);
    Push(&fixture, response);
    const char *items = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                        "\"character\":3}},\"severity\":2,\"message\":\"Unknown name\"}]";
    if (strcmp(mode, "empty") == 0)
        items = "[]";
    if (strcmp(mode, "invalid-content") == 0)
    {
        items = "[{\"message\":\"missing range\"}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "outside") == 0)
    {
        request.source_bytes = 2U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "surrogate") == 0)
    {
        request.source = "ab\xf0\x9f\x98\x80";
        request.source_bytes = 6U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "related") == 0)
        items = "[{\"message\":\"name\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":"
                "0,\"character\":3}},\"relatedInformation\":[{\"message\":\"other "
                "declaration\",\"location\":{\"uri\":\"file:///"
                "other.c\",\"range\":{\"start\":{\"line\":9,\"character\":0},\"end\":{\"line\":9,"
                "\"character\":1}}}}]}]";
    if (strcmp(mode, "unknown-tag") == 0)
        items = "[{\"message\":\"name\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":"
                "0,\"character\":3}},\"tags\":[77]}]";
    if (strcmp(mode, "noise") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"method\":\"window/"
                       "logMessage\",\"params\":{\"type\":3,\"message\":\"working\"}}");
    if (strcmp(mode, "wrong-uri") == 0 || strcmp(mode, "stale-version") == 0 ||
        strcmp(mode, "future-version") == 0)
    {
        (void)snprintf(response, sizeof(response),
                       "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/"
                       "publishDiagnostics\",\"params\":{\"uri\":\"%s\",\"version\":%d,\"diagnostics\":[]}}",
                       strcmp(mode, "wrong-uri") == 0 ? "file:///other.c" : request.document_uri,
                       strcmp(mode, "stale-version") == 0    ? 0
                       : strcmp(mode, "future-version") == 0 ? 2
                                                             : 1);
        Push(&fixture, response);
    }
    if (strcmp(mode, "server-request") == 0)
    {
        Push(&fixture,
             "{\"jsonrpc\":\"2.0\",\"id\":99,\"method\":\"workspace/configuration\",\"params\":{}}");
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "publication-timeout") != 0)
    {
        (void)snprintf(
            response, sizeof(response),
            "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\",\"params\":{\"uri\":\"file:/"
            "//workspace/main.c\",%s\"diagnostics\":%s}}",
            strcmp(mode, "unversioned") == 0 ? "" : "\"version\":1,", items);
        Push(&fixture, response);
    }
    if (strcmp(mode, "shutdown-error") == 0)
    {
        Push(&fixture,
             "{\"jsonrpc\":\"2.0\",\"id\":2,\"error\":{\"code\":-32603,\"message\":\"shutdown refused\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (strcmp(mode, "publication-timeout") != 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":null}");
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
    if (strcmp(mode, "timeout") == 0 || strcmp(mode, "publication-timeout") == 0)
    {
        request.timeout_ms = 5U;
        expected = UMI_STATUS_TIMEOUT;
        if (strcmp(mode, "timeout") == 0)
        {
            fixture.size = 0U;
            initialize_failure = 1;
        }
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
    char *large = NULL;
    if (strcmp(mode, "escaped-limit") == 0)
    {
        large = malloc(40001U);
        CHECK(large != NULL);
        memset(large, '"', 40000U);
        large[40000] = '\0';
        request.source = large;
        request.source_bytes = 40000U;
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
    UmiLanguageDiagnosticReport report;
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    UmiStatus status;
    if (strcmp(mode, "native-invalid") == 0)
    {
        UmiLanguageServerProfile profile = {0};
        strcpy(profile.id, "invalid");
        profile.enabled = 1;
        status = UmiLanguageDiagnosticQueryNative(&profile, NULL, &request, cancel, &report, &catalogue);
        CHECK(status != UMI_STATUS_OK && !report.started && catalogue == NULL);
        untouched = 1;
    }
    else
    {
        status = UmiLanguageDiagnosticQueryOnServer(server, &request, cancel, &report, &catalogue);
        CHECK(status == expected);
        if (status == UMI_STATUS_OK)
        {
            size_t count = strcmp(mode, "empty") == 0 ? 0U : 1U;
            CHECK(report.diagnostics == count && UmiLanguageDiagnosticCatalogueCount(catalogue) == count);
            UmiLanguageDiagnosticPublication publication;
            CHECK(UmiLanguageDiagnosticCataloguePublication(catalogue, &publication) == UMI_STATUS_OK);
            CHECK(publication.has_version == (strcmp(mode, "unversioned") != 0));
            if (count != 0U)
            {
                UmiLanguageDiagnostic diagnostic;
                CHECK(UmiLanguageDiagnosticCatalogueAt(catalogue, 0U, &diagnostic) == UMI_STATUS_OK);
                CHECK(diagnostic.range.end.utf16_column == 3U);
            }
            CHECK(strstr(fixture.written, "\"versionSupport\":true") != NULL &&
                  strstr(fixture.written, "\"relatedInformation\":true") != NULL);
            CHECK(strstr(fixture.written, "\"method\":\"textDocument/diagnostic\"") == NULL);
            CHECK(strstr(fixture.written, "textDocument/codeAction") == NULL &&
                  strstr(fixture.written, "textDocument/completion") == NULL);
            CHECK(report.initialized && report.document_opened && report.close_status == UMI_STATUS_OK &&
                  report.shutdown_status == UMI_STATUS_OK);
        }
        else
            CHECK(catalogue == NULL);
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
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    free(large);
    return 0;
}
