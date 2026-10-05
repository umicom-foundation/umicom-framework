/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_action_sources.c
 * PURPOSE: Check complete-source code actions, same-session diagnostics and whole-result cleanup.
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
    unsigned open_attempts, close_attempts, fail_open_at, fail_close_at;
    UmiCancellationToken *cancel;
} Fixture;

static UmiStatus Write(void *instance, const void *bytes, size_t size)
{
    Fixture *fixture = instance;
    if (strstr((const char *)bytes, "textDocument/didOpen") != NULL)
    {
        ++fixture->open_attempts;
        if (fixture->open_attempts == fixture->fail_open_at)
            return UMI_STATUS_IO_ERROR;
    }
    if (strstr((const char *)bytes, "textDocument/didClose") != NULL)
    {
        ++fixture->close_attempts;
        if (fixture->close_attempts == fixture->fail_close_at)
            return UMI_STATUS_IO_ERROR;
    }
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
    const char *cases[] = {"valid",          "diagnostics",      "secondary-diagnostics",
                           "no-additional",  "duplicate",        "missing",
                           "invalid-option", "cancel-before",    "fragmented",
                           "open-failure",   "close-failure",    "action-error",
                           "shutdown-error", "malformed-action", "command",
                           "disabled",       "deferred",         "annotated"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    Fixture fixture = {0};
    UmiLanguageCodeActionRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "source main", 11U, 0U, 6U, 100U};
    UmiLanguageQuerySource other[] = {{"file:///workspace/second.c", "c", "source second", 13U},
                                      {"file:///workspace/third.c", "c", "source third", 12U}};
    size_t count = 2U;
    const UmiLanguageQuerySource *input = other;
    int diagnostics = strcmp(mode, "diagnostics") == 0 || strcmp(mode, "secondary-diagnostics") == 0;
    UmiStatus expected = UMI_STATUS_OK;
    int preflight = 0;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "no-additional") == 0)
    {
        count = 0U;
        input = NULL;
    }
    if (strcmp(mode, "duplicate") == 0)
    {
        other[1].document_uri = other[0].document_uri;
        expected = UMI_STATUS_ALREADY_EXISTS;
        preflight = 1;
    }
    if (strcmp(mode, "missing") == 0)
    {
        input = NULL;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        preflight = 1;
    }
    if (strcmp(mode, "invalid-option") == 0)
    {
        diagnostics = 2;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        preflight = 1;
    }
    if (strcmp(mode, "cancel-before") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
        preflight = 1;
    }
    if (strcmp(mode, "fragmented") == 0)
        fixture.fragment = 3U;
    if (strcmp(mode, "open-failure") == 0)
    {
        fixture.fail_open_at = 3U;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "close-failure") == 0)
    {
        fixture.fail_close_at = 2U;
        expected = UMI_STATUS_IO_ERROR;
    }
    Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{\"textDocumentSync\":{"
                   "\"openClose\":true,\"change\":1},\"codeActionProvider\":true}}}");
    if (strcmp(mode, "secondary-diagnostics") == 0)
        Push(
            &fixture,
            "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\",\"params\":{\"uri\":\"file:/"
            "//workspace/second.c\",\"version\":1,\"diagnostics\":[{\"message\":\"not the primary "
            "context\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":6}"
            "}}]}}");
    if (diagnostics == 1)
        Push(
            &fixture,
            "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\",\"params\":{\"uri\":\"file:/"
            "//workspace/main.c\",\"version\":1,\"diagnostics\":[{\"message\":\"fix this "
            "name\",\"data\":{\"opaque\":\"same "
            "connection\"},\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
            "\"character\":6}}}]}}");
    const char *action = "{\"title\":\"Fix "
                         "name\",\"edit\":{\"changes\":{\"file:///workspace/"
                         "second.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                         "\"character\":6}},\"newText\":\"fixed\"}]}}}";
    if (strcmp(mode, "command") == 0)
        action = "{\"title\":\"Run command\",\"command\":{\"title\":\"Run\",\"command\":\"do not "
                 "execute\"},\"edit\":{}}";
    if (strcmp(mode, "disabled") == 0)
        action = "{\"title\":\"Unavailable\",\"disabled\":{\"reason\":\"not here\"},\"edit\":{}}";
    if (strcmp(mode, "deferred") == 0)
        action = "{\"title\":\"Resolve later\",\"data\":{\"opaque\":1}}";
    if (strcmp(mode, "malformed-action") == 0)
    {
        action = "{\"title\":7}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "annotated") == 0)
        action =
            "{\"title\":\"Explain "
            "edit\",\"edit\":{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///workspace/"
            "second.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":"
            "{\"line\":0,\"character\":6}},\"newText\":\"fixed\",\"annotationId\":\"reason\"}]}],"
            "\"changeAnnotations\":{\"reason\":{\"label\":\"Review this\",\"needsConfirmation\":true}}}}";
    char response[4096];
    unsigned shutdown_id = 3U;
    if (strcmp(mode, "open-failure") == 0)
        shutdown_id = 2U;
    else
    {
        if (strcmp(mode, "action-error") == 0)
        {
            (void)snprintf(response, sizeof(response),
                           "{\"jsonrpc\":\"2.0\",\"id\":2,\"error\":{\"code\":-1,\"message\":\"refused\"}}");
            expected = UMI_STATUS_UNAVAILABLE;
        }
        else
            (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":[%s]}",
                           action);
        Push(&fixture, response);
    }
    if (strcmp(mode, "shutdown-error") == 0)
    {
        (void)snprintf(
            response, sizeof(response),
            "{\"jsonrpc\":\"2.0\",\"id\":%u,\"error\":{\"code\":-1,\"message\":\"shutdown refused\"}}",
            shutdown_id);
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else
        (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":null}",
                       shutdown_id);
    Push(&fixture, response);
    UmiLanguageRuntimeServer *server = Server(&fixture, request.root_uri);
    UmiLanguageCodeActionReport report;
    UmiLanguageCodeActionCatalogue *catalogue = NULL;
    UmiStatus actual = UmiLanguageCodeActionQueryOnServerWithSources(
        server, &request, input, count, diagnostics, cancel, &report, &catalogue);
    if (actual != expected)
        fprintf(stderr, "%s: %d expected %d\n", mode, (int)actual, (int)expected);
    CHECK(actual == expected);
    if (preflight)
        CHECK(fixture.writeSize == 0U && fixture.stops == 0U && !report.started);
    else
    {
        CHECK(fixture.close_attempts == (strcmp(mode, "open-failure") == 0 ? 2U : (unsigned)(count + 1U)) &&
              fixture.stops != 0U);
        const char *query = strstr(fixture.written, "\"method\":\"textDocument/codeAction\"");
        if (strcmp(mode, "open-failure") == 0)
            CHECK(query == NULL);
        else
        {
            CHECK(query != NULL);
            if (count != 0U)
            {
                const char *last_source = strstr(fixture.written, "source third");
                CHECK(last_source != NULL && last_source < query);
            }
            if (diagnostics)
                CHECK(strstr(query, "same connection") != NULL &&
                      strstr(query, "not the primary context") == NULL);
            else
                CHECK(strstr(query, "\"diagnostics\":[]") != NULL);
        }
    }
    if (expected == UMI_STATUS_OK)
    {
        CHECK(report.actions == 1U && UmiLanguageCodeActionCatalogueCount(catalogue) == 1U);
        UmiLanguageWorkspaceEditCatalogue *edits = NULL;
        UmiStatus wanted = strcmp(mode, "disabled") == 0 ? UMI_STATUS_PERMISSION_DENIED
                           : strcmp(mode, "command") == 0 || strcmp(mode, "deferred") == 0
                               ? UMI_STATUS_NOT_IMPLEMENTED
                               : UMI_STATUS_OK;
        CHECK(UmiLanguageCodeActionCatalogueReadEdits(catalogue, 0U, NULL, &edits) == wanted);
        if (wanted == UMI_STATUS_OK)
        {
            CHECK(UmiLanguageWorkspaceEditCatalogueCount(edits) == 1U);
            UmiLanguageWorkspaceDocumentChange document;
            CHECK(UmiLanguageWorkspaceEditCatalogueDocument(edits, 0U, &document) == UMI_STATUS_OK &&
                  strcmp(document.uri, "file:///workspace/second.c") == 0);
            if (strcmp(mode, "annotated") == 0)
                CHECK(UmiLanguageWorkspaceEditCatalogueAnnotationCount(edits) == 1U);
        }
        else
            CHECK(edits == NULL);
        UmiLanguageWorkspaceEditCatalogueDestroy(edits);
    }
    else
        CHECK(catalogue == NULL);
    UmiLanguageCodeActionCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
