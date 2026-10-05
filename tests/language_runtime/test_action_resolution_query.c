/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_action_resolution_query.c
 * PURPOSE: Check bounded same-session text action resolution, immutable proposals and failure cleanup.
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
    UmiCancellationToken *resolve_cancel;
    unsigned resolve_writes;
    int fail_resolve_write, fail_resolve_read, timeout_resolve;
} Fixture;

static UmiStatus Write(void *instance, const void *bytes, size_t size)
{
    Fixture *fixture = instance;
    if (strstr((const char *)bytes, "\"method\":\"codeAction/resolve\"") != NULL)
    {
        ++fixture->resolve_writes;
        if (fixture->resolve_cancel != NULL)
            umi_cancellation_token_request(fixture->resolve_cancel);
        if (fixture->timeout_resolve)
            fixture->offset = fixture->size;
        if (fixture->fail_resolve_read)
            fixture->failRead = 1;
        if (fixture->fail_resolve_write)
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

#include "umicom/language_runtime/diagnostic_query.h"

#include "umicom/language_runtime/code_action_query.h"

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"maximum",
                           "over-maximum",
                           "valid",
                           "no-data",
                           "metadata",
                           "reordered",
                           "filter",
                           "notification",
                           "pull",
                           "sources",
                           "complete",
                           "disabled",
                           "command",
                           "empty",
                           "limit",
                           "exact-limit",
                           "zero-limit",
                           "high-limit",
                           "null-options",
                           "invalid-filter",
                           "invalid-mode",
                           "invalid-source",
                           "duplicate-source",
                           "cancel-before",
                           "cancel-resolve",
                           "timeout-resolve",
                           "read-error",
                           "write-error",
                           "close-error",
                           "shutdown-error",
                           "resolve-error",
                           "second-error",
                           "changed-title",
                           "changed-data",
                           "missing-edit",
                           "added-command",
                           "malformed",
                           "wrong-id",
                           "fragmented",
                           "unsupported",
                           "false-provider",
                           "malformed-provider",
                           "off"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    UmiLanguageCodeActionRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 0U, 3U, 2000U};
    UmiLanguageQuerySource extra = {"file:///workspace/other.c", "c", "other", 5U};
    UmiLanguageCodeActionQueryOptions options = {0};
    size_t limit = 32U;
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, initialize_failure = 0;
    if (strcmp(mode, "filter") == 0)
        options.filter = UMI_LANGUAGE_ACTION_FILTER_QUICK_FIX;
    if (strcmp(mode, "notification") == 0)
        options.diagnostics = UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION;
    if (strcmp(mode, "pull") == 0)
        options.diagnostics = UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL;
    if (strcmp(mode, "sources") == 0 || strcmp(mode, "duplicate-source") == 0)
    {
        options.additional_sources = &extra;
        options.source_count = 1U;
    }
    if (strcmp(mode, "duplicate-source") == 0)
    {
        extra.document_uri = request.document_uri;
        expected = UMI_STATUS_ALREADY_EXISTS;
        untouched = 1;
    }
    if (strcmp(mode, "zero-limit") == 0 || strcmp(mode, "high-limit") == 0)
    {
        limit = strcmp(mode, "zero-limit") == 0 ? 0U : 33U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "null-options") == 0)
    {
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-filter") == 0)
    {
        options.filter = (UmiLanguageCodeActionFilter)99;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-mode") == 0)
    {
        options.diagnostics = (UmiLanguageActionDiagnosticMode)99;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "invalid-source") == 0)
    {
        request.source = "a\xc0\x80";
        expected = UMI_STATUS_PARSE_ERROR;
        untouched = 1;
    }
    const char *provider = "{\"resolveProvider\":true}";
    if (strcmp(mode, "unsupported") == 0)
    {
        provider = "true";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "false-provider") == 0)
    {
        provider = "{\"resolveProvider\":false}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "malformed-provider") == 0)
    {
        provider = "{\"resolveProvider\":1}";
        expected = UMI_STATUS_PARSE_ERROR;
        initialize_failure = 1;
    }
    char response[8192], actions[4096];
    (void)snprintf(response, sizeof(response),
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{\"codeActionProvider\":%s,"
                   "\"textDocumentSync\":2,\"diagnosticProvider\":{\"interFileDependencies\":true,"
                   "\"workspaceDiagnostics\":false}}}}",
                   provider);
    Push(&fixture, response);
    if (options.diagnostics == UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION)
        Push(
            &fixture,
            "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\",\"params\":{\"uri\":\"file:/"
            "//workspace/main.c\",\"version\":1,\"diagnostics\":[]}}");
    if (options.diagnostics == UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":{\"kind\":\"full\",\"items\":[]}}");
    const char *data =
        strcmp(mode, "no-data") == 0 ? "" : ",\"data\":{\"session\":1,\"nested\":[true,\"opaque\"]}";
    const char *extra_metadata =
        strcmp(mode, "metadata") == 0 ? ",\"diagnostics\":[],\"extension\":{\"x\":1}" : "";
    const char *modifier = strcmp(mode, "complete") == 0   ? ",\"edit\":{}"
                           : strcmp(mode, "disabled") == 0 ? ",\"disabled\":{\"reason\":\"blocked\"}"
                           : strcmp(mode, "command") == 0
                               ? ",\"command\":{\"title\":\"Required\",\"command\":\"never-run\"}"
                               : "";
    int two =
        strcmp(mode, "second-error") == 0 || strcmp(mode, "limit") == 0 || strcmp(mode, "exact-limit") == 0;
    (void)snprintf(actions, sizeof(actions), "[{\"title\":\"Fix\",\"kind\":\"quickfix\"%s%s%s}%s]", data,
                   extra_metadata, modifier,
                   two ? ",{\"title\":\"Second\",\"kind\":\"quickfix\"}"
                   : strcmp(mode, "filter") == 0
                       ? ",{\"title\":\"Other family\",\"kind\":\"refactor\",\"data\":9}"
                       : "");
    size_t result_count = two ? 2U : 1U;
    unsigned resolves = two ? 2U : 1U;
    if (strcmp(mode, "complete") == 0 || strcmp(mode, "disabled") == 0 || strcmp(mode, "command") == 0 ||
        strcmp(mode, "off") == 0)
        resolves = 0U;
    if (strcmp(mode, "empty") == 0)
    {
        strcpy(actions, "[]");
        result_count = 0U;
        resolves = 0U;
    }
    if (strcmp(mode, "limit") == 0)
    {
        limit = 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        resolves = 0U;
    }
    if (strcmp(mode, "exact-limit") == 0)
        limit = 2U;

    if (strcmp(mode, "maximum") == 0 || strcmp(mode, "over-maximum") == 0)
    {
        /* Exercise the complete public ceiling, including refusal before the
         * first resolve when even one extra eligible action is present. */
        size_t wanted_count = strcmp(mode, "maximum") == 0 ? 32U : 33U;
        size_t used = strlen(actions) - 1U;
        for (size_t i = 1U; i < wanted_count; ++i)
        {
            int written = snprintf(actions + used, sizeof(actions) - used,
                                   ",{\"title\":\"Second\",\"kind\":\"quickfix\"}");
            CHECK(written > 0 && (size_t)written < sizeof(actions) - used);
            used += (size_t)written;
        }
        strcpy(actions + used, "]");
        result_count = wanted_count;
        resolves = strcmp(mode, "maximum") == 0 ? 32U : 0U;
        if (resolves == 0U)
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    unsigned action_id = options.diagnostics == UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL ? 3U : 2U;
    (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":%s}", action_id,
                   actions);
    Push(&fixture, response);
    for (unsigned i = 0U; i < resolves; ++i)
    {
        const char *title = i == 0U ? "Fix" : "Second", *item_data = i == 0U ? data : "",
                   *metadata = i == 0U ? extra_metadata : "", *edit = ",\"edit\":{}", *command = "";
        if (strcmp(mode, "changed-title") == 0)
        {
            title = "Changed";
            expected = UMI_STATUS_INVALID_STATE;
        }
        if (strcmp(mode, "changed-data") == 0)
        {
            item_data = ",\"data\":{\"session\":2}";
            expected = UMI_STATUS_INVALID_STATE;
        }
        if (strcmp(mode, "missing-edit") == 0)
        {
            edit = "";
            expected = UMI_STATUS_INVALID_STATE;
        }
        if (strcmp(mode, "added-command") == 0)
        {
            command = ",\"command\":{\"title\":\"Run\",\"command\":\"never\"}";
            expected = UMI_STATUS_INVALID_STATE;
        }
        if (strcmp(mode, "resolve-error") == 0 || (strcmp(mode, "second-error") == 0 && i == 1U))
        {
            (void)snprintf(
                response, sizeof(response),
                "{\"jsonrpc\":\"2.0\",\"id\":%u,\"error\":{\"code\":-32603,\"message\":\"refused\"}}",
                action_id + i + 1U);
            expected = UMI_STATUS_UNAVAILABLE;
        }
        else if (strcmp(mode, "malformed") == 0)
        {
            (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":null}",
                           action_id + i + 1U);
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "reordered") == 0)
        {
            (void)snprintf(response, sizeof(response),
                           "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":{\"edit\":{},\"kind\":\"quickfix\","
                           "\"title\":\"Fix\"%s}}",
                           action_id + i + 1U, data);
        }
        else
            (void)snprintf(
                response, sizeof(response),
                "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":{\"title\":\"%s\",\"kind\":\"quickfix\"%s%s%s%s}}",
                action_id + i + 1U, title, item_data, metadata, edit, command);
        if (strcmp(mode, "wrong-id") == 0)
            Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":999,\"result\":null}");
        Push(&fixture, response);
    }
    if (strcmp(mode, "shutdown-error") == 0)
    {
        (void)snprintf(response, sizeof(response),
                       "{\"jsonrpc\":\"2.0\",\"id\":%u,\"error\":{\"code\":-32603,\"message\":\"shutdown\"}}",
                       action_id + resolves + 1U);
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else
        (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":null}",
                       action_id + resolves + 1U);
    Push(&fixture, response);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel-before") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
        untouched = 1;
    }
    if (strcmp(mode, "cancel-resolve") == 0)
    {
        fixture.resolve_cancel = cancel;
        expected = UMI_STATUS_CANCELLED;
    }
    if (strcmp(mode, "timeout-resolve") == 0)
    {
        fixture.timeout_resolve = 1;
        request.timeout_ms = 5U;
        expected = UMI_STATUS_TIMEOUT;
    }
    if (strcmp(mode, "read-error") == 0)
    {
        fixture.fail_resolve_read = 1;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "write-error") == 0)
    {
        fixture.fail_resolve_write = 1;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "close-error") == 0)
    {
        fixture.failClose = 1;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "fragmented") == 0)
        fixture.fragment = 1U;
    UmiLanguageRuntimeServer *server = Server(&fixture, request.root_uri);
    UmiLanguageCodeActionReport report;
    UmiLanguageCodeActionCatalogue *catalogue = NULL;
    UmiStatus status = strcmp(mode, "off") == 0
                           ? UmiLanguageCodeActionQueryOnServerWithOptions(server, &request, &options, cancel,
                                                                           &report, &catalogue)
                           : UmiLanguageCodeActionQueryOnServerWithResolution(
                                 server, &request, strcmp(mode, "null-options") == 0 ? NULL : &options, limit,
                                 cancel, &report, &catalogue);
    CHECK(status == expected);
    if (untouched)
        CHECK(fixture.writeSize == 0U && fixture.stops == 0U && !report.started);
    else
    {
        CHECK(fixture.stops == 1U && report.started && report.initialized == !initialize_failure);
        CHECK(strstr(fixture.written, "workspace/executeCommand") == NULL);
        if (strcmp(mode, "off") == 0)
            CHECK(strstr(fixture.written, "resolveSupport") == NULL && fixture.resolve_writes == 0U);
        else
            CHECK(strstr(fixture.written, "\"resolveSupport\":{\"properties\":[\"edit\"]}") != NULL);
    }
    if (strcmp(mode, "limit") == 0 || strcmp(mode, "over-maximum") == 0 || initialize_failure)
        CHECK(fixture.resolve_writes == 0U);
    if (status == UMI_STATUS_OK)
    {
        CHECK(catalogue != NULL && report.actions == result_count &&
              UmiLanguageCodeActionCatalogueCount(catalogue) == result_count);
        CHECK(fixture.resolve_writes == resolves && report.close_status == UMI_STATUS_OK &&
              report.shutdown_status == UMI_STATUS_OK);
        if (resolves != 0U)
        {
            const char *resolution = strstr(fixture.written, "\"method\":\"codeAction/resolve\"");
            const char *closing = strstr(fixture.written, "textDocument/didClose");
            CHECK(resolution != NULL && closing != NULL && resolution < closing);
            if (data[0] != '\0')
                CHECK(strstr(resolution, "opaque") != NULL);
            UmiLanguageCodeAction action;
            CHECK(UmiLanguageCodeActionCatalogueAt(catalogue, 0U, &action) == UMI_STATUS_OK &&
                  action.has_edit && !action.has_command);
        }
        if (strcmp(mode, "filter") == 0)
            CHECK(strstr(fixture.written, "Other family") == NULL &&
                  strstr(fixture.written, "\"only\":[\"quickfix\"]") != NULL);
        if (strcmp(mode, "sources") == 0)
        {
            const char *opened = strstr(fixture.written, "other.c"),
                       *action = strstr(fixture.written, "textDocument/codeAction");
            CHECK(opened != NULL && action != NULL && opened < action);
        }
    }
    else
        CHECK(catalogue == NULL);
    if (strcmp(mode, "close-error") == 0 || strcmp(mode, "shutdown-error") == 0)
        CHECK(report.query_status == UMI_STATUS_OK);
    UmiLanguageCodeActionCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
