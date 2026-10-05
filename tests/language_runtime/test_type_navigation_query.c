/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_type_navigation_query.c
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
    const char *known[] = {"type-valid",
                           "type-object",
                           "type-link",
                           "type-origin-outside",
                           "type-self-outside",
                           "type-self-surrogate",
                           "type-external",
                           "type-missing",
                           "type-disabled",
                           "type-wrong-provider",
                           "type-encoding",
                           "type-sync",
                           "type-fragmented",
                           "type-notification",
                           "type-error",
                           "type-invalid-result",
                           "type-shutdown-error",
                           "type-close-error",
                           "type-timeout",
                           "type-cancel-before",
                           "type-cancel-during",
                           "type-invalid-source",
                           "type-invalid-caret",
                           "type-invalid-kind",
                           "type-invalid-declaration",
                           "type-state",
                           "type-empty",
                           "type-unicode",
                           "type-native-invalid",
                           "type-read-error",
                           "type-write-error",
                           "type-wrong-id",
                           "implementation-valid",
                           "implementation-object",
                           "implementation-link",
                           "implementation-origin-outside",
                           "implementation-self-outside",
                           "implementation-self-surrogate",
                           "implementation-external",
                           "implementation-missing",
                           "implementation-disabled",
                           "implementation-wrong-provider",
                           "implementation-encoding",
                           "implementation-sync",
                           "implementation-fragmented",
                           "implementation-notification",
                           "implementation-error",
                           "implementation-invalid-result",
                           "implementation-shutdown-error",
                           "implementation-close-error",
                           "implementation-timeout",
                           "implementation-cancel-before",
                           "implementation-cancel-during",
                           "implementation-invalid-source",
                           "implementation-invalid-caret",
                           "implementation-invalid-kind",
                           "implementation-invalid-declaration",
                           "implementation-state",
                           "implementation-empty",
                           "implementation-unicode",
                           "implementation-native-invalid",
                           "implementation-read-error",
                           "implementation-write-error",
                           "implementation-wrong-id"};
    int known_mode = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            known_mode = 1;
    CHECK(known_mode);
    int implementation = strncmp(mode, "implementation-", 15U) == 0;
    mode += implementation ? 15U : 5U;

    Fixture fixture = {0};
    UmiLanguageNavigationRequest request = {"file:///workspace",
                                            "file:///workspace/main.c",
                                            "c",
                                            "abc",
                                            3U,
                                            1U,
                                            500U,
                                            implementation ? UMI_LANGUAGE_NAVIGATION_IMPLEMENTATION
                                                           : UMI_LANGUAGE_NAVIGATION_TYPE_DEFINITION,
                                            0};

    /* Each capability must stand on its own. A definition provider cannot
     * authorize type or implementation queries, even when shapes are shared. */
    const int references = 0;
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, initialize_failure = 0;
    const char *provider = implementation ? "implementationProvider" : "typeDefinitionProvider";
    const char *capability = implementation
                                 ? "\"implementation\":{\"dynamicRegistration\":false,\"linkSupport\":true}"
                                 : "\"typeDefinition\":{\"dynamicRegistration\":false,\"linkSupport\":true}";
    const char *method = implementation ? "\"method\":\"textDocument/implementation\""
                                        : "\"method\":\"textDocument/typeDefinition\"";
    const char *value = "true", *sync = "2", *encoding = "utf-16";
    if (strcmp(mode, "object") == 0)
        value = "{}";
    if (strcmp(mode, "disabled") == 0)
        value = "false";
    if (strcmp(mode, "encoding") == 0)
        encoding = "utf-8";
    if (strcmp(mode, "sync") == 0)
        sync = "{\"openClose\":false}";
    char caps[1024];
    (void)snprintf(caps, sizeof(caps), "{\"%s\":%s,\"textDocumentSync\":%s,\"positionEncoding\":\"%s\"}",
                   provider, value, sync, encoding);
    if (strcmp(mode, "missing") == 0)
        strcpy(caps, "{\"textDocumentSync\":2}");
    if (strcmp(mode, "wrong-provider") == 0)
        strcpy(caps, "{\"definitionProvider\":true,\"textDocumentSync\":2}");
    if (strcmp(mode, "missing") == 0 || strcmp(mode, "wrong-provider") == 0 ||
        strcmp(mode, "disabled") == 0 || strcmp(mode, "encoding") == 0 || strcmp(mode, "sync") == 0)
    {
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    char response[4096];
    if (strcmp(mode, "notification") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"method\":\"window/"
                       "logMessage\",\"params\":{\"type\":3,\"message\":\"working\"}}");
    (void)snprintf(response, sizeof(response),
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":%s}}", caps);
    Push(&fixture, response);
    const char *locations = references ? "[" LOCATION "]" : LOCATION;
    size_t count = 1U;
    int link = 0;
    const char *uri = request.document_uri;
    if (strcmp(mode, "empty") == 0)
    {
        locations = "null";
        count = 0U;
    }
    if (strcmp(mode, "link") == 0)
    {
        locations = "[" LINK "]";
        link = 1;
        uri = "file:///workspace/other.c";
    }
    if (strcmp(mode, "references-object") == 0)
    {
        locations = LOCATION;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "references-link") == 0)
    {
        locations = "[" LINK "]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "origin-outside") == 0)
    {
        locations = "[{\"targetUri\":\"file:///workspace/"
                    "other.c\",\"targetRange\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                    "\"character\":0}},\"targetSelectionRange\":{\"start\":{\"line\":0,\"character\":0},"
                    "\"end\":{\"line\":0,\"character\":0}},\"originSelectionRange\":{\"start\":{\"line\":1,"
                    "\"character\":0},\"end\":{\"line\":1,\"character\":1}}}]";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "self-outside") == 0)
    {
        locations = "{\"uri\":\"file:///workspace/"
                    "main.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":4,"
                    "\"character\":0}}}";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "external") == 0)
    {
        locations = "{\"uri\":\"file:///workspace/"
                    "external.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":4,"
                    "\"character\":0}}}";
        uri = "file:///workspace/external.c";
    }
    if (strcmp(mode, "unicode") == 0 || strcmp(mode, "self-surrogate") == 0)
    {
        request.source = "a\xf0\x9f\x8c\x8d"
                         "c";
        request.source_bytes = 6U;
        request.caret = 5U;
    }
    if (strcmp(mode, "self-surrogate") == 0)
    {
        locations = "{\"uri\":\"file:///workspace/"
                    "main.c\",\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,"
                    "\"character\":2}}}";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-result") == 0)
    {
        locations = "[{}]";
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
            CHECK(strstr(fixture.written, capability) != NULL && strstr(fixture.written, method) != NULL);
            CHECK(strstr(fixture.written, "textDocument/definition") == NULL &&
                  strstr(fixture.written, "includeDeclaration") == NULL);
            CHECK(strstr(fixture.written, "textDocument/formatting") == NULL &&
                  strstr(fixture.written, "textDocument/completion") == NULL &&
                  strstr(fixture.written, "textDocument/hover") == NULL);
            CHECK(strstr(fixture.written, strcmp(mode, "unicode") == 0
                                              ? "\"position\":{\"line\":0,\"character\":3}"
                                              : "\"position\":{\"line\":0,\"character\":1}") != NULL);
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
