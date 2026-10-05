/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_action_diagnostics_query.c
 * PURPOSE: Check fresh diagnostic context, same-session action ordering and cleanup.
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
/* Keep the original small fixture buffers for review. The expanded input
     * allows a near-limit publication to reach the production context bounds. */
#if 0
    char input[16384], written[16384];
#endif
    char input[262144], written[16384];
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
    const char *known[] = {"oversized-context",
                           "valid",
                           "empty",
                           "filter",
                           "point",
                           "raw",
                           "stale",
                           "wrong-uri",
                           "missing",
                           "invalid-publication",
                           "outside",
                           "action-error",
                           "shutdown-error",
                           "cancel-before",
                           "encoding"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    UmiLanguageCodeActionRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abcd", 4U, 0U, 2U, 500U};
    UmiStatus expected = UMI_STATUS_OK;
    const char *caps =
        strcmp(mode, "encoding") == 0
            ? "{\"textDocumentSync\":2,\"codeActionProvider\":true,\"positionEncoding\":\"utf-8\"}"
            : "{\"textDocumentSync\":2,\"codeActionProvider\":true}";
    char response[4096];
    (void)snprintf(response, sizeof(response),
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":%s}}", caps);
    Push(&fixture, response);
    if (strcmp(mode, "stale") == 0 || strcmp(mode, "wrong-uri") == 0)
    {
        (void)snprintf(response, sizeof(response),
                       "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/"
                       "publishDiagnostics\",\"params\":{\"uri\":\"%s\",\"version\":%d,\"diagnostics\":[]}}",
                       strcmp(mode, "wrong-uri") == 0 ? "file:///other.c" : request.document_uri,
                       strcmp(mode, "stale") == 0 ? 0 : 1);
        Push(&fixture, response);
    }
    const char *diagnostics =
        "[{\"message\":\"first\",\"code\":7,\"data\":{\"opaque\":\"same-session\"},\"range\":{\"start\":{"
        "\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}}},{\"message\":\"second\",\"data\":"
        "{\"opaque\":\"other-range\"},\"range\":{\"start\":{\"line\":0,\"character\":2},\"end\":{\"line\":0,"
        "\"character\":4}}}]";
    if (strcmp(mode, "empty") == 0)
        diagnostics = "[]";
    if (strcmp(mode, "invalid-publication") == 0)
    {
        diagnostics = "[{\"message\":\"missing range\"}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "outside") == 0)
    {
        request.source = "abc";
        request.source_bytes = 3U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "point") == 0)
        request.range_start = request.range_end;

    if (strcmp(mode, "oversized-context") == 0)
    {
        /* The incoming publication fits the native body limit. Its diagnostic
         * nearly fills that limit, leaving too little room for an action's
         * larger range/context envelope. No partial action may be sent. */
        const char *prefix =
            "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\",\"params\":{\"uri\":\"file:/"
            "//workspace/"
            "main.c\",\"version\":1,\"diagnostics\":[{\"message\":\"m\",\"range\":{\"start\":{\"line\":0,"
            "\"character\":0},\"end\":{\"line\":0,\"character\":2}},\"data\":{\"padding\":\"";
        const char *suffix = "\"}}]}}";
        char *publication = malloc(65521U);
        CHECK(publication != NULL);
        size_t first = strlen(prefix), last = strlen(suffix);
        memcpy(publication, prefix, first);
        memset(publication + first, 'x', 65520U - first - last);
        memcpy(publication + 65520U - last, suffix, last + 1U);
        Push(&fixture, publication);
        free(publication);
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "missing") == 0)
    {
        request.timeout_ms = 5U;
        expected = UMI_STATUS_TIMEOUT;
    }
    else if (strcmp(mode, "oversized-context") != 0)
    {
        (void)snprintf(
            response, sizeof(response),
            "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\",\"params\":{\"uri\":\"file:/"
            "//workspace/main.c\",\"version\":1,\"diagnostics\":%s}}",
            diagnostics);
        Push(&fixture, response);
    }
    if (expected == UMI_STATUS_OK)
    {
        if (strcmp(mode, "action-error") == 0)
        {
            Push(&fixture,
                 "{\"jsonrpc\":\"2.0\",\"id\":2,\"error\":{\"code\":-32603,\"message\":\"action refused\"}}");
            expected = UMI_STATUS_UNAVAILABLE;
        }
        else
            Push(&fixture,
                 "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":[{\"title\":\"Fix name\",\"edit\":{}}]}");
        if (strcmp(mode, "shutdown-error") == 0)
        {
            Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":3,\"error\":{\"code\":-32603,\"message\":\"shutdown "
                           "refused\"}}");
            expected = UMI_STATUS_UNAVAILABLE;
        }
        else
            Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":3,\"result\":null}");
    }
    else if (strcmp(mode, "missing") != 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":null}");
    if (strcmp(mode, "encoding") == 0)
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel-before") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    UmiLanguageRuntimeServer *server = Server(&fixture, request.root_uri);
    UmiLanguageCodeActionReport report;
    UmiLanguageCodeActionCatalogue *catalogue = NULL;
    CHECK(UmiLanguageCodeActionQueryOnServerWithDiagnostics(server, &request, cancel, &report, &catalogue) ==
          expected);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(report.actions == 1U && UmiLanguageCodeActionCatalogueCount(catalogue) == 1U &&
              report.shutdown_status == UMI_STATUS_OK);
        const char *action = strstr(fixture.written, "\"method\":\"textDocument/codeAction\"");
        CHECK(action != NULL);
        CHECK(strstr(fixture.written, "\"dataSupport\":true") != NULL &&
              strstr(fixture.written, "\"publishDiagnostics\":") != NULL);
        if (strcmp(mode, "empty") == 0)
            CHECK(strstr(action, "\"diagnostics\":[]") != NULL);
        else
        {
            CHECK(strstr(action, "\"opaque\":\"same-session\"") != NULL &&
                  strstr(action, "\"code\":7") != NULL);
            CHECK((strstr(action, "other-range") != NULL) == (strcmp(mode, "point") == 0));
        }
        CHECK(strstr(fixture.written, "textDocument/diagnostic\"") == NULL);
    }
    else
        CHECK(catalogue == NULL);
    if (strcmp(mode, "oversized-context") == 0 || strcmp(mode, "missing") == 0 ||
        strcmp(mode, "invalid-publication") == 0 || strcmp(mode, "outside") == 0)
        CHECK(strstr(fixture.written, "textDocument/codeAction\"") == NULL);
    if (strcmp(mode, "cancel-before") == 0)
        CHECK(!report.started && fixture.stops == 0U);
    else
        CHECK(fixture.stops == 1U);
    UmiLanguageCodeActionCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
