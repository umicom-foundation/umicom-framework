/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_rename_query.c
 * PURPOSE: Check rename negotiation, exact positions and cleanup before publication.
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

#include "umicom/language_runtime/rename_query.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"valid",
                           "object",
                           "missing",
                           "disabled",
                           "encoding",
                           "sync",
                           "malformed-prepare-option",
                           "prepare-disabled",
                           "fragmented",
                           "notification",
                           "error",
                           "shutdown-error",
                           "close-error",
                           "timeout",
                           "cancel-before",
                           "cancel-during",
                           "invalid-source",
                           "invalid-caret",
                           "state",
                           "unicode",
                           "native-invalid",
                           "read-error",
                           "write-error",
                           "escaped-limit",
                           "name-null",
                           "name-empty",
                           "name-control",
                           "name-utf8",
                           "name-capacity",
                           "name-unicode",
                           "name-escaped",
                           "wrong-id",
                           "prepare-error",
                           "prepare-range",
                           "prepare-placeholder",
                           "prepare-null",
                           "prepare-default",
                           "prepare-outside",
                           "prepare-reversed",
                           "prepare-away",
                           "prepare-end",
                           "prepare-empty-range",
                           "prepare-missing-placeholder",
                           "prepare-invalid-placeholder",
                           "prepare-stray-placeholder",
                           "prepare-invalid-point",
                           "prepare-surrogate",
                           "prepare-unicode",
                           "prepare-wrong-id",
                           "empty",
                           "versioned",
                           "unversioned",
                           "stale-version",
                           "annotations",
                           "overlap",
                           "invalid-content",
                           "result-range-outside",
                           "resource-operation",
                           "external-document",
                           "multiple-documents",
                           "missing-annotation"};

    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    UmiLanguageRenameRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 1U, 500U, "total"};
    UmiStatus expected = UMI_STATUS_OK;
    int untouched = 0, initialize_failure = 0, prepare = 0, prepare_failed = 0;
    size_t count = 1U;
    const char *caps = "{\"renameProvider\":true,\"textDocumentSync\":2}";
    const char *preparation = NULL;
    const char *proposal = "{\"changes\":{\"file:///workspace/"
                           "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                           "\"character\":3}},\"newText\":\"total\"}]}}";

    if (strcmp(mode, "object") == 0)
        caps = "{\"renameProvider\":{},\"textDocumentSync\":{\"openClose\":true}}";
    if (strcmp(mode, "prepare-disabled") == 0)
        caps = "{\"renameProvider\":{\"prepareProvider\":false},\"textDocumentSync\":2}";
    if (strcmp(mode, "missing") == 0)
    {
        caps = "{\"textDocumentSync\":2}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "disabled") == 0)
    {
        caps = "{\"renameProvider\":false,\"textDocumentSync\":2}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "encoding") == 0)
    {
        caps = "{\"renameProvider\":true,\"textDocumentSync\":2,\"positionEncoding\":\"utf-8\"}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "sync") == 0)
    {
        caps = "{\"renameProvider\":true,\"textDocumentSync\":{\"openClose\":false}}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        initialize_failure = 1;
    }
    if (strcmp(mode, "malformed-prepare-option") == 0)
    {
        caps = "{\"renameProvider\":{\"prepareProvider\":\"true\"},\"textDocumentSync\":2}";
        expected = UMI_STATUS_PARSE_ERROR;
        initialize_failure = 1;
    }
    /* Each preparation shape exercises a distinct decision before the rename
     * request is allowed onto the wire. Failure must still close the draft. */
    if (strcmp(mode, "prepare-range") == 0)
    {
        prepare = 1;
        preparation = "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}}";
        expected = UMI_STATUS_OK;
        prepare_failed = 0;
    }
    if (strcmp(mode, "prepare-placeholder") == 0)
    {
        prepare = 1;
        preparation = "{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":"
                      "3}},\"placeholder\":\"abc\"}";
        expected = UMI_STATUS_OK;
        prepare_failed = 0;
    }
    if (strcmp(mode, "prepare-null") == 0)
    {
        prepare = 1;
        preparation = "null";
        expected = UMI_STATUS_NOT_FOUND;
        prepare_failed = 1;
    }
    if (strcmp(mode, "prepare-default") == 0)
    {
        prepare = 1;
        preparation = "{\"defaultBehavior\":true}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        prepare_failed = 1;
    }
    if (strcmp(mode, "prepare-outside") == 0)
    {
        prepare = 1;
        preparation = "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":99}}";
        expected = UMI_STATUS_INVALID_ARGUMENT;
        prepare_failed = 1;
    }
    if (strcmp(mode, "prepare-reversed") == 0)
    {
        prepare = 1;
        preparation = "{\"start\":{\"line\":0,\"character\":3},\"end\":{\"line\":0,\"character\":0}}";
        expected = UMI_STATUS_PARSE_ERROR;
        prepare_failed = 1;
    }
    if (strcmp(mode, "prepare-away") == 0)
    {
        prepare = 1;
        preparation = "{\"start\":{\"line\":0,\"character\":2},\"end\":{\"line\":0,\"character\":3}}";
        expected = UMI_STATUS_PARSE_ERROR;
        prepare_failed = 1;
    }
    if (strcmp(mode, "prepare-end") == 0)
    {
        prepare = 1;
        preparation = "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":1}}";
        expected = UMI_STATUS_OK;
        prepare_failed = 0;
    }
    if (strcmp(mode, "prepare-empty-range") == 0)
    {
        prepare = 1;
        preparation = "{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":1}}";
        expected = UMI_STATUS_OK;
        prepare_failed = 0;
    }
    if (strcmp(mode, "prepare-missing-placeholder") == 0)
    {
        prepare = 1;
        preparation =
            "{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}}}";
        expected = UMI_STATUS_PARSE_ERROR;
        prepare_failed = 1;
    }
    if (strcmp(mode, "prepare-invalid-placeholder") == 0)
    {
        prepare = 1;
        preparation = "{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":"
                      "3}},\"placeholder\":7}";
        expected = UMI_STATUS_PARSE_ERROR;
        prepare_failed = 1;
    }
    if (strcmp(mode, "prepare-stray-placeholder") == 0)
    {
        prepare = 1;
        preparation = "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3},"
                      "\"placeholder\":\"abc\"}";
        expected = UMI_STATUS_PARSE_ERROR;
        prepare_failed = 1;
    }
    if (strcmp(mode, "prepare-invalid-point") == 0)
    {
        prepare = 1;
        preparation = "{\"start\":{\"line\":0,\"character\":1.5},\"end\":{\"line\":0,\"character\":3}}";
        expected = UMI_STATUS_PARSE_ERROR;
        prepare_failed = 1;
    }
    if (strcmp(mode, "prepare-surrogate") == 0)
    {
        prepare = 1;
        preparation = "{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":2}}";
        expected = UMI_STATUS_INVALID_ARGUMENT;
        prepare_failed = 1;
    }
    if (strcmp(mode, "prepare-unicode") == 0)
    {
        prepare = 1;
        preparation = "{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}}";
        expected = UMI_STATUS_OK;
        prepare_failed = 0;
    }
    if (strcmp(mode, "prepare-wrong-id") == 0)
    {
        prepare = 1;
        preparation = "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}}";
        expected = UMI_STATUS_OK;
        prepare_failed = 0;
    }
    if (strcmp(mode, "empty") == 0)
    {
        proposal = "null";
        expected = UMI_STATUS_OK;
        count = 0U;
    }
    if (strcmp(mode, "versioned") == 0)
    {
        proposal = "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///workspace/"
                   "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},"
                   "\"end\":{\"line\":0,\"character\":3}},\"newText\":\"total\"}]}]}";
        expected = UMI_STATUS_OK;
        count = 1U;
    }
    if (strcmp(mode, "unversioned") == 0)
    {
        proposal = "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///workspace/"
                   "main.c\",\"version\":null},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},"
                   "\"end\":{\"line\":0,\"character\":3}},\"newText\":\"total\"}]}]}";
        expected = UMI_STATUS_OK;
        count = 1U;
    }
    if (strcmp(mode, "stale-version") == 0)
    {
        proposal = "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///workspace/"
                   "main.c\",\"version\":2},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},"
                   "\"end\":{\"line\":0,\"character\":3}},\"newText\":\"total\"}]}]}";
        expected = UMI_STATUS_INVALID_STATE;
        count = 1U;
    }
    if (strcmp(mode, "annotations") == 0)
    {
        proposal = "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///workspace/"
                   "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},"
                   "\"end\":{\"line\":0,\"character\":3}},\"newText\":\"total\",\"annotationId\":\"reason\"}]"
                   "}],\"changeAnnotations\":{\"reason\":{\"label\":\"Rename "
                   "references\",\"description\":\"Update this document.\",\"needsConfirmation\":true}}}";
        expected = UMI_STATUS_OK;
        count = 1U;
    }
    /* Overlapping changes are a conflicting edit set. The shared preview
     * reports INVALID_STATE; retain the former expectation for review. */
#if 0
    if(strcmp(mode,"overlap")==0) {proposal="{\"changes\":{\"file:///workspace/main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}},\"newText\":\"total\"},{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,\"character\":3}},\"newText\":\"total\"}]}}";expected=UMI_STATUS_INVALID_ARGUMENT;count=1U;}
#endif
    if (strcmp(mode, "overlap") == 0)
    {
        proposal = "{\"changes\":{\"file:///workspace/"
                   "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                   "\"character\":2}},\"newText\":\"total\"},{\"range\":{\"start\":{\"line\":0,\"character\":"
                   "1},\"end\":{\"line\":0,\"character\":3}},\"newText\":\"total\"}]}}";
        expected = UMI_STATUS_INVALID_STATE;
        count = 1U;
    }
    if (strcmp(mode, "invalid-content") == 0)
    {
        proposal = "{\"changes\":7}";
        expected = UMI_STATUS_PARSE_ERROR;
        count = 0U;
    }
    if (strcmp(mode, "result-range-outside") == 0)
    {
        proposal = "{\"changes\":{\"file:///workspace/"
                   "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                   "\"character\":99}},\"newText\":\"total\"}]}}";
        expected = UMI_STATUS_INVALID_ARGUMENT;
        count = 1U;
    }
    if (strcmp(mode, "resource-operation") == 0)
    {
        proposal = "{\"documentChanges\":[{\"kind\":\"create\",\"uri\":\"file:///workspace/other.c\"}]}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
        count = 0U;
    }
    if (strcmp(mode, "external-document") == 0)
    {
        proposal = "{\"changes\":{\"file:///workspace/"
                   "other.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                   "\"character\":99}},\"newText\":\"total\"}]}}";
        expected = UMI_STATUS_OK;
        count = 1U;
    }
    if (strcmp(mode, "multiple-documents") == 0)
    {
        proposal = "{\"changes\":{\"file:///workspace/"
                   "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                   "\"character\":3}},\"newText\":\"total\"}],\"file:///workspace/"
                   "other.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                   "\"character\":3}},\"newText\":\"total\"}]}}";
        expected = UMI_STATUS_OK;
        count = 2U;
    }
    if (strcmp(mode, "missing-annotation") == 0)
    {
        proposal =
            "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"file:///workspace/"
            "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
            "\"line\":0,\"character\":3}},\"newText\":\"total\",\"annotationId\":\"missing\"}]}]}";
        expected = UMI_STATUS_NOT_FOUND;
        count = 0U;
    }

    if (strcmp(mode, "prepare-error") == 0)
    {
        prepare = 1;
        prepare_failed = 1;
        expected = UMI_STATUS_UNAVAILABLE;
    }
    if (prepare)
        caps = "{\"renameProvider\":{\"prepareProvider\":true},\"textDocumentSync\":2}";
    if (strcmp(mode, "unicode") == 0 || strcmp(mode, "prepare-surrogate") == 0 ||
        strcmp(mode, "prepare-unicode") == 0)
    {
        request.source = "a\xf0\x9f\x8c\x8d"
                         "c";
        request.source_bytes = 6U;
        request.caret = 5U;
    }
    if (strcmp(mode, "notification") == 0)
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"method\":\"window/"
                       "logMessage\",\"params\":{\"type\":3,\"message\":\"working\"}}");
    char response[4096];
    (void)snprintf(response, sizeof(response),
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":%s}}", caps);
    Push(&fixture, response);
    unsigned query_id = 2U;
    if (prepare)
    {
        if (strcmp(mode, "prepare-wrong-id") == 0)
            Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":99,\"result\":null}");
        if (strcmp(mode, "prepare-error") == 0)
            Push(&fixture,
                 "{\"jsonrpc\":\"2.0\",\"id\":2,\"error\":{\"code\":-1,\"message\":\"not renamable\"}}");
        else
        {
            (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":%s}",
                           preparation);
            Push(&fixture, response);
        }
        query_id = 3U;
    }
    if (!prepare_failed)
    {
        if (strcmp(mode, "wrong-id") == 0)
            Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":99,\"result\":null}");
        if (strcmp(mode, "error") == 0)
        {
            (void)snprintf(response, sizeof(response),
                           "{\"jsonrpc\":\"2.0\",\"id\":%u,\"error\":{\"code\":-1,\"message\":\"refused\"}}",
                           query_id);
            expected = UMI_STATUS_UNAVAILABLE;
        }
        else
            (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":%s}",
                           query_id, proposal);
        Push(&fixture, response);
    }
    unsigned shutdown_id = prepare_failed ? query_id : query_id + 1U;
    if (strcmp(mode, "shutdown-error") == 0)
    {
        (void)snprintf(response, sizeof(response),
                       "{\"jsonrpc\":\"2.0\",\"id\":%u,\"error\":{\"code\":-1,\"message\":\"shutdown\"}}",
                       shutdown_id);
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else
        (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":null}",
                       shutdown_id);
    Push(&fixture, response);
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
    if (strcmp(mode, "name-null") == 0)
    {
        request.new_name = NULL;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "name-empty") == 0)
    {
        request.new_name = "";
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "name-control") == 0)
    {
        request.new_name = "a\nb";
        expected = UMI_STATUS_INVALID_ARGUMENT;
        untouched = 1;
    }
    if (strcmp(mode, "name-utf8") == 0)
    {
        request.new_name = "\xc0\x80";
        expected = UMI_STATUS_PARSE_ERROR;
        untouched = 1;
    }
    if (strcmp(mode, "name-unicode") == 0)
        request.new_name = "caf\xc3\xa9";
    if (strcmp(mode, "name-escaped") == 0)
        request.new_name = "operator\"\\";
    char name[1026];
    memset(name, 'a', sizeof(name));
    name[sizeof(name) - 1U] = '\0';
    if (strcmp(mode, "name-capacity") == 0)
    {
        request.new_name = name;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
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
    UmiLanguageRenameReport report;
    UmiLanguageWorkspaceEditCatalogue *catalogue = NULL;
    UmiStatus status;
    if (strcmp(mode, "native-invalid") == 0)
    {
        UmiLanguageServerProfile profile = {0};
        strcpy(profile.id, "invalid");
        profile.enabled = 1;
        status = UmiLanguageRenameQueryNative(&profile, NULL, &request, cancel, &report, &catalogue);
        CHECK(status != UMI_STATUS_OK && !report.started && catalogue == NULL);
        untouched = 1;
    }
    else
    {
        status = UmiLanguageRenameQueryOnServer(server, &request, cancel, &report, &catalogue);
        if (status != expected)
            fprintf(stderr, "mode %s: status %d expected %d\n", mode, (int)status, (int)expected);
        CHECK(status == expected);
        if (status == UMI_STATUS_OK)
        {
            CHECK(UmiLanguageWorkspaceEditCatalogueCount(catalogue) == count && report.documents == count);
            CHECK(strstr(fixture.written, "\"prepareSupport\":true") != NULL);
            CHECK(strstr(fixture.written, "\"resourceOperations\":[]") != NULL);
            CHECK(strstr(fixture.written, "\"honorsChangeAnnotations\":true") != NULL);
            CHECK(strstr(fixture.written, "\"method\":\"textDocument/rename\"") != NULL);
            CHECK((strstr(fixture.written, "\"method\":\"textDocument/prepareRename\"") != NULL) == prepare);
            CHECK(strstr(fixture.written, "textDocument/completion") == NULL);
            CHECK(report.initialized && report.document_opened && report.close_status == UMI_STATUS_OK &&
                  report.shutdown_status == UMI_STATUS_OK);
            if (strcmp(mode, "annotations") == 0)
            {
                UmiLanguageWorkspaceChangeAnnotation annotation;
                CHECK(UmiLanguageWorkspaceEditCatalogueAnnotationCount(catalogue) == 1U);
                CHECK(UmiLanguageWorkspaceEditCatalogueAnnotation(catalogue, 0U, &annotation) ==
                          UMI_STATUS_OK &&
                      annotation.needs_confirmation);
            }
            if (strcmp(mode, "external-document") == 0)
            {
                UmiLanguageWorkspaceDocumentChange change;
                CHECK(UmiLanguageWorkspaceEditCatalogueDocument(catalogue, 0U, &change) == UMI_STATUS_OK);
                CHECK(strcmp(change.uri, "file:///workspace/other.c") == 0);
            }
            if (strcmp(mode, "name-unicode") == 0)
                CHECK(strstr(fixture.written, "\"newName\":\"caf\xc3\xa9\"") != NULL);
            if (strcmp(mode, "name-escaped") == 0)
                CHECK(strstr(fixture.written, "\"newName\":\"operator\\\"\\\\\"") != NULL);
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
    if (prepare_failed)
    {
        CHECK(strstr(fixture.written, "\"method\":\"textDocument/rename\"") == NULL);
        CHECK(strstr(fixture.written, "textDocument/didClose") != NULL);
        CHECK(report.query_status == expected);
    }
    if (strcmp(mode, "shutdown-error") == 0 || strcmp(mode, "close-error") == 0)
        CHECK(report.query_status == UMI_STATUS_OK);
    UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    free(large);
    return 0;
}
