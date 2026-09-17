/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_editor_workflow.c
 * PURPOSE: Exercise real editor drafts, tab identity and rejected mutations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"
#include "umicom/studio_runtime/close_guard.h"
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
    char root[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    int createdRoot;
} Fixture;

/* Isolate each invocation from the source checkout and other parallel tests. */
static int Start(Fixture *fixture, const char *name)
{
    char temp[UMI_PATH_CAPACITY];
    char folder[128];
    CHECK(snprintf(folder, sizeof folder, "umicom-editor-%ld-%s", (long)PROCESS_ID(), name) > 0);
    CHECK(umi_fs_temp_directory(temp, sizeof temp) == UMI_STATUS_OK);
    CHECK(umi_fs_join(fixture->root, sizeof fixture->root, temp, folder) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(fixture->root));
    CHECK(umi_fs_make_directories(fixture->root) == UMI_STATUS_OK);
    fixture->createdRoot = 1;
    CHECK(umi_fs_join(fixture->path, sizeof fixture->path, fixture->root, "notes.c") == UMI_STATUS_OK);
    CHECK(umi_command_registry_create(&fixture->commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.editor-workflow", fixture->commands, &fixture->workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&fixture->store) == UMI_STATUS_OK);
    if (strcmp(name, "virtual-draft") == 0) {
        UmiUiDocumentViewSnapshot view = {0};
        strcpy(view.view_id, "welcome"); strcpy(view.title, "Welcome.c");
        strcpy(view.uri, "umicom://welcome"); strcpy(view.source_text, "Welcome\n");
        view.active = 1;
        CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(fixture->workbench), &view) == UMI_STATUS_OK);
        CHECK(umi_ui_workbench_activate_document(fixture->workbench, view.view_id) == UMI_STATUS_OK);
    }
    CHECK(umi_document_coordinator_create(fixture->store, fixture->workbench, NULL, &fixture->documents) == UMI_STATUS_OK);
    return 0;
}

static void Stop(Fixture *fixture)
{
    umi_document_coordinator_destroy(fixture->documents);
    umi_document_store_destroy(fixture->store);
    umi_ui_workbench_destroy(fixture->workbench);
    umi_command_registry_destroy(fixture->commands);
    if (fixture->createdRoot) (void)umi_fs_remove_tree(fixture->root);
}

/* Reproduce typing through the same toolkit-neutral draft as the GTK adapter. */
static int Edit(Fixture *fixture, const char *text, int dirty)
{
    UmiDocumentWorkingCopySnapshot snapshot;
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &snapshot) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(fixture->workbench), snapshot.view_id, &view) == UMI_STATUS_OK);
    CHECK(strlen(text) < sizeof view.source_text);
    strcpy(view.source_text, text); view.dirty = dirty;
    CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(fixture->workbench), &view) == UMI_STATUS_OK);
    return 0;
}


/* Read the selected presentation without synchronising its pending text. */
static int ReadView(Fixture *fixture, UmiUiDocumentViewSnapshot *view)
{
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &snapshot) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(fixture->workbench), snapshot.view_id, view) == UMI_STATUS_OK);
    return 0;
}

/* Each case begins with one saved file and then exercises the public contract. */
static int Navigation(Fixture *fixture, const char *name)
{
    UmiUiDocumentViewSnapshot view, before;
    UmiDocumentWorkingCopySnapshot original, after;
    size_t offset = 999U, count = 999U;
    int wrapped = -1;
    CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &original) == UMI_STATUS_OK);
    CHECK(Edit(fixture, "note note note", 1) == 0);
    if (strcmp(name, "nav-forward") == 0) {
        for (size_t n = 0; n < 3U; ++n) {
            CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "note", 0, &offset, &wrapped) == UMI_STATUS_OK);
            CHECK(offset == n * 5U && wrapped == 0);
        }
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "note", 0, &offset, &wrapped) == UMI_STATUS_OK);
        CHECK(offset == 0 && wrapped == 1);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK);
        CHECK(after.revision == original.revision && after.undo_count == 0U);
        return 0;
    }
    if (strcmp(name, "nav-backward") == 0) {
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "note", 1, &offset, &wrapped) == UMI_STATUS_OK);
        CHECK(offset == 10U && wrapped == 1);
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "note", 1, &offset, &wrapped) == UMI_STATUS_OK);
        CHECK(offset == 5U && wrapped == 0);
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "note", 1, &offset, &wrapped) == UMI_STATUS_OK);
        CHECK(offset == 0 && wrapped == 0);
        return 0;
    }
    if (strcmp(name, "nav-not-found") == 0) {
        CHECK(ReadView(fixture, &before) == 0);
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "absent", 0, &offset, &wrapped) == UMI_STATUS_NOT_FOUND);
        CHECK(offset == 999U && wrapped == -1);
        CHECK(ReadView(fixture, &view) == 0 && memcmp(&view, &before, sizeof view) == 0);
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "", 0, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(name, "nav-smart-case") == 0) {
        CHECK(Edit(fixture, "Notes notes NOTES", 1) == 0);
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "notes", 0, &offset, NULL) == UMI_STATUS_OK && offset == 0U);
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "notes", 0, &offset, NULL) == UMI_STATUS_OK && offset == 6U);
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "Notes", 0, &offset, &wrapped) == UMI_STATUS_OK && offset == 0U && wrapped);
        return 0;
    }
    if (strcmp(name, "nav-utf8-offset") == 0) {
        CHECK(Edit(fixture, "\xc2\xa3 note \xc3\xa9 note", 1) == 0);
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "note", 0, &offset, NULL) == UMI_STATUS_OK && offset == 3U);
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "note", 0, &offset, NULL) == UMI_STATUS_OK && offset == 11U);
        return 0;
    }
    if (strcmp(name, "nav-replace-selection") == 0) {
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "note", 0, NULL, NULL) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorFindNext(fixture->documents, "note", 0, NULL, NULL) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorReplaceNext(fixture->documents, "note", "memo", &offset) == UMI_STATUS_OK && offset == 5U);
        CHECK(ReadView(fixture, &view) == 0 && strcmp(view.source_text, "note memo note") == 0);
        CHECK(umi_document_coordinator_undo(fixture->documents) == UMI_STATUS_OK);
        CHECK(ReadView(fixture, &view) == 0 && strcmp(view.source_text, "note note note") == 0);
        return 0;
    }
    if (strcmp(name, "nav-replace-advance") == 0) {
        CHECK(UmiDocumentCoordinatorReplaceNext(fixture->documents, "note", "NOTE", &offset) == UMI_STATUS_OK && offset == 0U);
        CHECK(UmiDocumentCoordinatorReplaceNext(fixture->documents, "note", "NOTE", &offset) == UMI_STATUS_OK && offset == 5U);
        CHECK(ReadView(fixture, &view) == 0 && strcmp(view.source_text, "NOTE NOTE note") == 0);
        return 0;
    }
    if (strcmp(name, "nav-replace-delete") == 0) {
        CHECK(UmiDocumentCoordinatorReplaceNext(fixture->documents, "note", "", &offset) == UMI_STATUS_OK);
        CHECK(ReadView(fixture, &view) == 0 && strcmp(view.source_text, " note note") == 0);
        CHECK(UmiDocumentCoordinatorReplaceNext(fixture->documents, "note", "", &offset) == UMI_STATUS_OK && offset == 1U);
        CHECK(ReadView(fixture, &view) == 0 && strcmp(view.source_text, "  note") == 0);
        return 0;
    }
    if (strcmp(name, "nav-replace-all-undo") == 0) {
        CHECK(umi_document_coordinator_sync_active(fixture->documents) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &original) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorReplaceAll(fixture->documents, "note", "memo", &count) == UMI_STATUS_OK && count == 3U);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK && after.undo_count == original.undo_count + 1U);
        CHECK(ReadView(fixture, &view) == 0 && strcmp(view.source_text, "memo memo memo") == 0);
        CHECK(umi_document_coordinator_undo(fixture->documents) == UMI_STATUS_OK);
        CHECK(ReadView(fixture, &view) == 0 && strcmp(view.source_text, "note note note") == 0);
        CHECK(umi_document_coordinator_redo(fixture->documents) == UMI_STATUS_OK);
        CHECK(ReadView(fixture, &view) == 0 && strcmp(view.source_text, "memo memo memo") == 0);
        return 0;
    }
    if (strcmp(name, "nav-replace-all-noop") == 0) {
        CHECK(ReadView(fixture, &before) == 0);
        CHECK(UmiDocumentCoordinatorReplaceAll(fixture->documents, "missing", "memo", &count) == UMI_STATUS_OK && count == 0);
        CHECK(ReadView(fixture, &view) == 0 && memcmp(&view, &before, sizeof view) == 0);
        CHECK(UmiDocumentCoordinatorReplaceAll(fixture->documents, "note", "note", &count) == UMI_STATUS_OK && count == 3U);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK);
        CHECK(after.revision == original.revision && after.undo_count == 0);
        return 0;
    }
    if (strcmp(name, "nav-replace-all-capacity") == 0) {
        char *replacement = malloc(UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U);
        CHECK(replacement != NULL);
        memset(replacement, 'x', UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES);
        replacement[UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES] = '\0';
        CHECK(ReadView(fixture, &before) == 0);
        UmiStatus rejected = UmiDocumentCoordinatorReplaceAll(fixture->documents, "note", replacement, &count);
        free(replacement);
        CHECK(rejected == UMI_STATUS_CAPACITY_EXCEEDED && count == 999U);
        CHECK(ReadView(fixture, &view) == 0 && memcmp(&view, &before, sizeof view) == 0);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK && after.revision == original.revision && after.undo_count == 0);
        return 0;
    }
    if (strcmp(name, "nav-read-only-history") == 0) {
        CHECK(UmiDocumentCoordinatorReplaceAll(fixture->documents, "note", "memo", NULL) == UMI_STATUS_OK);
        CHECK(ReadView(fixture, &view) == 0); view.read_only = 1;
        CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(fixture->workbench), &view) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &original) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorReplaceAll(fixture->documents, "memo", "note", NULL) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(umi_document_coordinator_undo(fixture->documents) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(umi_document_coordinator_redo(fixture->documents) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK && after.undo_count == original.undo_count && after.revision == original.revision);
        return 0;
    }
    if (strcmp(name, "nav-close-captured") == 0 || strcmp(name, "nav-close-denied") == 0) {
        CHECK(umi_document_coordinator_new(fixture->documents, "other.c", NULL, 0U) == UMI_STATUS_OK);
        UmiDocumentWorkingCopySnapshot other;
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &other) == UMI_STATUS_OK);
        CHECK(Edit(fixture, "other draft", 1) == 0);
        int force = strcmp(name, "nav-close-captured") == 0;
        CHECK(UmiDocumentCoordinatorClose(fixture->documents, original.document_id, force) == (force ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE));
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK && after.document_id == other.document_id);
        CHECK(ReadView(fixture, &view) == 0 && strcmp(view.source_text, "other draft") == 0);
        CHECK(umi_document_coordinator_count(fixture->documents) == (force ? 1U : 2U));
        if (force) CHECK(UmiDocumentCoordinatorClose(fixture->documents, original.document_id, 1) == UMI_STATUS_NOT_FOUND);
        return 0;
    }
    if (strcmp(name, "nav-reopen-small-output") == 0) {
        char tiny[2];
        CHECK(umi_document_coordinator_new(fixture->documents, "other.c", NULL, 0U) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &original) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_open(fixture->documents, fixture->path, tiny, sizeof tiny) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK && after.document_id == original.document_id);
        CHECK(umi_document_coordinator_count(fixture->documents) == 2U);
        return 0;
    }
    if (strcmp(name, "nav-save-clean-marker") == 0) {
        CHECK(Edit(fixture, "int notes = 1;\n", 1) == 0);
        CHECK(UmiDocumentCoordinatorSaveAll(fixture->documents, &count) == UMI_STATUS_OK && count == 0U);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK && !after.dirty);
        CHECK(ReadView(fixture, &view) == 0 && !view.dirty);
        return 0;
    }
    if (strcmp(name, "nav-history-capacity") == 0) {
        char *large = malloc(UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 2U);
        CHECK(large != NULL);
        memset(large, 'x', UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U);
        large[UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U] = '\0';
        UmiStatus stored = umi_document_store_replace_text(fixture->store, original.document_id,
            large, UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U);
        free(large);
        CHECK(stored == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_sync_active(fixture->documents) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &original) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_undo(fixture->documents) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK && after.undo_count == original.undo_count && after.revision == original.revision);
        CHECK(ReadView(fixture, &view) == 0 && strcmp(view.source_text, "note note note") == 0);
        return 0;
    }
    if (strncmp(name, "nav-close-guard-", 16) == 0) {
        UmiIdeActiveContext *context = calloc(1U, sizeof *context);
        UmiStudioRuntimeCloseReport report = {0};
        CHECK(context != NULL);
        if (strcmp(name, "nav-close-guard-missing") == 0) {
            CHECK(umi_ui_document_view_model_remove(umi_ui_workbench_documents(fixture->workbench), original.view_id) == UMI_STATUS_OK);
            UmiStatus status = UmiStudioCloseGuardEvaluateWithActivity(fixture->documents, context, 0, &report);
            free(context);
            CHECK(status == UMI_STATUS_NOT_FOUND);
        } else {
            int clean = strcmp(name, "nav-close-guard-build") == 0;
            if (clean) CHECK(Edit(fixture, "int notes = 1;\n", 0) == 0);
            UmiStatus status = UmiStudioCloseGuardEvaluateWithActivity(fixture->documents, context, 1, &report);
            free(context);
            CHECK(status == UMI_STATUS_OK);
            CHECK(report.decision == (clean ? UMI_STUDIO_CLOSE_CONFIRM_RUNNING : UMI_STUDIO_CLOSE_CONFIRM_DIRTY_AND_RUNNING));
            CHECK(!report.tests_running && strstr(report.summary, "additionalActivityRunning=1") != NULL);
        }
        return 0;
    }
    return 2;
}

static int Run(Fixture *fixture, const char *name)
{
    UmiDocumentWorkingCopySnapshot first, after;
    UmiUiDocumentViewSnapshot view;
    char out[UMI_UI_ID_CAPACITY];
    UmiStatus status;
    if (strcmp(name, "new-small-output") == 0 || strcmp(name, "open-small-output") == 0) {
        char tiny[2] = "x";
        CHECK(umi_fs_write_text(fixture->path, "int notes = 1;\n") == UMI_STATUS_OK);
        status = strcmp(name, "new-small-output") == 0
            ? umi_document_coordinator_new(fixture->documents, "notes.c", tiny, sizeof tiny)
            : umi_document_coordinator_open(fixture->documents, fixture->path, tiny, sizeof tiny);
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_document_coordinator_count(fixture->documents) == 0U);
        CHECK(umi_document_store_count(fixture->store) == 0U);
        CHECK(umi_ui_document_view_model_count(umi_ui_workbench_documents(fixture->workbench)) == 0U);
        return 0;
    }
    if (strcmp(name, "view-capacity-rollback") == 0) {
        memset(&view, 0, sizeof view);
        for (size_t n = 0; n < UMI_UI_DOCUMENT_VIEW_MAX; ++n) {
            CHECK(snprintf(view.view_id, sizeof view.view_id, "unmanaged.%zu", n) > 0);
            CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(fixture->workbench), &view) == UMI_STATUS_OK);
        }
        CHECK(umi_document_coordinator_new(fixture->documents, "notes.c", NULL, 0) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_document_coordinator_count(fixture->documents) == 0U);
        CHECK(umi_document_store_count(fixture->store) == 0U);
        return 0;
    }
    if (strcmp(name, "view-id-collision") == 0) {
        UmiUiDocumentViewSnapshot existing = {0};
        strcpy(existing.view_id, "umicom.document.view.1");
        strcpy(existing.source_text, "contributed view");
        CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(fixture->workbench), &existing) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_new(fixture->documents, "notes.c", NULL, 0U) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(umi_document_store_count(fixture->store) == 0U && umi_document_coordinator_count(fixture->documents) == 0U);
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(fixture->workbench), existing.view_id, &view) == UMI_STATUS_OK);
        CHECK(strcmp(view.source_text, "contributed view") == 0);
        return 0;
    }
    if (strcmp(name, "view-field-boundaries") == 0) {
        UmiUiDocumentViewSnapshot existing = {0};
        strcpy(existing.view_id, "bounded.view"); strcpy(existing.source_text, "preserved");
        CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(fixture->workbench), &existing) == UMI_STATUS_OK);
        for (size_t field = 0; field < 8U; ++field) {
            view = existing;
            char *fields[] = {view.view_id, view.document_id, view.title, view.icon_name, view.uri, view.language_id, view.source_text, view.group_id};
            const size_t sizes[] = {sizeof view.view_id, sizeof view.document_id, sizeof view.title, sizeof view.icon_name, sizeof view.uri, sizeof view.language_id, sizeof view.source_text, sizeof view.group_id};
            memset(fields[field], 'x', sizes[field]);
            uint64_t revision = umi_ui_document_view_model_revision(umi_ui_workbench_documents(fixture->workbench));
            CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(fixture->workbench), &view) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_ui_document_view_model_revision(umi_ui_workbench_documents(fixture->workbench)) == revision);
            CHECK(umi_ui_document_view_model_count(umi_ui_workbench_documents(fixture->workbench)) == 1U);
        }
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(fixture->workbench), existing.view_id, &view) == UMI_STATUS_OK);
        CHECK(strcmp(view.source_text, "preserved") == 0);
        return 0;
    }
    if (strncmp(name, "view-unterminated", 17) == 0) {
        memset(&view, 0, sizeof view); strcpy(view.view_id, "bounded.view");
        memset(view.source_text, 'x', sizeof view.source_text);
        uint64_t revision = umi_ui_document_view_model_revision(umi_ui_workbench_documents(fixture->workbench));
        status = strcmp(name, "view-unterminated-preview") == 0
            ? umi_ui_document_view_model_open_preview(umi_ui_workbench_documents(fixture->workbench), &view)
            : umi_ui_document_view_model_upsert(umi_ui_workbench_documents(fixture->workbench), &view);
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ui_document_view_model_revision(umi_ui_workbench_documents(fixture->workbench)) == revision);
        CHECK(umi_ui_document_view_model_count(umi_ui_workbench_documents(fixture->workbench)) == 0);
        return 0;
    }
    if (strcmp(name, "virtual-draft") == 0) {
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &first) == UMI_STATUS_OK);
        CHECK(!first.dirty);
        CHECK(Edit(fixture, "My project notes\n", 1) == 0);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK);
        CHECK(after.dirty && after.text_length == strlen("My project notes\n"));
        return 0;
    }
    CHECK(umi_fs_write_text(fixture->path, "int notes = 1;\n") == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_open(fixture->documents, fixture->path, out, sizeof out) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &first) == UMI_STATUS_OK);
    if (strncmp(name, "nav-", 4) == 0) return Navigation(fixture, name);
    if (strcmp(name, "draft-snapshot") == 0 || strcmp(name, "draft-without-flag") == 0 || strcmp(name, "close-guard-draft") == 0) {
        CHECK(Edit(fixture, "int notes = 250;\n", strcmp(name, "draft-without-flag") != 0) == 0);
        if (strcmp(name, "close-guard-draft") == 0) {
            UmiIdeActiveContext *context = calloc(1, sizeof *context);
            UmiStudioRuntimeCloseReport report;
            CHECK(context != NULL);
            status = umi_studio_close_guard_evaluate(fixture->documents, context, &report);
            free(context);
            CHECK(status == UMI_STATUS_OK && report.dirty_document_count == 1U && report.decision == UMI_STUDIO_CLOSE_CONFIRM_DIRTY);
        } else {
            CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK);
            CHECK(after.dirty && after.text_length == strlen("int notes = 250;\n"));
            CHECK(after.revision == first.revision); /* Reading a snapshot does not commit the draft. */
        }
        return 0;
    }
    if (strncmp(name, "reopen-", 7) == 0) {
        char alias[UMI_PATH_CAPACITY];
        CHECK(Edit(fixture, "int notes = 250;\n", 1) == 0);
        CHECK(umi_document_coordinator_new(fixture->documents, "other.c", NULL, 0) == UMI_STATUS_OK);
        CHECK(snprintf(alias, sizeof alias, "%s/./notes.c", fixture->root) > 0);
        if (strcmp(name, "reopen-missing") == 0) CHECK(remove(fixture->path) == 0);
        CHECK(umi_document_coordinator_open(fixture->documents,
            strcmp(name, "reopen-alias") == 0 ? alias : fixture->path, out, sizeof out) == UMI_STATUS_OK);
        CHECK(strcmp(out, first.view_id) == 0 && umi_document_coordinator_count(fixture->documents) == 2U);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK);
        CHECK(after.document_id == first.document_id);
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(fixture->workbench), out, &view) == UMI_STATUS_OK);
        CHECK(strcmp(view.source_text, "int notes = 250;\n") == 0 && view.dirty);
        return 0;
    }
    if (strcmp(name, "replace-null") == 0) {
        CHECK(umi_document_coordinator_replace(fixture->documents, "notes", NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK);
        CHECK(after.revision == first.revision && !after.dirty && after.undo_count == first.undo_count);
        return 0;
    }
    if (strcmp(name, "replace-capacity") == 0) {
        char *replacement = malloc(UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U);
        char *text = NULL; size_t length;
        CHECK(replacement != NULL);
        memset(replacement, 'a', UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES);
        replacement[UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES] = '\0';
        UmiStatus rejected = umi_document_coordinator_replace(fixture->documents, "notes", replacement, NULL);
        free(replacement);
        CHECK(rejected == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_document_store_copy_text(fixture->store, first.document_id, &text, &length) == UMI_STATUS_OK);
        int unchanged = strcmp(text, "int notes = 1;\n") == 0;
        umi_document_store_free_text(text);
        CHECK(unchanged);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK);
        CHECK(after.revision == first.revision && after.undo_count == first.undo_count && !after.dirty);
        return 0;
    }
    if (strcmp(name, "replace-read-only") == 0) {
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(fixture->workbench), first.view_id, &view) == UMI_STATUS_OK);
        view.read_only = 1;
        CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(fixture->workbench), &view) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_replace(fixture->documents, "notes", "balance", NULL) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(umi_document_coordinator_active_snapshot(fixture->documents, &after) == UMI_STATUS_OK);
        CHECK(after.revision == first.revision && !after.dirty);
        return 0;
    }
    return 2;
}

int main(int argc, char **argv)
{
    Fixture fixture = {0};
    if (argc != 2) return 2;
    int result = Start(&fixture, argv[1]);
    if (result == 0) result = Run(&fixture, argv[1]);
    Stop(&fixture);
    return result;
}
