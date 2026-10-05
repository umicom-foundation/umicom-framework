/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_signature_query.c
 * PURPOSE: Check parameter-help negotiation, exact positions and cleanup before publication.
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

#include "umicom/language_runtime/signature_query.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {
        "valid",         "object",          "missing",        "disabled",      "encoding",
        "sync",          "fragmented",      "notification",   "error",         "invalid-content",
        "label-outside", "label-surrogate", "shutdown-error", "close-error",   "timeout",
        "cancel-before", "cancel-during",   "invalid-source", "invalid-caret", "state",
        "empty",         "unicode",         "native-invalid", "read-error",    "write-error",
        "escaped-limit", "overloads",       "markup",         "wrong-id",      "local-active"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    UmiLanguageSignatureRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 1U, 500U};
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, initialize_failure = 0;
    const char *caps = "{\"signatureHelpProvider\":{},\"textDocumentSync\":2}";
    if (strcmp(mode, "object") == 0)
        caps = "{\"signatureHelpProvider\":{},\"textDocumentSync\":{\"openClose\":true}}";
    if (strcmp(mode, "missing") == 0)
    {
        caps = "{\"textDocumentSync\":2}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "disabled") == 0)
    {
        caps = "{\"signatureHelpProvider\":false,\"textDocumentSync\":2}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "encoding") == 0)
    {
        caps = "{\"signatureHelpProvider\":{},\"textDocumentSync\":2,\"positionEncoding\":\"utf-8\"}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "sync") == 0)
    {
        caps = "{\"signatureHelpProvider\":{},\"textDocumentSync\":{\"openClose\":false}}";
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
    const char *contents =
        "{\"signatures\":[{\"label\":\"sum(left, "
        "right)\",\"parameters\":[{\"label\":\"left\"},{\"label\":[10,15]}]}],\"activeParameter\":1}";
    size_t count = 1U, active_parameter = 1U;
    if (strcmp(mode, "empty") == 0)
    {
        contents = "null";
        count = 0U;
    }
    if (strcmp(mode, "overloads") == 0)
    {
        contents = "{\"signatures\":[{\"label\":\"f()\"},{\"label\":\"f(value)\",\"parameters\":[{\"label\":"
                   "\"value\"}]}],\"activeSignature\":1}";
        count = 2U;
        active_parameter = 0U;
    }
    if (strcmp(mode, "markup") == 0)
    {
        contents = "{\"signatures\":[{\"label\":\"f(value)\",\"documentation\":{\"kind\":\"markdown\","
                   "\"value\":\"<script>literal</script>\"},\"parameters\":[{\"label\":\"value\"}]}]}";
        active_parameter = 0U;
    }
    if (strcmp(mode, "unicode") == 0)
    {
        request.source = "a\xf0\x9f\x8c\x8d"
                         "c";
        request.source_bytes = 6U;
        request.caret = 5U;
    }
    if (strcmp(mode, "local-active") == 0)
    {
        contents = "{\"signatures\":[{\"label\":\"sum(left, "
                   "right)\",\"parameters\":[{\"label\":\"left\"},{\"label\":[10,15]}],\"activeParameter\":0}"
                   "],\"activeParameter\":1}";
        active_parameter = 0U;
    }
    if (strcmp(mode, "invalid-content") == 0)
    {
        contents = "{\"signatures\":7}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "label-outside") == 0)
    {
        contents = "{\"signatures\":[{\"label\":\"f(a)\",\"parameters\":[{\"label\":[2,99]}]}]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "label-surrogate") == 0)
    {
        contents = "{\"signatures\":[{\"label\":\"f(\\ud83d\\ude00)\",\"parameters\":[{\"label\":[2,3]}]}]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "wrong-id") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":99,\"result\":{\"signatures\":[]}}");
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
        request.caret = 4U;
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
    UmiLanguageSignatureReport report;
    UmiLanguageSignatureCatalogue *document = NULL;
    UmiStatus status;
    if (strcmp(mode, "native-invalid") == 0)
    {
        UmiLanguageServerProfile profile = {0};
        strcpy(profile.id, "invalid");
        profile.enabled = 1;
        status = UmiLanguageSignatureQueryNative(&profile, NULL, &request, cancel, &report, &document);
        CHECK(status != UMI_STATUS_OK && !report.started && document == NULL);
        untouched = 1;
    }
    else
    {
        status = UmiLanguageSignatureQueryOnServer(server, &request, cancel, &report, &document);
        CHECK(status == expected);
        if (status == UMI_STATUS_OK)
        {
            CHECK(UmiLanguageSignatureCatalogueCount(document) == count && report.signatures == count);
            if (count != 0U)
            {
                UmiLanguageSignature signature;
                CHECK(UmiLanguageSignatureCatalogueAt(document, UmiLanguageSignatureCatalogueActive(document),
                                                      &signature) == UMI_STATUS_OK);
                CHECK(signature.active_parameter == active_parameter);
                if (strcmp(mode, "markup") == 0)
                    CHECK(strcmp(signature.documentation.text, "<script>literal</script>") == 0);
            }
            CHECK(strstr(fixture.written, "\"labelOffsetSupport\":true") != NULL);
            CHECK(strstr(fixture.written, "\"activeParameterSupport\":true") != NULL);
            CHECK(strstr(fixture.written, "\"contextSupport\":false") != NULL);
            CHECK(strstr(fixture.written, "\"method\":\"textDocument/signatureHelp\"") != NULL);
            CHECK(strstr(fixture.written, "textDocument/hover") == NULL &&
                  strstr(fixture.written, "textDocument/completion") == NULL);
            CHECK(strstr(fixture.written, strcmp(mode, "unicode") == 0
                                              ? "\"position\":{\"line\":0,\"character\":3}"
                                              : "\"position\":{\"line\":0,\"character\":1}") != NULL);
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
    UmiLanguageSignatureCatalogueDestroy(document);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    free(large);
    return 0;
}
