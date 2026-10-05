/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_action_pull_context.c
 * PURPOSE: Check same-session pull diagnostic context, selected ranges and complete query cleanup.
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

#include "umicom/language_runtime/code_action_query.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"valid",
                           "primary-only",
                           "empty",
                           "filter",
                           "point",
                           "raw",
                           "none",
                           "notification",
                           "invalid-mode",
                           "negative-mode",
                           "missing-provider",
                           "missing-actions",
                           "diagnostic-error",
                           "unchanged",
                           "related",
                           "invalid-diagnostics",
                           "outside",
                           "missing",
                           "action-error",
                           "close-error",
                           "shutdown-error",
                           "cancel-before",
                           "invalid-source",
                           "duplicate-source"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    UmiLanguageCodeActionRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abcd", 4U, 0U, 2U, 500U};
    UmiLanguageQuerySource extra = {"file:///workspace/other.c", "c", "other draft", 11U};
    size_t extra_count = strcmp(mode, "primary-only") == 0 ? 0U : 1U;
    UmiLanguageActionDiagnosticMode delivery = UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL;
    if (strcmp(mode, "none") == 0)
        delivery = UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE;
    if (strcmp(mode, "notification") == 0)
        delivery = UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION;
    if (strcmp(mode, "invalid-mode") == 0)
        delivery = (UmiLanguageActionDiagnosticMode)99;
    if (strcmp(mode, "negative-mode") == 0)
        delivery = (UmiLanguageActionDiagnosticMode)-1;
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, diagnostic_failure = 0, initialize_failure = 0;
    const char *caps = "{\"textDocumentSync\":2,\"codeActionProvider\":true,\"diagnosticProvider\":{"
                       "\"interFileDependencies\":true,\"workspaceDiagnostics\":false}}";
    if (strcmp(mode, "missing-provider") == 0)
    {
        caps = "{\"textDocumentSync\":2,\"codeActionProvider\":true}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "missing-actions") == 0)
    {
        caps = "{\"textDocumentSync\":2,\"diagnosticProvider\":{\"interFileDependencies\":true,"
               "\"workspaceDiagnostics\":false}}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    char response[4096];
    (void)snprintf(response, sizeof(response),
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":%s}}", caps);
    Push(&fixture, response);
    const char *diagnostics =
        "[{\"message\":\"first\",\"code\":7,\"data\":{\"opaque\":\"same-session\"},\"range\":{\"start\":{"
        "\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}}},{\"message\":\"second\",\"data\":"
        "{\"opaque\":\"other-range\"},\"range\":{\"start\":{\"line\":0,\"character\":2},\"end\":{\"line\":0,"
        "\"character\":4}}}]";
    if (strcmp(mode, "empty") == 0)
        diagnostics = "[]";
    if (strcmp(mode, "point") == 0)
        request.range_start = request.range_end;
    if (strcmp(mode, "invalid-diagnostics") == 0)
    {
        diagnostics = "[{\"message\":\"no range\"}]";
        expected = UMI_STATUS_PARSE_ERROR;
        diagnostic_failure = 1;
    }
    if (strcmp(mode, "outside") == 0)
    {
        request.source_bytes = 3U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        diagnostic_failure = 1;
    }
    if (delivery == UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION)
        (void)snprintf(
            response, sizeof(response),
            "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\",\"params\":{\"uri\":\"file:/"
            "//workspace/main.c\",\"version\":1,\"diagnostics\":%s}}",
            diagnostics);
    else if (strcmp(mode, "diagnostic-error") == 0)
    {
        strcpy(response, "{\"jsonrpc\":\"2.0\",\"id\":2,\"error\":{\"code\":-32802,\"message\":\"later\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
        diagnostic_failure = 1;
    }
    else if (strcmp(mode, "unchanged") == 0)
    {
        strcpy(response,
               "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":{\"kind\":\"unchanged\",\"resultId\":\"old\"}}");
        expected = UMI_STATUS_INVALID_STATE;
        diagnostic_failure = 1;
    }
    else
    {
        (void)snprintf(response, sizeof(response),
                       "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":{\"kind\":\"full\",\"items\":%s%s}}",
                       diagnostics,
                       strcmp(mode, "related") == 0
                           ? ",\"relatedDocuments\":{\"file:///other.c\":{\"kind\":\"full\",\"items\":[]}}"
                           : "");
        if (strcmp(mode, "related") == 0)
        {
            expected = UMI_STATUS_NOT_IMPLEMENTED;
            diagnostic_failure = 1;
        }
    }
    if (strcmp(mode, "missing") == 0)
    {
        request.timeout_ms = 5U;
        expected = UMI_STATUS_TIMEOUT;
        diagnostic_failure = 1;
    }
    else if (delivery != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE)
        Push(&fixture, response);
    unsigned action_id = delivery == UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL ? 3U : 2U;
    unsigned shutdown_id = diagnostic_failure ? 3U : action_id + 1U;
    if (!diagnostic_failure)
    {
        if (strcmp(mode, "action-error") == 0)
        {
            (void)snprintf(
                response, sizeof(response),
                "{\"jsonrpc\":\"2.0\",\"id\":%u,\"error\":{\"code\":-32603,\"message\":\"action\"}}",
                action_id);
            expected = UMI_STATUS_UNAVAILABLE;
        }
        else
            (void)snprintf(
                response, sizeof(response),
                "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":[{\"title\":\"Fix name\",\"edit\":{}}]}",
                action_id);
        Push(&fixture, response);
    }
    if (strcmp(mode, "missing") != 0)
    {
        if (strcmp(mode, "shutdown-error") == 0)
        {
            (void)snprintf(
                response, sizeof(response),
                "{\"jsonrpc\":\"2.0\",\"id\":%u,\"error\":{\"code\":-32603,\"message\":\"shutdown\"}}",
                shutdown_id);
            expected = UMI_STATUS_UNAVAILABLE;
        }
        else
            (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":null}",
                           shutdown_id);
        Push(&fixture, response);
    }
    if (strcmp(mode, "close-error") == 0)
    {
        fixture.failClose = 1;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "invalid-mode") == 0 || strcmp(mode, "negative-mode") == 0)
    {
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-source") == 0)
    {
        request.source = "ab\xc0\x80";
        expected = UMI_STATUS_PARSE_ERROR;
        untouched = 1;
    }
    if (strcmp(mode, "duplicate-source") == 0)
    {
        extra.document_uri = request.document_uri;
        expected = UMI_STATUS_ALREADY_EXISTS;
        untouched = 1;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel-before") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
        untouched = 1;
    }
    UmiLanguageRuntimeServer *server = Server(&fixture, request.root_uri);
    UmiLanguageCodeActionReport report;
    UmiLanguageCodeActionCatalogue *catalogue = NULL;
    CHECK(UmiLanguageCodeActionQueryOnServerWithContext(server, &request, extra_count ? &extra : NULL,
                                                        extra_count, delivery, cancel, &report,
                                                        &catalogue) == expected);
    const char *action = strstr(fixture.written, "\"method\":\"textDocument/codeAction\"");
    if (expected == UMI_STATUS_OK)
    {
        CHECK(catalogue != NULL && report.actions == 1U && action != NULL);
        if (delivery == UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE || strcmp(mode, "empty") == 0)
            CHECK(strstr(action, "\"diagnostics\":[]") != NULL);
        else
        {
            CHECK(strstr(action, "\"opaque\":\"same-session\"") != NULL &&
                  strstr(action, "\"code\":7") != NULL);
            CHECK((strstr(action, "other-range") != NULL) == (strcmp(mode, "point") == 0));
        }
        const char *pull = strstr(fixture.written, "\"method\":\"textDocument/diagnostic\"");
        CHECK((pull != NULL) == (delivery == UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL));
        if (pull != NULL)
        {
            CHECK(pull < action && strstr(fixture.written, "\"relatedDocumentSupport\":false") != NULL);
            if (extra_count != 0U)
            {
                CHECK(strstr(fixture.written, "other draft") != NULL);
                CHECK(strstr(fixture.written, "other draft") < pull);
            }
        }
        CHECK(report.close_status == UMI_STATUS_OK && report.shutdown_status == UMI_STATUS_OK);
    }
    else
        CHECK(catalogue == NULL);
    if (diagnostic_failure || initialize_failure || untouched)
        CHECK(action == NULL);
    if (untouched)
        CHECK(!report.started && fixture.stops == 0U && fixture.writeSize == 0U);
    else
        CHECK(fixture.stops == 1U);
    UmiLanguageCodeActionCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
