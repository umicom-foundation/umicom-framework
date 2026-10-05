/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_rename_sources.c
 * PURPOSE: Check multi-source rename preflight, exact additional drafts and balanced cleanup.
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

#include "umicom/language_runtime/rename_query.h"

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"valid",
                           "no-additional",
                           "two-additional",
                           "duplicate-primary",
                           "duplicate-additional",
                           "missing-array",
                           "missing-uri",
                           "missing-language",
                           "missing-source",
                           "empty-uri",
                           "invalid-source",
                           "embedded-zero",
                           "invalid-uri",
                           "source-limit",
                           "escaped-limit",
                           "too-many",
                           "cancel-before",
                           "fragmented",
                           "versioned",
                           "stale-version",
                           "outside",
                           "overlap",
                           "unknown-target",
                           "prepare",
                           "open-failure",
                           "primary-close-failure",
                           "additional-close-failure",
                           "query-error",
                           "shutdown-error"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture fixture = {0};
    UmiLanguageRuntimeServer *server = Server(&fixture, "file:///workspace");
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    UmiLanguageRenameRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "source", 6U, 1U, 1000U, "renamed"};
    UmiLanguageQuerySource sources[2] = {{"file:///workspace/second.c", "c", "source second", 13U},
                                         {"file:///workspace/third.c", "c", "source third", 12U}};
    const UmiLanguageQuerySource *input = sources;
    size_t count = 1U;
    UmiStatus expected = UMI_STATUS_OK;
    int preflight = 0;
    char *large = NULL;
    if (strcmp(mode, "no-additional") == 0)
        count = 0U;
    if (strcmp(mode, "two-additional") == 0)
        count = 2U;
    if (strcmp(mode, "duplicate-primary") == 0)
    {
        sources[0].document_uri = request.document_uri;
        expected = UMI_STATUS_ALREADY_EXISTS;
        preflight = 1;
    }
    if (strcmp(mode, "duplicate-additional") == 0)
    {
        sources[1].document_uri = sources[0].document_uri;
        count = 2U;
        expected = UMI_STATUS_ALREADY_EXISTS;
        preflight = 1;
    }
    if (strcmp(mode, "missing-array") == 0)
    {
        input = NULL;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        preflight = 1;
    }
    if (strcmp(mode, "missing-uri") == 0)
    {
        sources[0].document_uri = NULL;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        preflight = 1;
    }
    if (strcmp(mode, "missing-language") == 0)
    {
        sources[0].language_id = NULL;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        preflight = 1;
    }
    if (strcmp(mode, "missing-source") == 0)
    {
        sources[0].source = NULL;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        preflight = 1;
    }
    if (strcmp(mode, "empty-uri") == 0)
    {
        sources[0].document_uri = "";
        expected = UMI_STATUS_INVALID_ARGUMENT;
        preflight = 1;
    }
    if (strcmp(mode, "invalid-source") == 0)
    {
        sources[0].source = "bad\xff";
        sources[0].source_bytes = 4U;
        expected = UMI_STATUS_PARSE_ERROR;
        preflight = 1;
    }
    if (strcmp(mode, "embedded-zero") == 0)
    {
        sources[0].source = "a\0b";
        sources[0].source_bytes = 3U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
        preflight = 1;
    }
    if (strcmp(mode, "invalid-uri") == 0)
    {
        sources[0].document_uri = "bad\xff";
        expected = UMI_STATUS_PARSE_ERROR;
        preflight = 1;
    }
    if (strcmp(mode, "source-limit") == 0)
    {
        sources[0].source_bytes = UMI_LANGUAGE_RUNTIME_JSON_CAPACITY;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        preflight = 1;
    }
    if (strcmp(mode, "escaped-limit") == 0)
    {
        large = malloc(40001U);
        CHECK(large != NULL);
        memset(large, '"', 40000U);
        large[40000] = '\0';
        sources[0].source = large;
        sources[0].source_bytes = 40000U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        preflight = 1;
    }
    if (strcmp(mode, "too-many") == 0)
    {
        count = UMI_LANGUAGE_SOURCE_QUERY_MAXIMUM_DOCUMENTS;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
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
        count = 2U;
        fixture.fail_open_at = 3U;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "primary-close-failure") == 0)
    {
        count = 2U;
        fixture.fail_close_at = 1U;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "additional-close-failure") == 0)
    {
        count = 2U;
        fixture.fail_close_at = 2U;
        expected = UMI_STATUS_IO_ERROR;
    }
    int prepare = strcmp(mode, "prepare") == 0;
    Push(&fixture, prepare
                       ? "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{\"renameProvider\":{"
                         "\"prepareProvider\":true},\"textDocumentSync\":{\"openClose\":true,\"change\":1}}}}"
                       : "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{\"renameProvider\":"
                         "true,\"textDocumentSync\":{\"openClose\":true,\"change\":1}}}}");
    unsigned id = 2U;
    if (prepare)
    {
        Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":{\"start\":{\"line\":0,\"character\":0},"
                       "\"end\":{\"line\":0,\"character\":6}}}");
        id = 3U;
    }
    const char *uri = strcmp(mode, "no-additional") == 0    ? request.document_uri
                      : strcmp(mode, "unknown-target") == 0 ? "file:///workspace/unknown.c"
                                                            : "file:///workspace/second.c";
    char proposal[2048], response[4096];
    const char *edits = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                        "\"character\":6}},\"newText\":\"renamed\"}]";
    if (strcmp(mode, "outside") == 0)
    {
        edits = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":99}},"
                "\"newText\":\"renamed\"}]";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "overlap") == 0)
    {
        edits = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":6}},"
                "\"newText\":\"a\"},{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":0,"
                "\"character\":2}},\"newText\":\"b\"}]";
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "versioned") == 0 || strcmp(mode, "stale-version") == 0)
    {
        int version = strcmp(mode, "stale-version") == 0 ? 2 : 1;
        if (version == 2)
            expected = UMI_STATUS_INVALID_STATE;
        (void)snprintf(
            proposal, sizeof(proposal),
            "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"%s\",\"version\":%d},\"edits\":%s}]}", uri,
            version, edits);
    }
    else
        (void)snprintf(proposal, sizeof(proposal), "{\"changes\":{\"%s\":%s}}", uri, edits);
    if (strcmp(mode, "open-failure") != 0)
    {
        if (strcmp(mode, "query-error") == 0)
        {
            (void)snprintf(response, sizeof(response),
                           "{\"jsonrpc\":\"2.0\",\"id\":%u,\"error\":{\"code\":-1,\"message\":\"refused\"}}",
                           id);
            expected = UMI_STATUS_UNAVAILABLE;
        }
        else
            (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":%s}", id,
                           proposal);
        Push(&fixture, response);
        ++id;
    }
    if (strcmp(mode, "shutdown-error") == 0)
    {
        (void)snprintf(response, sizeof(response),
                       "{\"jsonrpc\":\"2.0\",\"id\":%u,\"error\":{\"code\":-1,\"message\":\"shutdown\"}}",
                       id);
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else
        (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":null}", id);
    Push(&fixture, response);
    UmiLanguageRenameReport report;
    UmiLanguageWorkspaceEditCatalogue *catalogue = (UmiLanguageWorkspaceEditCatalogue *)1;
    UmiStatus actual = UmiLanguageRenameQueryOnServerWithSources(server, &request, input, count, cancel,
                                                                 &report, &catalogue);
    if (actual != expected)
        fprintf(stderr, "%s: %d expected %d\n", mode, (int)actual, (int)expected);
    CHECK(actual == expected);
    if (preflight)
    {
        CHECK(!report.started && fixture.writeSize == 0U && fixture.stops == 0U);
    }
    else
    {
        unsigned opened = (unsigned)(count + 1U);
        if (strcmp(mode, "open-failure") == 0)
            opened = 2U;
        CHECK(report.started && report.initialized && report.document_opened &&
              fixture.close_attempts == opened);
        CHECK(fixture.stops != 0U);
        if (strcmp(mode, "open-failure") == 0)
            CHECK(strstr(fixture.written, "\"method\":\"textDocument/rename\"") == NULL);
        else
        {
            const char *query = strstr(fixture.written, "\"method\":\"textDocument/rename\"");
            CHECK(query != NULL);
            if (count != 0U)
            {
                const char *second_source = strstr(fixture.written, "source second");
                CHECK(second_source != NULL && second_source < query);
            }
            if (prepare)
            {
                const char *preparation = strstr(fixture.written, "textDocument/prepareRename");
                CHECK(preparation != NULL && strstr(fixture.written, "source second") < preparation &&
                      preparation < query);
            }
        }
    }
    if (expected == UMI_STATUS_OK)
    {
        CHECK(catalogue != NULL && UmiLanguageWorkspaceEditCatalogueCount(catalogue) == 1U);
        UmiLanguageWorkspaceDocumentChange change;
        CHECK(UmiLanguageWorkspaceEditCatalogueDocument(catalogue, 0U, &change) == UMI_STATUS_OK &&
              strcmp(change.uri, uri) == 0);
    }
    else
        CHECK(catalogue == NULL);
    UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    free(large);
    return 0;
}
