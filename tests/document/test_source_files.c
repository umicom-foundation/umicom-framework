/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_source_files.c
 *
 * PURPOSE:
 *   Check complete source-file editing, history and saving through real stores.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"
#include "umicom/editor/search_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define PROCESS_ID _getpid
#else
#include <unistd.h>
#define PROCESS_ID getpid
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

typedef struct Fixture {
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *documents;
    UmiUiDocumentViewModel *views;
    char root[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    char viewId[UMI_UI_ID_CAPACITY];
    char *text;
    size_t length;
    int createdRoot;
} Fixture;

/* Each process uses an independent directory and releases every owned buffer. */
static int Start(Fixture *fixture, const char *name)
{
    char temp[UMI_PATH_CAPACITY], folder[128];
    CHECK(snprintf(folder, sizeof folder, "umicom-source-%ld-%s", (long)PROCESS_ID(), name) > 0);
    CHECK(umi_fs_temp_directory(temp, sizeof temp) == UMI_STATUS_OK);
    CHECK(umi_fs_join(fixture->root, sizeof fixture->root, temp, folder) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(fixture->root));
    CHECK(umi_fs_make_directories(fixture->root) == UMI_STATUS_OK);
    fixture->createdRoot = 1;
    CHECK(umi_fs_join(fixture->path, sizeof fixture->path, fixture->root, "notes.c") == UMI_STATUS_OK);
    CHECK(umi_command_registry_create(&fixture->commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.source-files", fixture->commands, &fixture->workbench) == UMI_STATUS_OK);
    fixture->views = umi_ui_workbench_documents(fixture->workbench);
    CHECK(umi_document_store_create(&fixture->store) == UMI_STATUS_OK);
    fixture->length = 131072U;
    fixture->text = malloc(UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 2U);
    CHECK(fixture->text != NULL);
    memset(fixture->text, ' ', fixture->length);
    for (size_t index = 63U; index < fixture->length; index += 64U) fixture->text[index] = '\n';
    memcpy(fixture->text + fixture->length - 13U, "noteCount = 7", 13U);
    fixture->text[fixture->length] = '\0';
    if (strcmp(name, "import-large") == 0 || strcmp(name, "import-rollback") == 0) {
        UmiUiDocumentViewSnapshot view = {0};
        strcpy(view.view_id, "imported-notes"); strcpy(view.title, "imported.c");
        view.dirty = 1;
        CHECK(UmiUiDocumentViewModelUpsertText(fixture->views, &view, fixture->text, fixture->length) == UMI_STATUS_OK);
        if (strcmp(name, "import-rollback") == 0) {
            UmiDocumentId id;
            for (size_t n = 0U; n < UMI_DOCUMENT_STORE_MAX - 1U; ++n)
                CHECK(umi_document_store_new(fixture->store, "existing", &id) == UMI_STATUS_OK);
            strcpy(view.view_id, "second-contribution");
            CHECK(UmiUiDocumentViewModelUpsertText(fixture->views, &view, fixture->text, fixture->length) == UMI_STATUS_OK);
        }
        return 0;
    }
    CHECK(umi_document_coordinator_create(fixture->store, fixture->workbench, NULL, &fixture->documents) == UMI_STATUS_OK);
    CHECK(umi_fs_write_text(fixture->path, fixture->text) == UMI_STATUS_OK);
    return 0;
}

/* Compare complete content, not the prefix in the compatibility snapshot. */
static int FullEquals(Fixture *fixture, const char *expected, size_t expectedLength)
{
    char *copy = NULL; size_t length = 0U;
    CHECK(UmiUiDocumentViewModelCopyText(fixture->views, fixture->viewId, &copy, &length) == UMI_STATUS_OK);
    int equal = length == expectedLength && memcmp(copy, expected, length) == 0 && copy[length] == '\0';
    UmiUiDocumentViewModelFreeText(copy);
    CHECK(equal);
    return 0;
}

/* Publish the same complete draft that a native editor hands to Framework. */
static int Edit(Fixture *fixture, const char *text, size_t length, int dirty)
{
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(fixture->views, fixture->viewId, &view) == UMI_STATUS_OK);
    view.dirty = dirty;
    CHECK(UmiUiDocumentViewModelUpsertText(fixture->views, &view, text, length) == UMI_STATUS_OK);
    return 0;
}

/* Disk comparison also detects truncation after the old preview boundary. */
static int DiskEquals(const char *path, const char *expected, size_t expectedLength)
{
    char *text = NULL; size_t length = 0U;
    CHECK(umi_fs_read_text(path, &text, &length) == UMI_STATUS_OK);
    int equal = length == expectedLength && memcmp(text, expected, length) == 0;
    umi_fs_free_text(text);
    CHECK(equal);
    return 0;
}

typedef struct LoadProbe {
    UmiStatus statStatus;
    uint64_t reportedSize;
    size_t returnedSize;
    unsigned reads;
} LoadProbe;

/* A minimal provider makes the pre-read contract observable independently of
 * a real filesystem race. Actual file loading is exercised by the other cases. */
static UmiStatus ProbeStat(void *instance, const char *resource, UmiDocumentFileInfo *info)
{
    LoadProbe *probe = instance;
    (void)resource;
    *info = (UmiDocumentFileInfo){0};
    info->exists = 1;
    info->byte_count = probe->reportedSize;
    return probe->statStatus;
}

static UmiStatus ProbeRead(void *instance, const char *resource, unsigned char **bytes, size_t *length)
{
    LoadProbe *probe = instance;
    (void)resource;
    ++probe->reads;
    *bytes = malloc(probe->returnedSize);
    if (*bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memset(*bytes, 'x', probe->returnedSize);
    *length = probe->returnedSize;
    return UMI_STATUS_OK;
}

static void ProbeRelease(void *instance, void *bytes)
{
    (void)instance;
    free(bytes);
}

/* Report size is an early guard, never a substitute for checking read bytes. */
static int CheckLoader(const char *name)
{
    LoadProbe probe = {UMI_STATUS_OK, 40U, 40U, 0U};
    UmiDocumentProvider provider = {0};
    UmiDocumentLoadOptions options = umi_document_load_options_default();
    UmiDocumentLoadResult result;
    provider.struct_size = (uint32_t)sizeof provider;
    provider.abi_version = UMI_DOCUMENT_PROVIDER_ABI_VERSION;
    provider.provider_id = "test.load-probe";
    provider.scheme = "memory";
    provider.flags = UMI_DOCUMENT_PROVIDER_READ | UMI_DOCUMENT_PROVIDER_STAT;
    provider.instance = &probe;
    provider.read = ProbeRead;
    provider.release_bytes = ProbeRelease;
    provider.stat = ProbeStat;
    options.maximum_bytes = 20U;
    unsigned expectedReads = 0U;
    UmiStatus expected = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(name, "loader-no-stat") == 0) {
        provider.flags = UMI_DOCUMENT_PROVIDER_READ;
        expectedReads = 1U;
    } else if (strcmp(name, "loader-changed-size") == 0) {
        probe.reportedSize = 10U;
        expectedReads = 1U;
    } else if (strcmp(name, "loader-stat-error") == 0) {
        probe.statStatus = UMI_STATUS_PERMISSION_DENIED;
        expected = UMI_STATUS_PERMISSION_DENIED;
    }
    UmiStatus status = umi_document_load(&provider, "notes.c", &options, &result);
    umi_document_load_result_dispose(&result);
    CHECK(status == expected && probe.reads == expectedReads);
    return 0;
}

static int Run(Fixture *fixture, const char *name)
{
    if (strncmp(name, "loader-", 7U) == 0) return CheckLoader(name);
    UmiDocumentWorkingCopySnapshot before, after;
    UmiUiDocumentViewSnapshot view;
    size_t offset = 0U, count = 0U;
    if (strncmp(name, "import-", 7U) == 0) {
        UmiStatus status = umi_document_coordinator_create(fixture->store, fixture->workbench, NULL, &fixture->documents);
        if (strcmp(name, "import-rollback") == 0) {
            CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && fixture->documents == NULL);
            CHECK(umi_document_store_count(fixture->store) == UMI_DOCUMENT_STORE_MAX - 1U);
            CHECK(umi_ui_document_view_model_count(fixture->views) == 2U);
        } else {
            CHECK(status == UMI_STATUS_OK);
            strcpy(fixture->viewId, "imported-notes");
            CHECK(umi_ui_workbench_activate_document(fixture->workbench, fixture->viewId) == UMI_STATUS_OK);
            CHECK(FullEquals(fixture, fixture->text, fixture->length) == 0);
            CHECK(umi_document_coordinator_save_active_as(fixture->documents, fixture->path) == UMI_STATUS_OK);
            CHECK(DiskEquals(fixture->path, fixture->text, fixture->length) == 0);
        }
        return 0;
    }
    if (strcmp(name, "decoded-limit") == 0) {
        /* 6 MiB UTF-16 input expands to 9 MiB UTF-8; raw-byte checks alone
         * cannot protect the editor's decoded-document limit. */
        size_t bytes = 6U * 1024U * 1024U;
        unsigned char *encoded = (unsigned char *)fixture->text;
        encoded[0] = 0xffU; encoded[1] = 0xfeU;
        for (size_t n = 2U; n < bytes; n += 2U) { encoded[n] = 0xacU; encoded[n + 1U] = 0x20U; }
        CHECK(umi_fs_write_bytes(fixture->path, encoded, bytes) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_open(fixture->documents, fixture->path, NULL, 0U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_document_store_count(fixture->store) == 0U && umi_document_coordinator_count(fixture->documents) == 0U);
        return 0;
    }
    if (strcmp(name, "raw-limit") == 0) {
        memset(fixture->text, 'x', UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U);
        CHECK(umi_fs_write_bytes(fixture->path, fixture->text, UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_open(fixture->documents, fixture->path, NULL, 0U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_document_store_count(fixture->store) == 0U);
        return 0;
    }
    CHECK(umi_document_coordinator_open(fixture->documents, fixture->path, fixture->viewId, sizeof fixture->viewId) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &before) == UMI_STATUS_OK);
    CHECK(before.text_length == fixture->length && !before.dirty);
    if (strcmp(name, "open-complete") == 0) return FullEquals(fixture, fixture->text, fixture->length);
    if (strcmp(name, "tail-search") == 0) {
        CHECK(umi_document_coordinator_find(fixture->documents, "noteCount", &offset) == UMI_STATUS_OK);
        CHECK(offset == fixture->length - 13U);
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "noteCount", 1, &offset, NULL) == UMI_STATUS_OK);
        CHECK(offset == fixture->length - 13U);
        CHECK(umi_document_coordinator_go_to_line(fixture->documents, 1025U, &offset) == UMI_STATUS_OK && offset == 65536U);
        return FullEquals(fixture, fixture->text, fixture->length);
    }
    if (strcmp(name, "tail-dirty") == 0 || strcmp(name, "save-complete") == 0 || strcmp(name, "reopen-draft") == 0) {
        fixture->text[fixture->length - 1U] = '8';
        CHECK(Edit(fixture, fixture->text, fixture->length, 0) == 0);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK && after.dirty);
        CHECK(after.text_length == fixture->length && after.revision == before.revision);
        if (strcmp(name, "reopen-draft") == 0) {
            CHECK(umi_document_coordinator_open(fixture->documents, fixture->path, NULL, 0U) == UMI_STATUS_OK);
            CHECK(umi_document_coordinator_count(fixture->documents) == 1U);
            return FullEquals(fixture, fixture->text, fixture->length);
        }
        CHECK(UmiDocumentCoordinatorSaveAll(fixture->documents, &count) == UMI_STATUS_OK && count == 1U);
        CHECK(DiskEquals(fixture->path, fixture->text, fixture->length) == 0);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK && !after.dirty);
        return 0;
    }
    if (strcmp(name, "replace-history") == 0) {
        CHECK(umi_document_coordinator_replace(fixture->documents, "noteCount", "savedCount", &offset) == UMI_STATUS_OK);
        CHECK(offset == fixture->length - 13U);
        CHECK(umi_document_coordinator_undo(fixture->documents) == UMI_STATUS_OK);
        CHECK(FullEquals(fixture, fixture->text, fixture->length) == 0);
        CHECK(umi_document_coordinator_redo(fixture->documents) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_find(fixture->documents, "savedCount", &offset) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_save_active(fixture->documents) == UMI_STATUS_OK);
        char *disk = NULL; size_t length = 0U;
        CHECK(umi_fs_read_text(fixture->path, &disk, &length) == UMI_STATUS_OK);
        int complete = length == fixture->length + 1U && strstr(disk + 65536U, "savedCount") != NULL;
        umi_fs_free_text(disk); CHECK(complete);
        return 0;
    }
    if (strcmp(name, "many-replacements") == 0) {
        const size_t repetitions = 12000U;
        for (size_t n = 0U; n < repetitions; ++n) memcpy(fixture->text + n * 5U, "note ", 5U);
        fixture->length = repetitions * 5U; fixture->text[fixture->length] = '\0';
        CHECK(Edit(fixture, fixture->text, fixture->length, 1) == 0);
        CHECK(UmiDocumentCoordinatorReplaceAll(fixture->documents, "note", "savedNote", &count) == UMI_STATUS_OK && count == repetitions);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK && after.text_length == repetitions * 10U);
        CHECK(umi_document_coordinator_undo(fixture->documents) == UMI_STATUS_OK);
        return FullEquals(fixture, fixture->text, fixture->length);
    }
    if (strcmp(name, "replace-rejected") == 0) {
        char *replacement = malloc(UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U);
        CHECK(replacement != NULL);
        memset(replacement, 'x', UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES); replacement[UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES] = '\0';
        UmiStatus rejected = UmiDocumentCoordinatorReplaceAll(fixture->documents, "noteCount", replacement, &count);
        free(replacement);
        CHECK(rejected == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK);
        CHECK(after.revision == before.revision && after.undo_count == before.undo_count && !after.dirty);
        CHECK(DiskEquals(fixture->path, fixture->text, fixture->length) == 0);
        return FullEquals(fixture, fixture->text, fixture->length);
    }
    if (strcmp(name, "read-only") == 0) {
        CHECK(umi_ui_document_view_model_find(fixture->views, fixture->viewId, &view) == UMI_STATUS_OK);
        view.read_only = 1; CHECK(umi_ui_document_view_model_upsert(fixture->views, &view) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorReplaceAll(fixture->documents, "noteCount", "changed", NULL) == UMI_STATUS_PERMISSION_DENIED);
        return FullEquals(fixture, fixture->text, fixture->length);
    }
    if (strcmp(name, "external-conflict") == 0) {
        fixture->text[fixture->length - 1U] = '9'; CHECK(Edit(fixture, fixture->text, fixture->length, 1) == 0);
        CHECK(umi_fs_write_text(fixture->path, "external edit\n") == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_save_active(fixture->documents) == UMI_STATUS_INVALID_STATE);
        CHECK(DiskEquals(fixture->path, "external edit\n", 14U) == 0);
        return FullEquals(fixture, fixture->text, fixture->length);
    }
    if (strcmp(name, "captured-save") == 0) {
        char destination[UMI_PATH_CAPACITY];
        CHECK(umi_fs_join(destination, sizeof destination, fixture->root, "saved-notes.c") == UMI_STATUS_OK);
        fixture->text[fixture->length - 1U] = '9'; CHECK(Edit(fixture, fixture->text, fixture->length, 1) == 0);
        CHECK(umi_document_coordinator_new(fixture->documents, "other.c", NULL, 0U) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorSaveAs(fixture->documents, before.document_id, destination) == UMI_STATUS_OK);
        CHECK(DiskEquals(destination, fixture->text, fixture->length) == 0);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK && after.document_id != before.document_id);
        return FullEquals(fixture, fixture->text, fixture->length);
    }
    if (strcmp(name, "history-budget") == 0) {
        fixture->length = 4U * 1024U * 1024U;
        memset(fixture->text, 'x', fixture->length); fixture->text[fixture->length] = '\0';
        for (unsigned step = 0U; step < 8U; ++step) {
            fixture->text[fixture->length - 1U] = (char)('a' + step);
            CHECK(Edit(fixture, fixture->text, fixture->length, 1) == 0);
            CHECK(umi_document_coordinator_sync_active(fixture->documents) == UMI_STATUS_OK);
        }
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK);
        CHECK(after.undo_count > 0U && after.undo_count < 8U);
        CHECK(umi_document_coordinator_undo(fixture->documents) == UMI_STATUS_OK);
        fixture->text[fixture->length - 1U] = 'g';
        return FullEquals(fixture, fixture->text, fixture->length);
    }
    if (strcmp(name, "shrink-and-undo") == 0) {
        CHECK(Edit(fixture, "note", 4U, 1) == 0);
        CHECK(umi_document_coordinator_sync_active(fixture->documents) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_undo(fixture->documents) == UMI_STATUS_OK);
        return FullEquals(fixture, fixture->text, fixture->length);
    }
    fprintf(stderr, "Unknown source-file case: %s\n", name);
    return 1;
}

int main(int argc, char **argv)
{
    Fixture fixture = {0};
    int result = 1;
    if (argc == 2 && Start(&fixture, argv[1]) == 0) result = Run(&fixture, argv[1]);
    umi_document_coordinator_destroy(fixture.documents);
    umi_document_store_destroy(fixture.store);
    umi_ui_workbench_destroy(fixture.workbench);
    umi_command_registry_destroy(fixture.commands);
    free(fixture.text);
    if (fixture.createdRoot) (void)umi_fs_remove_tree(fixture.root);
    return result;
}
