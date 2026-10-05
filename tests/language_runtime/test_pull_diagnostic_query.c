/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_pull_diagnostic_query.c
 * PURPOSE: Check explicit diagnostic negotiation, response correlation and child cleanup.
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
    const char *known[] = {"valid",          "identifier",
                           "no-identifier",  "empty",
                           "fragmented",     "notification-noise",
                           "wrong-reply",    "missing-provider",
                           "provider-type",  "missing-flags",
                           "flag-type",      "identifier-type",
                           "encoding",       "sync",
                           "unchanged",      "related",
                           "invalid-items",  "outside",
                           "surrogate",      "query-error",
                           "shutdown-error", "close-error",
                           "cancel-before",  "cancel-during",
                           "invalid-source", "state",
                           "server-request", "timeout",
                           "read-error",     "write-error"};
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
    const char *provider = "{\"interFileDependencies\":true,\"workspaceDiagnostics\":false,\"identifier\":"
                           "\"tool\\\"\\\\scope\"}";
    const char *sync = "2", *encoding = "utf-16";
    if (strcmp(mode, "no-identifier") == 0)
        provider = "{\"interFileDependencies\":false,\"workspaceDiagnostics\":true}";
    if (strcmp(mode, "provider-type") == 0)
    {
        provider = "true";
        expected = UMI_STATUS_PARSE_ERROR;
        initialize_failure = 1;
    }
    if (strcmp(mode, "missing-flags") == 0)
    {
        provider = "{}";
        expected = UMI_STATUS_PARSE_ERROR;
        initialize_failure = 1;
    }
    if (strcmp(mode, "flag-type") == 0)
    {
        provider = "{\"interFileDependencies\":1,\"workspaceDiagnostics\":false}";
        expected = UMI_STATUS_PARSE_ERROR;
        initialize_failure = 1;
    }
    if (strcmp(mode, "identifier-type") == 0)
    {
        provider = "{\"interFileDependencies\":false,\"workspaceDiagnostics\":false,\"identifier\":null}";
        expected = UMI_STATUS_PARSE_ERROR;
        initialize_failure = 1;
    }
    if (strcmp(mode, "missing-provider") == 0)
    {
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "encoding") == 0)
    {
        encoding = "utf-8";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "sync") == 0)
    {
        sync = "{\"openClose\":false}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    char response[4096];
    if (strcmp(mode, "missing-provider") == 0)
        strcpy(response,
               "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{\"textDocumentSync\":2}}}");
    else
        (void)snprintf(response, sizeof(response),
                       "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{\"textDocumentSync\":%s,"
                       "\"positionEncoding\":\"%s\",\"diagnosticProvider\":%s}}}",
                       sync, encoding, provider);
    Push(&fixture, response);
    const char *items =
        "[{\"message\":\"Unknown "
        "name\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}}}]";
    if (strcmp(mode, "empty") == 0)
        items = "[]";
    if (strcmp(mode, "invalid-items") == 0)
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
    if (strcmp(mode, "notification-noise") == 0)
        Push(&fixture,
             "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/"
             "publishDiagnostics\",\"params\":{\"uri\":\"file:///workspace/main.c\",\"diagnostics\":[]}}");
    if (strcmp(mode, "wrong-reply") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":99,\"result\":{\"kind\":\"full\",\"items\":[]}}");
    if (strcmp(mode, "server-request") == 0)
    {
        Push(&fixture,
             "{\"jsonrpc\":\"2.0\",\"id\":99,\"method\":\"workspace/diagnostic/refresh\",\"params\":{}}");
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "query-error") == 0)
    {
        strcpy(response, "{\"jsonrpc\":\"2.0\",\"id\":2,\"error\":{\"code\":-32802,\"message\":\"retry\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (strcmp(mode, "unchanged") == 0)
    {
        strcpy(response,
               "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":{\"kind\":\"unchanged\",\"resultId\":\"old\"}}");
        expected = UMI_STATUS_INVALID_STATE;
    }
    else
    {
        (void)snprintf(response, sizeof(response),
                       "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":{\"kind\":\"full\",\"resultId\":\"fresh\","
                       "\"items\":%s%s}}",
                       items,
                       strcmp(mode, "related") == 0
                           ? ",\"relatedDocuments\":{\"file:///other.c\":{\"kind\":\"full\",\"items\":[]}}"
                           : "");
        if (strcmp(mode, "related") == 0)
            expected = UMI_STATUS_NOT_IMPLEMENTED;
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
        request.timeout_ms = 5U;
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
    UmiLanguageDiagnosticReport report;
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    CHECK(UmiLanguageDiagnosticQueryPullOnServer(server, &request, cancel, &report, &catalogue) == expected);
    if (expected == UMI_STATUS_OK)
    {
        size_t count = strcmp(mode, "empty") == 0 ? 0U : 1U;
        CHECK(report.diagnostics == count && UmiLanguageDiagnosticCatalogueCount(catalogue) == count);
        UmiLanguageDiagnosticPublication publication;
        CHECK(UmiLanguageDiagnosticCataloguePublication(catalogue, &publication) == UMI_STATUS_OK &&
              !publication.has_version);
        const char *result_id = NULL;
        CHECK(UmiLanguageDiagnosticCataloguePullResultId(catalogue, &result_id) == UMI_STATUS_OK &&
              strcmp(result_id, "fresh") == 0);
        const char *opened = strstr(fixture.written, "textDocument/didOpen"),
                   *query = strstr(fixture.written, "textDocument/diagnostic"),
                   *closed = strstr(fixture.written, "textDocument/didClose"),
                   *shutdown = strstr(fixture.written, "\"method\":\"shutdown\"");
        CHECK(opened != NULL && query != NULL && closed != NULL && shutdown != NULL && opened < query &&
              query < closed && closed < shutdown);
        CHECK(strstr(fixture.written, "\"relatedDocumentSupport\":false") != NULL &&
              strstr(fixture.written, "previousResultId") == NULL);
        CHECK((strstr(query, "\"identifier\":\"tool\\\"\\\\scope\"") != NULL) ==
              (strcmp(mode, "no-identifier") != 0));
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
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
