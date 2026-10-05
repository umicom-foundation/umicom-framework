/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_action_filter_query.c
 * PURPOSE: Check explicit action families, complete proposals, diagnostic context and cleanup ownership.
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
    const char *known[] = {"all",
                           "quickfix",
                           "refactor",
                           "imports",
                           "fix-all",
                           "descendants",
                           "prefix-boundary",
                           "missing-kind",
                           "empty-response",
                           "ignored-filter",
                           "notification",
                           "pull",
                           "no-diagnostics",
                           "primary-only",
                           "raw",
                           "disabled",
                           "command",
                           "deferred",
                           "malformed-unrelated",
                           "invalid-filter",
                           "negative-filter",
                           "null-options",
                           "invalid-mode",
                           "duplicate-source",
                           "invalid-source",
                           "cancel-before",
                           "close-error",
                           "shutdown-error",
                           "action-error",
                           "fragmented"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    UmiLanguageCodeActionRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abcd", 4U, 0U, 2U, 500U};
    UmiLanguageQuerySource extra = {"file:///workspace/other.c", "c", "other draft", 11U};
    UmiLanguageCodeActionQueryOptions options = {&extra, 1U, UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE,
                                                 UMI_LANGUAGE_ACTION_FILTER_ORGANIZE_IMPORTS};
    const char *kind = "source.organizeImports";
    if (strcmp(mode, "all") == 0)
    {
        options.filter = UMI_LANGUAGE_ACTION_FILTER_ALL;
        kind = "";
    }
    if (strcmp(mode, "quickfix") == 0)
    {
        options.filter = UMI_LANGUAGE_ACTION_FILTER_QUICK_FIX;
        kind = "quickfix";
    }
    if (strcmp(mode, "refactor") == 0)
    {
        options.filter = UMI_LANGUAGE_ACTION_FILTER_REFACTOR;
        kind = "refactor";
    }
    if (strcmp(mode, "fix-all") == 0)
    {
        options.filter = UMI_LANGUAGE_ACTION_FILTER_FIX_ALL;
        kind = "source.fixAll";
    }
    if (strcmp(mode, "notification") == 0)
        options.diagnostics = UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION;
    if (strcmp(mode, "pull") == 0)
        options.diagnostics = UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL;
    if (strcmp(mode, "primary-only") == 0)
    {
        options.additional_sources = NULL;
        options.source_count = 0U;
    }
    if (strcmp(mode, "fragmented") == 0)
        fixture.fragment = 1U;
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0;
    if (strcmp(mode, "invalid-filter") == 0)
    {
        options.filter = (UmiLanguageCodeActionFilter)99;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "negative-filter") == 0)
    {
        options.filter = (UmiLanguageCodeActionFilter)-1;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "null-options") == 0)
    {
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-mode") == 0)
    {
        options.diagnostics = (UmiLanguageActionDiagnosticMode)99;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "duplicate-source") == 0)
    {
        extra.document_uri = request.document_uri;
        expected = UMI_STATUS_ALREADY_EXISTS;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-source") == 0)
    {
        request.source = "ab\xc0\x80";
        expected = UMI_STATUS_PARSE_ERROR;
        untouched = 1;
    }
    Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{\"textDocumentSync\":2,"
                   "\"codeActionProvider\":true,\"diagnosticProvider\":{\"interFileDependencies\":true,"
                   "\"workspaceDiagnostics\":false}}}}");
    char response[4096], actions[3072];
    const char *diagnostics =
        "[{\"message\":\"first\",\"data\":{\"opaque\":\"same-session\"},\"range\":{\"start\":{\"line\":0,"
        "\"character\":0},\"end\":{\"line\":0,\"character\":2}}}]";
    if (options.diagnostics == UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION)
    {
        (void)snprintf(
            response, sizeof(response),
            "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\",\"params\":{\"uri\":\"file:/"
            "//workspace/main.c\",\"version\":1,\"diagnostics\":%s}}",
            diagnostics);
        Push(&fixture, response);
    }
    if (options.diagnostics == UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL)
    {
        (void)snprintf(response, sizeof(response),
                       "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":{\"kind\":\"full\",\"items\":%s}}",
                       diagnostics);
        Push(&fixture, response);
    }
    /* A server may ignore context.only. Include unrelated and unclassified
     * rows so the client must enforce the requested family before display. */
    const char *selected = strcmp(mode, "descendants") == 0 ? "source.organizeImports.cpp" : kind;
    const char *metadata = strcmp(mode, "disabled") == 0 ? ",\"disabled\":{\"reason\":\"not available\"}"
                           : strcmp(mode, "command") == 0
                               ? ",\"command\":{\"title\":\"Run\",\"command\":\"never execute\"}"
                               : "";
    const char *edit = strcmp(mode, "deferred") == 0 ? "" : ",\"edit\":{}";
    (void)snprintf(
        actions, sizeof(actions),
        "[{\"title\":\"Unrelated\",\"kind\":\"source.other\",\"edit\":{}},{\"title\":\"Chosen\",\"kind\":\"%"
        "s\",\"data\":{\"opaque\":\"complete action\"}%s%s},{\"title\":\"Unclassified\",\"edit\":{}}]",
        selected, edit, metadata);
    size_t count = options.filter == UMI_LANGUAGE_ACTION_FILTER_ALL ? 3U : 1U;
    if (strcmp(mode, "prefix-boundary") == 0)
    {
        strcpy(actions, "[{\"title\":\"Prefix only\",\"kind\":\"source.organizeImportsOther\",\"edit\":{}}]");
        count = 0U;
    }
    if (strcmp(mode, "missing-kind") == 0)
    {
        strcpy(actions, "[{\"title\":\"Unclassified\",\"edit\":{}}]");
        count = 0U;
    }
    if (strcmp(mode, "ignored-filter") == 0)
    {
        strcpy(actions, "[{\"title\":\"Other\",\"kind\":\"quickfix\",\"edit\":{}}]");
        count = 0U;
    }
    if (strcmp(mode, "empty-response") == 0)
    {
        strcpy(actions, "[]");
        count = 0U;
    }
    if (strcmp(mode, "malformed-unrelated") == 0)
    {
        strcpy(actions, "[{\"title\":\"Chosen\",\"kind\":\"source.organizeImports\",\"edit\":{}},{\"title\":"
                        "false,\"kind\":\"quickfix\"}]");
        expected = UMI_STATUS_PARSE_ERROR;
    }
    unsigned action_id = options.diagnostics == UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL ? 3U : 2U;
    if (strcmp(mode, "action-error") == 0)
    {
        (void)snprintf(response, sizeof(response),
                       "{\"jsonrpc\":\"2.0\",\"id\":%u,\"error\":{\"code\":-32603,\"message\":\"action\"}}",
                       action_id);
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else
        (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":%s}", action_id,
                       actions);
    Push(&fixture, response);
    if (strcmp(mode, "shutdown-error") == 0)
    {
        (void)snprintf(response, sizeof(response),
                       "{\"jsonrpc\":\"2.0\",\"id\":%u,\"error\":{\"code\":-32603,\"message\":\"shutdown\"}}",
                       action_id + 1U);
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else
        (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":null}",
                       action_id + 1U);
    Push(&fixture, response);
    if (strcmp(mode, "close-error") == 0)
    {
        fixture.failClose = 1;
        expected = UMI_STATUS_IO_ERROR;
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
    CHECK(UmiLanguageCodeActionQueryOnServerWithOptions(server, &request,
                                                        strcmp(mode, "null-options") == 0 ? NULL : &options,
                                                        cancel, &report, &catalogue) == expected);
    const char *action = strstr(fixture.written, "\"method\":\"textDocument/codeAction\"");
    if (expected == UMI_STATUS_OK)
    {
        CHECK(catalogue != NULL && report.actions == count &&
              UmiLanguageCodeActionCatalogueCount(catalogue) == count && action != NULL);
        if (options.filter == UMI_LANGUAGE_ACTION_FILTER_ALL)
            CHECK(strstr(action, "\"only\"") == NULL);
        else
        {
            char only[96];
            (void)snprintf(only, sizeof(only), "\"only\":[\"%s\"]", kind);
            CHECK(strstr(action, only) != NULL);
        }
        if (options.diagnostics == UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE)
            CHECK(strstr(action, "\"diagnostics\":[]") != NULL);
        else
            CHECK(strstr(action, "same-session") != NULL);
        if (options.source_count != 0U)
            CHECK(strstr(fixture.written, "other draft") < action);
        if (count == 1U)
        {
            UmiLanguageCodeAction item;
            CHECK(UmiLanguageCodeActionCatalogueAt(catalogue, 0U, &item) == UMI_STATUS_OK);
            CHECK(strcmp(item.title, "Chosen") == 0 && strcmp(item.kind, selected) == 0);
            const char *raw = NULL;
            size_t bytes = 0U;
            CHECK(UmiLanguageCodeActionCatalogueItemJson(catalogue, 0U, &raw, &bytes) == UMI_STATUS_OK);
            CHECK(bytes != 0U && bytes < 1024U);
            char copy[1024];
            memcpy(copy, raw, bytes);
            copy[bytes] = '\0';
            CHECK(strstr(copy, "complete action") != NULL);
            UmiLanguageWorkspaceEditCatalogue *edits = NULL;
            UmiStatus can_read = strcmp(mode, "disabled") == 0 ? UMI_STATUS_PERMISSION_DENIED
                                 : strcmp(mode, "command") == 0 || strcmp(mode, "deferred") == 0
                                     ? UMI_STATUS_NOT_IMPLEMENTED
                                     : UMI_STATUS_OK;
            CHECK(UmiLanguageCodeActionCatalogueReadEdits(catalogue, 0U, NULL, &edits) == can_read);
            UmiLanguageWorkspaceEditCatalogueDestroy(edits);
        }
        CHECK(report.close_status == UMI_STATUS_OK && report.shutdown_status == UMI_STATUS_OK);
    }
    else
        CHECK(catalogue == NULL);
    if (untouched)
        CHECK(action == NULL && !report.started && fixture.stops == 0U && fixture.writeSize == 0U);
    else
        CHECK(fixture.stops == 1U);
    UmiLanguageCodeActionCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
