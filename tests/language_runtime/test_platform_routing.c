/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_platform_routing.c
 * PURPOSE: Exercise server-scoped requests and reviewed completion acceptance through the platform.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/platform.h"
#include "umicom/language_runtime/memory_transport.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition)                                                                                     \
    do                                                                                                       \
    {                                                                                                        \
        if (!(condition))                                                                                    \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #condition);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
typedef struct Fixture
{
    UmiLanguageRuntimePlatform *platform;
    UmiLanguageRuntimeMemoryTransport *memory[2];
    UmiLanguageRuntimeServer *server[2];
    UmiEditorTextBuffer *buffer;
} Fixture;
static int Open(Fixture *fixture, size_t index)
{
    const char *root = index == 0U ? "file:///left" : "file:///right";
    const char *document = index == 0U ? "left" : "right";
    const char *uri = index == 0U ? "file:///left/a.c" : "file:///right/a.c";
    CHECK(umi_language_runtime_platform_open_document(fixture->platform, root, ".", document, uri, "c", "a.c",
                                                      "ab\n", 0U) == UMI_STATUS_OK);
    return 0;
}
static int Setup(Fixture *fixture)
{
    memset(fixture, 0, sizeof(*fixture));
    CHECK(umi_language_runtime_platform_create(&fixture->platform) == UMI_STATUS_OK);
    CHECK(umi_editor_text_buffer_create(64U, &fixture->buffer) == UMI_STATUS_OK);
    CHECK(umi_editor_text_buffer_set(fixture->buffer, "ab\n", 3U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 2U; ++i)
    {
        UmiLanguageRuntimeTransport transport;
        UmiLanguageServerProfile profile = {0};
        const char *root = i == 0U ? "file:///left" : "file:///right";
        strcpy(profile.id, i == 0U ? "left-profile" : "right-profile");
        strcpy(profile.display_name, "Memory peer");
        strcpy(profile.executable, "not-launched");
        profile.enabled = 1;
        CHECK(umi_language_runtime_memory_transport_create(&fixture->memory[i], &transport) == UMI_STATUS_OK);
        CHECK(umi_language_runtime_server_create_with_transport(i == 0U ? "left-server" : "right-server",
                                                                &profile, root, &transport,
                                                                &fixture->server[i]) == UMI_STATUS_OK);
        UmiLanguageRuntimeInitializeResult capabilities = {0};
        capabilities.completion = 1;
        capabilities.hover = 1;
        CHECK(umi_language_runtime_platform_attach_server(fixture->platform, "c", root, fixture->server[i],
                                                          &capabilities) == UMI_STATUS_OK);
        CHECK(Open(fixture, i) == 0);
    }
    return 0;
}
static int Feed(Fixture *fixture, size_t index, const char *json)
{
    char frame[4096];
    size_t bytes = 0U;
    CHECK(umi_language_runtime_frame_encode(json, frame, sizeof(frame), &bytes) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_memory_transport_push_read(fixture->memory[index], frame, bytes) ==
          UMI_STATUS_OK);
    return 0;
}
static int Completion(Fixture *fixture, size_t index, unsigned id, const char *label)
{
    char json[512];
    CHECK(snprintf(json, sizeof(json), "{\"jsonrpc\":\"2.0\",\"id\":%u,\"result\":[{\"label\":\"%s\"}]}", id,
                   label) > 0);
    return Feed(fixture, index, json);
}
static int Pump(Fixture *fixture, size_t index, int expected_handled)
{
    int handled = 99;
    CHECK(umi_language_runtime_platform_pump_server(fixture->platform, "c",
                                                    index == 0U ? "file:///left" : "file:///right", 0U,
                                                    &handled) == UMI_STATUS_OK);
    CHECK(handled == expected_handled);
    return 0;
}
static int Label(Fixture *fixture, const char *document, const char *label)
{
    char id[64];
    (void)snprintf(id, sizeof(id), "%s.completion.0", document);
    UmiLanguageCompletionSnapshot item;
    CHECK(umi_language_completion_registry_find(
              umi_language_service_completion(umi_language_runtime_platform_language(fixture->platform)), id,
              &item) == UMI_STATUS_OK);
    CHECK(strcmp(item.document_id, document) == 0 && strcmp(item.label, label) == 0);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    Fixture fixture;
    CHECK(Setup(&fixture) == 0);
    UmiLanguageRuntimePlatform *platform = fixture.platform;
    if (strncmp(name, "diagnostic-", 11U) == 0)
    {
        int wrong = strcmp(name, "diagnostic-owner") == 0;
        const char *version = strcmp(name, "diagnostic-stale") == 0         ? ",\"version\":0"
                              : strcmp(name, "diagnostic-future") == 0      ? ",\"version\":2"
                              : strcmp(name, "diagnostic-unversioned") == 0 ? ""
                                                                            : ",\"version\":1";
        CHECK(wrong || strcmp(name, "diagnostic-stale") == 0 || strcmp(name, "diagnostic-future") == 0 ||
              strcmp(name, "diagnostic-unversioned") == 0 || strcmp(name, "diagnostic-current") == 0);
        char json[1024];
        CHECK(snprintf(json, sizeof(json),
                       "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/"
                       "publishDiagnostics\",\"params\":{\"uri\":\"file:///left/a.c\"%s,"
                       "\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
                       "\"line\":0,\"character\":1}},"
                       "\"severity\":1,\"message\":\"check\"}]}}",
                       version) > 0);
        CHECK(Feed(&fixture, wrong ? 1U : 0U, json) == 0);
        int accepted = !wrong && (strcmp(name, "diagnostic-current") == 0 ||
                                  strcmp(name, "diagnostic-unversioned") == 0);
        CHECK(Pump(&fixture, wrong ? 1U : 0U, accepted) == 0);
        CHECK(umi_language_diagnostic_registry_count(umi_language_service_diagnostic(
                  umi_language_runtime_platform_language(platform))) == (accepted ? 1U : 0U));
    }
    else if (strcmp(name, "scoped") == 0 || strcmp(name, "wrong-server") == 0)
    {
        CHECK(umi_language_runtime_platform_request_completion(platform, "left", 0U, 1U) == UMI_STATUS_OK);
        if (strcmp(name, "scoped") == 0)
        {
            CHECK(umi_language_runtime_platform_request_completion(platform, "right", 0U, 1U) ==
                  UMI_STATUS_OK);
            CHECK(Completion(&fixture, 1U, 1U, "right-answer") == 0 && Pump(&fixture, 1U, 1) == 0);
            CHECK(Label(&fixture, "right", "right-answer") == 0);
        }
        else
            CHECK(Completion(&fixture, 1U, 1U, "wrong") == 0 && Pump(&fixture, 1U, 0) == 0);
        CHECK(Completion(&fixture, 0U, 1U, "left-answer") == 0 && Pump(&fixture, 0U, 1) == 0);
        CHECK(Label(&fixture, "left", "left-answer") == 0);
    }
    else if (strcmp(name, "capacity") == 0)
    {
        for (size_t i = 0U; i < UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS; ++i)
            CHECK(umi_language_runtime_platform_request_completion(platform, "left", 0U, 1U) ==
                  UMI_STATUS_OK);
        UmiLanguageRuntimeServerSnapshot before, after;
        CHECK(umi_language_runtime_server_snapshot(fixture.server[1], &before) == UMI_STATUS_OK);
        CHECK(umi_language_runtime_platform_request_completion(platform, "right", 0U, 1U) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_language_runtime_server_snapshot(fixture.server[1], &after) == UMI_STATUS_OK);
        CHECK(before.messages_sent == after.messages_sent && before.next_request_id == after.next_request_id);
    }
    else if (strcmp(name, "latest-first") == 0 || strcmp(name, "oldest-first") == 0)
    {
        CHECK(umi_language_runtime_platform_request_completion(platform, "left", 0U, 1U) == UMI_STATUS_OK);
        CHECK(umi_language_runtime_platform_request_completion(platform, "left", 0U, 2U) == UMI_STATUS_OK);
        if (strcmp(name, "oldest-first") == 0)
        {
            CHECK(Completion(&fixture, 0U, 1U, "old") == 0 && Pump(&fixture, 0U, 0) == 0);
            CHECK(Completion(&fixture, 0U, 2U, "new") == 0 && Pump(&fixture, 0U, 1) == 0);
        }
        else
        {
            CHECK(Completion(&fixture, 0U, 2U, "new") == 0 && Pump(&fixture, 0U, 1) == 0);
            CHECK(Completion(&fixture, 0U, 1U, "old") == 0 && Pump(&fixture, 0U, 0) == 0);
        }
        CHECK(Label(&fixture, "left", "new") == 0);
    }
    else
    {
        int display_only = strcmp(name, "display-only") == 0;
        CHECK((display_only ? umi_language_runtime_platform_request_completion(platform, "left", 0U, 1U)
                            : UmiLanguageRuntimePlatformRequestCompletionReview(
                                  platform, "left", 0U, 1U, fixture.buffer)) == UMI_STATUS_OK);
        if (strcmp(name, "server-request") == 0)
        {
            CHECK(
                Feed(&fixture, 0U,
                     "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"workspace/configuration\",\"params\":{}}") ==
                0);
            CHECK(Pump(&fixture, 0U, 0) == 0);
        }
        int retired =
            strcmp(name, "change") == 0 || strcmp(name, "close") == 0 || strcmp(name, "reopen") == 0;
        if (strcmp(name, "change") == 0)
            CHECK(umi_language_runtime_platform_change_document(platform, "left", "abc\n") == UMI_STATUS_OK);
        if (strcmp(name, "close") == 0 || strcmp(name, "reopen") == 0)
        {
            CHECK(umi_language_runtime_platform_close_document(platform, "left") == UMI_STATUS_OK);
            if (strcmp(name, "reopen") == 0)
                CHECK(Open(&fixture, 0U) == 0);
        }
        CHECK(Feed(&fixture, 0U,
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[{\"label\":\"puts\","
                   "\"textEdit\":{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                   "\"character\":2}},\"newText\":\"puts\"}}]}") == 0);
        CHECK(Pump(&fixture, 0U, retired ? 0 : 1) == 0);
        const UmiLanguageCompletionCatalogue *catalogue = NULL;
        CHECK(UmiLanguageRuntimePlatformCompletionCatalogue(platform, "left", &catalogue) ==
              (retired ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK));
        if (!retired)
        {
            CHECK(catalogue != NULL && UmiLanguageCompletionCatalogueCount(catalogue) == 1U);
            if (strcmp(name, "buffer-change") == 0)
                CHECK(umi_editor_text_buffer_insert(fixture.buffer, 0U, "!", 1U) == UMI_STATUS_OK);
            if (strcmp(name, "next-request") == 0)
            {
                CHECK(UmiLanguageRuntimePlatformRequestCompletionReview(platform, "left", 0U, 1U,
                                                                        fixture.buffer) == UMI_STATUS_OK);
                CHECK(UmiLanguageRuntimePlatformCompletionCatalogue(platform, "left", &catalogue) ==
                      UMI_STATUS_NOT_FOUND);
            }
            else
            {
                UmiEditorWorkspaceEditSet *edits = NULL;
                UmiLanguageCompletionRange range = {{0U, 0U}, {0U, 2U}};
                UmiStatus expected = display_only || strcmp(name, "buffer-change") == 0
                                         ? UMI_STATUS_INVALID_STATE
                                         : UMI_STATUS_OK;
                CHECK(UmiLanguageRuntimePlatformCompletionPlan(platform, "left", 0U, fixture.buffer, range,
                                                               UMI_LANGUAGE_COMPLETION_REPLACE, NULL,
                                                               &edits) == expected);
                if (expected == UMI_STATUS_OK)
                {
                    size_t applied = 0U;
                    CHECK(umi_editor_workspace_edit_set_apply_document(
                              edits, "file:///left/a.c", fixture.buffer, 1, &applied) == UMI_STATUS_OK);
                    UmiEditorTextBufferView view;
                    CHECK(umi_editor_text_buffer_view(fixture.buffer, &view) == UMI_STATUS_OK);
                    CHECK(applied == 1U && view.byte_count == 5U && memcmp(view.bytes, "puts\n", 5U) == 0);
                }
                else
                    CHECK(edits == NULL);
                umi_editor_workspace_edit_set_destroy(edits);
            }
        }
        CHECK(display_only || retired || strcmp(name, "review") == 0 || strcmp(name, "buffer-change") == 0 ||
              strcmp(name, "next-request") == 0 || strcmp(name, "server-request") == 0);
    }
    umi_editor_text_buffer_destroy(fixture.buffer);
    umi_language_runtime_platform_destroy(platform);
    return 0;
}
