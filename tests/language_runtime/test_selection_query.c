/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_selection_query.c
 * PURPOSE: Check definition/reference negotiation, exact positions and cleanup before publication.
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
#define LOCATION                                                                                             \
    "{\"uri\":\"file:///workspace/"                                                                          \
    "main.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}}}"
#define LINK                                                                                                 \
    "{\"targetUri\":\"file:///workspace/"                                                                    \
    "other.c\",\"targetRange\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":2,\"character\":0}" \
    "},\"targetSelectionRange\":{\"start\":{\"line\":1,\"character\":0},\"end\":{\"line\":1,\"character\":"  \
    "4}},\"originSelectionRange\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"              \
    "\"character\":1}}}"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {
        "valid",          "object",         "missing",        "disabled",     "wrong-provider",
        "encoding",       "sync",           "fragmented",     "notification", "error",
        "invalid-result", "shutdown-error", "close-error",    "timeout",      "cancel-before",
        "cancel-during",  "invalid-source", "invalid-caret",  "invalid-kind", "invalid-declaration",
        "state",          "empty",          "unicode",        "surrogate",    "outside",
        "outer-outside",  "caret-outside",  "parent-shrinks", "empty-array",  "multiple",
        "multiline",      "native-invalid", "read-error",     "write-error",  "wrong-id"};
    int known_mode = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            known_mode = 1;
    CHECK(known_mode);
    Fixture fixture = {0};
    UmiLanguageNavigationRequest request = {"file:///workspace",
                                            "file:///workspace/main.c",
                                            "c",
                                            "abc",
                                            3U,
                                            1U,
                                            500U,
                                            UMI_LANGUAGE_NAVIGATION_SELECTION_RANGES,
                                            0};

    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, initialize_failure = 0;
    const char *value = strcmp(mode, "object") == 0 ? "{}" : strcmp(mode, "disabled") == 0 ? "false" : "true";
    const char *sync = strcmp(mode, "sync") == 0 ? "{\"openClose\":false}" : "2";
    char caps[1024], response[4096];
    (void)snprintf(caps, sizeof(caps),
                   "{\"selectionRangeProvider\":%s,\"textDocumentSync\":%s,\"positionEncoding\":\"%s\"}",
                   value, sync, strcmp(mode, "encoding") == 0 ? "utf-8" : "utf-16");
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
    const char *uri = request.document_uri;
    const int link = 0;
    size_t count = 2U;
    const char *locations = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                            "\"character\":2}},\"parent\":{\"range\":{\"start\":{\"line\":0,\"character\":0},"
                            "\"end\":{\"line\":0,\"character\":3}}}}]";
    if (strcmp(mode, "empty") == 0)
    {
        locations = "null";
        count = 0U;
    }
    if (strcmp(mode, "notification") == 0)
        Push(&fixture,
             "{\"jsonrpc\":\"2.0\",\"method\":\"window/logMessage\",\"params\":{\"message\":\"working\"}}");
    if (strcmp(mode, "unicode") == 0 || strcmp(mode, "surrogate") == 0)
    {
        request.source = "a\xf0\x9f\x8c\x8d"
                         "c";
        request.source_bytes = 6U;
        request.caret = 5U;
        locations = "[{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}"
                    "},\"parent\":{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                    "\"character\":4}}}}]";
        if (strcmp(mode, "surrogate") == 0)
        {
            request.caret = 1U;
            locations =
                "[{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":2}}}]";
            expected = UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    if (strcmp(mode, "multiline") == 0)
    {
        request.source = "a\r\nbc";
        request.source_bytes = 5U;
        request.caret = 3U;
        locations = "[{\"range\":{\"start\":{\"line\":1,\"character\":0},\"end\":{\"line\":1,\"character\":1}"
                    "},\"parent\":{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":1,"
                    "\"character\":2}}}}]";
    }
    if (strcmp(mode, "outside") == 0)
    {
        locations =
            "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":4}}}]";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "outer-outside") == 0)
    {
        locations = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}"
                    "},\"parent\":{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                    "\"character\":4}}}}]";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "caret-outside") == 0)
    {
        locations =
            "[{\"range\":{\"start\":{\"line\":0,\"character\":2},\"end\":{\"line\":0,\"character\":3}}}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "parent-shrinks") == 0)
    {
        locations = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}"
                    "},\"parent\":{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,"
                    "\"character\":2}}}}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "invalid-result") == 0)
    {
        locations = "[{}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "empty-array") == 0)
    {
        locations = "[]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "multiple") == 0)
    {
        locations = "[{},{}]";
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
        (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":%s}", locations);
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
        request.caret = 4U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-kind") == 0)
    {
        request.kind = (UmiLanguageNavigationKind)99;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-declaration") == 0)
    {
        request.include_declaration = 2;
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
    UmiStatus status;
    if (strcmp(mode, "native-invalid") == 0)
    {
        UmiLanguageServerProfile profile = {0};
        strcpy(profile.id, "invalid");
        profile.enabled = 1;
        status = UmiLanguageNavigationQueryNative(&profile, NULL, &request, cancel, &report, &catalogue);
        CHECK(status != UMI_STATUS_OK && !report.started && catalogue == NULL);
        untouched = 1;
    }
    else
    {
        status = UmiLanguageNavigationQueryOnServer(server, &request, cancel, &report, &catalogue);
        CHECK(status == expected);
        if (status == UMI_STATUS_OK)
        {
            CHECK(report.locations == count && UmiLanguageLocationCatalogueCount(catalogue) == count);
            UmiLanguageSourceLocation location;
            if (count != 0U)
            {
                CHECK(UmiLanguageLocationCatalogueAt(catalogue, 0U, &location) == UMI_STATUS_OK);
                CHECK(strcmp(location.uri, uri) == 0 && location.is_link == link);
            }
            CHECK(strstr(fixture.written, "\"selectionRange\":{\"dynamicRegistration\":false}") != NULL);
            CHECK(strstr(fixture.written, "\"method\":\"textDocument/selectionRange\"") != NULL);
            CHECK(strstr(fixture.written, "textDocument/definition") == NULL &&
                  strstr(fixture.written, "includeDeclaration") == NULL);
            CHECK(strstr(fixture.written, strcmp(mode, "unicode") == 0
                                              ? "\"positions\":[{\"line\":0,\"character\":3}]"
                                          : strcmp(mode, "multiline") == 0
                                              ? "\"positions\":[{\"line\":1,\"character\":0}]"
                                              : "\"positions\":[{\"line\":0,\"character\":1}]") != NULL);
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
    UmiLanguageLocationCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
