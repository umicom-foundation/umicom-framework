/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_open_source_range.c
 * PURPOSE: Check source navigation through a memory provider while preserving unsaved drafts and refusing partial tab changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/document.h"
#include "umicom/document/source_navigation.h"
#include "umicom/document/navigation_history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
#ifdef _WIN32
#define TARGET_PATH "C:\\source.c"
#define TARGET_URI "file:///C:/source.c"
#define NORMAL_URI "FILE://localhost/C:/folder/../source.c"
#else
#define TARGET_PATH "/source.c"
#define TARGET_URI "file:///source.c"
#define NORMAL_URI "FILE://localhost/folder/../source.c"
#endif
typedef struct Fixture
{
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *documents;
    const char *text;
    unsigned reads, stats;
    UmiStatus read_status, stat_status, hook_status;
    int hook;
    char source[UMI_UI_ID_CAPACITY], target[UMI_UI_ID_CAPACITY], uri[256];
} Fixture;
static UmiStatus Draft(Fixture *f, const char *id, const char *text)
{
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
    UmiUiDocumentViewSnapshot view;
    UmiStatus status = umi_ui_document_view_model_find(views, id, &view);
    if (status != UMI_STATUS_OK)
        return status;
    view.dirty = 1;
    view.cursor_offset = view.selection_length = 0U;
    return UmiUiDocumentViewModelUpsertText(views, &view, text, strlen(text));
}
static UmiStatus Read(void *context, const char *resource, unsigned char **out, size_t *size)
{
    Fixture *f = context;
    (void)resource;
    ++f->reads;
    if (f->read_status != UMI_STATUS_OK)
        return f->read_status;
    *size = strlen(f->text);
    *out = malloc(*size + 1U);
    if (*out == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(*out, f->text, *size + 1U);
    int hook = f->hook;
    f->hook = 0;
    if (hook == 1)
    {
        f->hook_status =
            umi_document_coordinator_open(f->documents, TARGET_PATH, f->target, sizeof(f->target));
        if (f->hook_status == UMI_STATUS_OK)
            f->hook_status = Draft(f, f->target, "edited target\n");
    }
    else if (hook == 2)
    {
        f->hook_status =
            UmiDocumentCoordinatorOpenSourceRange(f->documents, TARGET_URI, (UmiEditorTextPosition){0U, 0U},
                                                  (UmiEditorTextPosition){0U, 1U}, NULL, NULL, NULL);
    }
    else if (hook == 3)
        strcpy(f->uri, "https://example.invalid/changed");
    return UMI_STATUS_OK;
}
static UmiStatus Stat(void *context, const char *resource, UmiDocumentFileInfo *out)
{
    Fixture *f = context;
    ++f->stats;
    if (f->stat_status != UMI_STATUS_OK)
        return f->stat_status;
    *out = (UmiDocumentFileInfo){0};
    (void)snprintf(out->path, sizeof(out->path), "%s", resource);
    out->exists = out->regular_file = out->readable = out->writable = 1;
    out->byte_count = strlen(f->text);
    return UMI_STATUS_OK;
}
static void Release(void *context, void *bytes)
{
    (void)context;
    free(bytes);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *cases[] = {"new",        "existing",      "draft",         "read-only",     "untitled",
                           "normalised", "read-error",    "stat-error",    "range-invalid", "unicode",
                           "surrogate",  "crlf",          "remote",        "nul",           "history",
                           "no-active",  "provider-open", "provider-busy", "copied-uri",    "arguments"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            found = 1;
    CHECK(found);
    Fixture f = {0};
    f.text = "abc\nother\n";
    strcpy(f.uri, TARGET_URI);
    if (strcmp(mode, "unicode") == 0 || strcmp(mode, "surrogate") == 0)
        f.text = "a\xf0\x9f\x8c\x8d"
                 "b\n";
    if (strcmp(mode, "crlf") == 0)
        f.text = "abc\r\nxy";
    CHECK(umi_command_registry_create(&f.commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("source.navigation.provider", f.commands, &f.workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&f.store) == UMI_STATUS_OK);
    UmiDocumentProvider provider = {.struct_size = sizeof(UmiDocumentProvider),
                                    .abi_version = UMI_DOCUMENT_PROVIDER_ABI_VERSION,
                                    .provider_id = "source.memory",
                                    .scheme = "file",
                                    .flags = UMI_DOCUMENT_PROVIDER_READ | UMI_DOCUMENT_PROVIDER_STAT,
                                    .instance = &f,
                                    .read = Read,
                                    .stat = Stat,
                                    .release_bytes = Release};
    CHECK(umi_document_coordinator_create(f.store, f.workbench, &provider, &f.documents) == UMI_STATUS_OK);
    UmiDocumentId source_id = 0U;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot view;
    if (strcmp(mode, "no-active") != 0)
    {
        CHECK(umi_document_coordinator_new(f.documents, "origin.c", f.source, sizeof(f.source)) ==
              UMI_STATUS_OK);
        CHECK(Draft(&f, f.source, "source\n") == UMI_STATUS_OK);
        UmiDocumentWorkingCopySnapshot active;
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK);
        source_id = active.document_id;
    }
    const char *uri = f.uri, *expected_text = strcmp(mode, "crlf") == 0 ? "abc\nxy" : f.text;
    int existing = strcmp(mode, "existing") == 0 || strcmp(mode, "draft") == 0 ||
                   strcmp(mode, "read-only") == 0 || strcmp(mode, "normalised") == 0;
    if (existing)
    {
        CHECK(umi_document_coordinator_open(f.documents, TARGET_PATH, f.target, sizeof(f.target)) ==
              UMI_STATUS_OK);
        if (strcmp(mode, "draft") == 0)
        {
            expected_text = "unsaved target\n";
            CHECK(Draft(&f, f.target, expected_text) == UMI_STATUS_OK);
        }
        CHECK(umi_ui_document_view_model_find(views, f.target, &view) == UMI_STATUS_OK);
        if (strcmp(mode, "read-only") == 0)
        {
            view.read_only = 1;
            CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
        }
        CHECK(umi_ui_workbench_activate_document(f.workbench, f.source) == UMI_STATUS_OK);
        if (strcmp(mode, "normalised") == 0)
            uri = NORMAL_URI;
    }
    if (strcmp(mode, "untitled") == 0)
    {
        CHECK(umi_ui_document_view_model_find(views, f.source, &view) == UMI_STATUS_OK);
        strcpy(f.uri, view.uri);
        uri = f.uri;
        expected_text = "source\n";
    }
    UmiEditorTextPosition begin = {0U, 1U}, end = {0U, 3U};
    size_t expected_offset = 1U, expected_bytes = strcmp(mode, "unicode") == 0 ? 4U : 2U;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "range-invalid") == 0)
    {
        end.line = 90U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "surrogate") == 0)
    {
        end.utf16_column = 2U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "crlf") == 0)
    {
        begin = (UmiEditorTextPosition){1U, 0U};
        end = (UmiEditorTextPosition){1U, 2U};
        expected_offset = 4U;
    }
    if (strcmp(mode, "read-error") == 0)
    {
        f.read_status = UMI_STATUS_IO_ERROR;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "stat-error") == 0)
    {
        f.stat_status = UMI_STATUS_IO_ERROR;
        expected = UMI_STATUS_IO_ERROR;
    }
    if (strcmp(mode, "remote") == 0)
    {
        uri = "https://example.invalid/source.c";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "nul") == 0)
    {
        uri = TARGET_URI "%00tail";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "provider-open") == 0)
    {
        f.hook = 1;
        expected_text = "edited target\n";
    }
    if (strcmp(mode, "provider-busy") == 0)
        f.hook = 2;
    if (strcmp(mode, "copied-uri") == 0)
        f.hook = 3;
    f.reads = f.stats = 0U;
    size_t count_before = umi_document_coordinator_count(f.documents);
    UmiDocumentNavigationHistorySnapshot history_before, history_after;
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(f.documents, &history_before) == UMI_STATUS_OK);
    UmiDocumentId selected = UINT64_MAX;
    size_t offset = 777U, bytes = 888U;
    CHECK(UmiDocumentCoordinatorOpenSourceRange(f.documents, uri, begin, end, &selected, &offset, &bytes) ==
          expected);
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(f.documents, &history_after) == UMI_STATUS_OK &&
          !history_after.busy);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(offset == expected_offset && bytes == expected_bytes);
        UmiDocumentWorkingCopySnapshot active;
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK &&
              active.document_id == selected);
        CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK);
        CHECK(view.cursor_offset == offset && view.selection_length == bytes);
        char *text = NULL;
        size_t length = 0U;
        CHECK(UmiUiDocumentViewModelCopyText(views, active.view_id, &text, &length) == UMI_STATUS_OK);
        CHECK(length == strlen(expected_text) && strcmp(text, expected_text) == 0);
        UmiUiDocumentViewModelFreeText(text);
        CHECK(umi_document_coordinator_count(f.documents) ==
              count_before + (existing || strcmp(mode, "untitled") == 0 ? 0U : 1U));
        if (existing || strcmp(mode, "untitled") == 0)
            CHECK(f.reads == 0U && f.stats == 0U);
        if (strcmp(mode, "provider-open") == 0)
            CHECK(f.hook_status == UMI_STATUS_OK && f.reads == 2U);
        if (strcmp(mode, "provider-busy") == 0)
            CHECK(f.hook_status == UMI_STATUS_BUSY && f.reads == 1U);
        if (strcmp(mode, "history") == 0)
        {
            CHECK(history_after.can_go_back);
            CHECK(UmiDocumentCoordinatorTravel(f.documents, -1, history_after.revision, NULL) ==
                  UMI_STATUS_OK);
            CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK &&
                  active.document_id == source_id);
        }
    }
    else
    {
        CHECK(selected == UINT64_MAX && offset == 777U && bytes == 888U);
        CHECK(umi_document_coordinator_count(f.documents) == count_before &&
              history_after.revision == history_before.revision);
        UmiDocumentWorkingCopySnapshot active;
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK &&
              active.document_id == source_id);
        if (strcmp(mode, "remote") == 0 || strcmp(mode, "nul") == 0)
            CHECK(f.reads == 0U && f.stats == 0U);
    }
    if (strcmp(mode, "arguments") == 0)
    {
        CHECK(UmiDocumentCoordinatorOpenSourceRange(NULL, TARGET_URI, begin, end, NULL, NULL, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorOpenSourceRange(f.documents, NULL, begin, end, NULL, NULL, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    umi_document_coordinator_destroy(f.documents);
    umi_document_store_destroy(f.store);
    umi_ui_workbench_destroy(f.workbench);
    umi_command_registry_destroy(f.commands);
    return 0;
}
