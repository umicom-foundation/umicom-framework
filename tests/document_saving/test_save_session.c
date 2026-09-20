/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_saving/test_save_session.c
 * PURPOSE: Exercise Save All and history against real documents and local files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/save_session.h"
#include "umicom/document/edit.h"
#include "umicom/document/local_provider.h"
#include "umicom/platform/filesystem.h"
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
    UmiDocumentSaveSession *session;
    char root[UMI_PATH_CAPACITY];
    char path[3][UMI_PATH_CAPACITY];
    char view[3][UMI_UI_ID_CAPACITY];
    UmiDocumentId id[3];
    int madeRoot;
} Fixture;
static int Start(Fixture *f, const char *name)
{
    char temp[UMI_PATH_CAPACITY], leaf[160];
    CHECK(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
    CHECK(snprintf(leaf, sizeof(leaf), "umicom-save-session-%ld-%s", (long)PROCESS_ID(), name) > 0);
    CHECK(umi_fs_join(f->root, sizeof(f->root), temp, leaf) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(f->root));
    CHECK(umi_fs_make_directories(f->root) == UMI_STATUS_OK); f->madeRoot = 1;
    CHECK(umi_command_registry_create(&f->commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.document.saving", f->commands, &f->workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&f->store) == UMI_STATUS_OK);
    UmiDocumentProvider provider = umi_document_local_provider();
    CHECK(umi_document_coordinator_create(f->store, f->workbench, &provider, &f->documents) == UMI_STATUS_OK);
    return 0;
}
static void Stop(Fixture *f)
{
    UmiDocumentSaveSessionDestroy(f->session);
    umi_document_coordinator_destroy(f->documents);
    umi_document_store_destroy(f->store);
    umi_ui_workbench_destroy(f->workbench);
    umi_command_registry_destroy(f->commands);
    if (f->madeRoot) (void)umi_fs_remove_tree(f->root);
}
static int Add(Fixture *f, size_t slot, int named)
{
    char leaf[64];
    CHECK(snprintf(leaf, sizeof(leaf), "notes-%zu.c", slot) > 0);
    CHECK(umi_fs_join(f->path[slot], sizeof(f->path[slot]), f->root, leaf) == UMI_STATUS_OK);
    if (named) {
        CHECK(umi_fs_write_text(f->path[slot], "original\n") == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_open(f->documents, f->path[slot], f->view[slot], sizeof(f->view[slot])) == UMI_STATUS_OK);
    } else CHECK(umi_document_coordinator_new(f->documents, leaf, f->view[slot], sizeof(f->view[slot])) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &snapshot) == UMI_STATUS_OK);
    f->id[slot] = snapshot.document_id;
    return 0;
}
static int Edit(Fixture *f, size_t slot, const char *text)
{
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), f->view[slot], &view) == UMI_STATUS_OK);
    view.dirty = 1; view.cursor_offset = 0U; view.selection_length = 0U;
    CHECK(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(f->workbench), &view, text, strlen(text)) == UMI_STATUS_OK);
    return 0;
}
static int Text(Fixture *f, size_t slot, const char *expected, int disk)
{
    char *text = NULL; size_t bytes = 0U;
    UmiStatus status = disk ? umi_fs_read_text(f->path[slot], &text, &bytes) :
        UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(f->workbench), f->view[slot], &text, &bytes);
    CHECK(status == UMI_STATUS_OK);
    int same = bytes == strlen(expected) && memcmp(text, expected, bytes) == 0;
    if (disk) free(text); else UmiUiDocumentViewModelFreeText(text);
    CHECK(same); return 0;
}
static int Begin(Fixture *f)
{
    CHECK(UmiDocumentSaveSessionCreate(f->documents, &f->session) == UMI_STATUS_OK); return 0;
}
static int Progress(Fixture *f, UmiDocumentSavePhase phase, size_t saved, size_t remaining)
{
    UmiDocumentSaveProgress p;
    CHECK(UmiDocumentSaveSessionProgress(f->session, &p) == UMI_STATUS_OK);
    CHECK(p.phase == phase && p.saved == saved && p.remaining == remaining); return 0;
}
static int Run(Fixture *f, const char *name)
{
    if (strcmp(name, "cancel-waiting-path") == 0 || strcmp(name, "cancel-retry") == 0) {
        CHECK(Add(f, 0U, 1) == 0); CHECK(Edit(f, 0U, "saved first\n") == 0);
        CHECK(Add(f, 1U, 0) == 0); CHECK(Edit(f, 1U, "header draft\n") == 0);
        CHECK(Add(f, 2U, 1) == 0); CHECK(Edit(f, 2U, "last draft\n") == 0);
        CHECK(Begin(f) == 0);
        CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_OK);
        CHECK(Progress(f, UMI_DOCUMENT_SAVE_NEEDS_PATH, 1U, 2U) == 0);
        CHECK(UmiDocumentSaveSessionCancel(f->session) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionCancel(f->session) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionProvidePath(f->session, f->path[1]) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_CANCELLED);
        CHECK(Progress(f, UMI_DOCUMENT_SAVE_CANCELLED, 1U, 2U) == 0);
        CHECK(Text(f, 0U, "saved first\n", 1) == 0);
        CHECK(Text(f, 1U, "header draft\n", 0) == 0);
        CHECK(!umi_fs_exists(f->path[1]));
        CHECK(Text(f, 2U, "original\n", 1) == 0);
        if (strcmp(name, "cancel-retry") == 0) {
            UmiDocumentSaveSessionDestroy(f->session); f->session = NULL;
            CHECK(Begin(f) == 0);
            CHECK(Progress(f, UMI_DOCUMENT_SAVE_READY, 0U, 2U) == 0);
            CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_OK);
            CHECK(UmiDocumentSaveSessionProvidePath(f->session, f->path[1]) == UMI_STATUS_OK);
            CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_OK);
            CHECK(Progress(f, UMI_DOCUMENT_SAVE_COMPLETE, 2U, 0U) == 0);
            CHECK(Text(f, 1U, "header draft\n", 1) == 0);
            CHECK(Text(f, 2U, "last draft\n", 1) == 0);
        }
        UmiDocumentWorkingCopySnapshot active;
        CHECK(umi_document_coordinator_active_snapshot(f->documents, &active) == UMI_STATUS_OK);
        CHECK(active.document_id == f->id[2]);
        return 0;
    }
    if (strcmp(name, "cancel-complete") == 0 || strcmp(name, "cancel-failed") == 0 ||
        strcmp(name, "progress-copy") == 0) {
        CHECK(Add(f, 0U, 1) == 0); CHECK(Edit(f, 0U, "new Notes text\n") == 0);
        CHECK(Begin(f) == 0);
        UmiDocumentSaveProgress copy;
        CHECK(UmiDocumentSaveSessionProgress(f->session, &copy) == UMI_STATUS_OK);
        copy.total = 999U; copy.remaining = 999U; copy.phase = UMI_DOCUMENT_SAVE_COMPLETE;
        CHECK(Progress(f, UMI_DOCUMENT_SAVE_READY, 0U, 1U) == 0);
        int failed = strcmp(name, "cancel-failed") == 0;
        if (failed) CHECK(umi_fs_write_text(f->path[0], "external writer\n") == UMI_STATUS_OK);
        UmiStatus expected = failed ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
        CHECK(UmiDocumentSaveSessionStep(f->session) == expected);
        CHECK(UmiDocumentSaveSessionCancel(f->session) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionCancel(f->session) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionStep(f->session) == expected);
        CHECK(Progress(f, failed ? UMI_DOCUMENT_SAVE_FAILED : UMI_DOCUMENT_SAVE_COMPLETE,
            failed ? 0U : 1U, failed ? 1U : 0U) == 0);
        CHECK(Text(f, 0U, failed ? "external writer\n" : "new Notes text\n", 1) == 0);
        return 0;
    }
    if (strcmp(name, "invalid-api") == 0) {
        CHECK(UmiDocumentSaveSessionCreate(NULL, &f->session) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f->session == NULL);
        CHECK(UmiDocumentSaveSessionCreate(f->documents, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSaveSessionStep(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSaveSessionProvidePath(NULL, "x") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSaveSessionCancel(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        UmiDocumentSaveProgress p = {0}; p.saved = 7U;
        CHECK(UmiDocumentSaveSessionProgress(NULL, &p) == UMI_STATUS_INVALID_ARGUMENT && p.saved == 7U);
        UmiDocumentSaveSessionDestroy(NULL); return 0;
    }
    if (strcmp(name, "empty") == 0 || strcmp(name, "clean") == 0) {
        if (strcmp(name, "clean") == 0) CHECK(Add(f, 0U, 1) == 0);
        CHECK(Begin(f) == 0); CHECK(Progress(f, UMI_DOCUMENT_SAVE_COMPLETE, 0U, 0U) == 0);
        CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionCancel(f->session) == UMI_STATUS_OK);
        CHECK(Progress(f, UMI_DOCUMENT_SAVE_COMPLETE, 0U, 0U) == 0); return 0;
    }
    int untitled = strncmp(name, "waiting-", 8U) == 0 || strcmp(name, "untitled") == 0 || strcmp(name, "bad-path") == 0 ||
        strcmp(name, "current-draft") == 0 || strcmp(name, "other-destination") == 0;
    CHECK(Add(f, 0U, !untitled) == 0); CHECK(Edit(f, 0U, "first draft\n") == 0);
    if (strcmp(name, "legacy-preflight") == 0) {
        CHECK(Add(f, 1U, 0) == 0); CHECK(Edit(f, 1U, "new header\n") == 0);
        size_t saved = 99U;
        CHECK(UmiDocumentCoordinatorSaveAll(f->documents, &saved) == UMI_STATUS_INVALID_STATE);
        CHECK(saved == 0U && Text(f, 0U, "original\n", 1) == 0);
        return 0;
    }
    if (strcmp(name, "mixed") == 0) {
        CHECK(Add(f, 1U, 0) == 0); CHECK(Edit(f, 1U, "untitled draft\n") == 0);
        CHECK(Add(f, 2U, 1) == 0); CHECK(Edit(f, 2U, "third draft\n") == 0);
    }
    if (strcmp(name, "cancel-after") == 0 || strcmp(name, "partial-failure") == 0) {
        CHECK(Add(f, 1U, 1) == 0); CHECK(Edit(f, 1U, "second draft\n") == 0);
    }
    if (strcmp(name, "history-target") == 0) {
        CHECK(UmiDocumentCoordinatorSyncDocument(f->documents, f->id[0]) == UMI_STATUS_OK);
        CHECK(Add(f, 1U, 1) == 0); CHECK(Edit(f, 1U, "second history\n") == 0);
        CHECK(UmiDocumentCoordinatorSyncDocument(f->documents, f->id[1]) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorUndo(f->documents, f->id[0]) == UMI_STATUS_OK);
        CHECK(Text(f, 0U, "original\n", 0) == 0); CHECK(Text(f, 1U, "second history\n", 0) == 0);
        CHECK(UmiDocumentCoordinatorRedo(f->documents, f->id[0]) == UMI_STATUS_OK);
        CHECK(Text(f, 0U, "first draft\n", 0) == 0); CHECK(Text(f, 1U, "second history\n", 0) == 0);
        UmiDocumentWorkingCopySnapshot active;
        CHECK(umi_document_coordinator_active_snapshot(f->documents, &active) == UMI_STATUS_OK && active.document_id == f->id[1]);
        CHECK(umi_document_coordinator_undo(f->documents) == UMI_STATUS_OK);
        CHECK(Text(f, 1U, "original\n", 0) == 0);
        CHECK(umi_document_coordinator_redo(f->documents) == UMI_STATUS_OK);
        CHECK(Text(f, 1U, "second history\n", 0) == 0);
        CHECK(Text(f, 0U, "original\n", 1) == 0); return 0;
    }
    CHECK(Begin(f) == 0);
    if (strcmp(name, "cancel-before") == 0) {
        CHECK(UmiDocumentSaveSessionCancel(f->session) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_CANCELLED);
        CHECK(Progress(f, UMI_DOCUMENT_SAVE_CANCELLED, 0U, 1U) == 0);
        CHECK(Text(f, 0U, "original\n", 1) == 0); return 0;
    }
    if (strcmp(name, "closed") == 0) {
        CHECK(UmiDocumentCoordinatorClose(f->documents, f->id[0], 1) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_NOT_FOUND);
        CHECK(Progress(f, UMI_DOCUMENT_SAVE_FAILED, 0U, 1U) == 0); return 0;
    }
    if (strcmp(name, "renamed") == 0) {
        char path[UMI_PATH_CAPACITY]; CHECK(umi_fs_join(path, sizeof(path), f->root, "renamed.c") == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorSaveAs(f->documents, f->id[0], path) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_INVALID_STATE);
        CHECK(Text(f, 0U, "original\n", 1) == 0); return 0;
    }
    if (strcmp(name, "already-saved") == 0) {
        CHECK(UmiDocumentCoordinatorSaveAs(f->documents, f->id[0], f->path[0]) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_OK);
        CHECK(Progress(f, UMI_DOCUMENT_SAVE_COMPLETE, 0U, 0U) == 0);
        UmiDocumentSaveProgress p; CHECK(UmiDocumentSaveSessionProgress(f->session, &p) == UMI_STATUS_OK && p.unchanged == 1U); return 0;
    }
    if (strcmp(name, "readonly") == 0) {
        UmiUiDocumentViewSnapshot view;
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), f->view[0], &view) == UMI_STATUS_OK);
        view.read_only = 1;
        CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f->workbench), &view) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(Text(f, 0U, "original\n", 1) == 0); return 0;
    }
    if (strcmp(name, "external-conflict") == 0) {
        CHECK(umi_fs_write_text(f->path[0], "external contents\n") == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_INVALID_STATE);
        CHECK(Text(f, 0U, "external contents\n", 1) == 0);
        CHECK(Text(f, 0U, "first draft\n", 0) == 0); return 0;
    }
    if (strcmp(name, "new-document") == 0 || strcmp(name, "other-tab") == 0) {
        CHECK(Add(f, 1U, 0) == 0); CHECK(Edit(f, 1U, "other unsaved draft\n") == 0);
    }
    CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_OK);
    if (untitled) {
        CHECK(Progress(f, UMI_DOCUMENT_SAVE_NEEDS_PATH, 0U, 1U) == 0);
        CHECK(!umi_fs_exists(f->path[0]));
        if (strcmp(name, "waiting-closed") == 0) {
            CHECK(UmiDocumentCoordinatorClose(f->documents, f->id[0], 1) == UMI_STATUS_OK);
            CHECK(UmiDocumentSaveSessionProvidePath(f->session, f->path[0]) == UMI_STATUS_NOT_FOUND);
            CHECK(!umi_fs_exists(f->path[0])); return 0;
        }
        if (strcmp(name, "waiting-renamed") == 0) {
            char other[UMI_PATH_CAPACITY]; CHECK(umi_fs_join(other, sizeof(other), f->root, "chosen-elsewhere.c") == UMI_STATUS_OK);
            CHECK(UmiDocumentCoordinatorSaveAs(f->documents, f->id[0], other) == UMI_STATUS_OK);
            CHECK(UmiDocumentSaveSessionProvidePath(f->session, f->path[0]) == UMI_STATUS_INVALID_STATE);
            CHECK(!umi_fs_exists(f->path[0])); return 0;
        }
        if (strcmp(name, "waiting-other-tab") == 0) {
            CHECK(Add(f, 1U, 0) == 0); CHECK(Edit(f, 1U, "other unsaved draft\n") == 0);
            CHECK(UmiDocumentSaveSessionProvidePath(f->session, f->path[0]) == UMI_STATUS_OK);
            UmiDocumentWorkingCopySnapshot active;
            CHECK(umi_document_coordinator_active_snapshot(f->documents, &active) == UMI_STATUS_OK && active.document_id == f->id[1]);
            CHECK(Text(f, 0U, "first draft\n", 1) == 0); CHECK(Text(f, 1U, "other unsaved draft\n", 0) == 0);
            CHECK(!umi_fs_exists(f->path[1])); return 0;
        }
        if (strcmp(name, "bad-path") == 0) {
            CHECK(UmiDocumentSaveSessionProvidePath(f->session, "relative.c") == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDocumentSaveSessionProvidePath(f->session, "") == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(Progress(f, UMI_DOCUMENT_SAVE_NEEDS_PATH, 0U, 1U) == 0);
            CHECK(UmiDocumentSaveSessionCancel(f->session) == UMI_STATUS_OK);
            CHECK(UmiDocumentSaveSessionProvidePath(f->session, f->path[0]) == UMI_STATUS_INVALID_STATE); return 0;
        }
        if (strcmp(name, "other-destination") == 0) {
            CHECK(Add(f, 1U, 1) == 0);
            CHECK(UmiDocumentSaveSessionProvidePath(f->session, f->path[1]) == UMI_STATUS_ALREADY_EXISTS);
            CHECK(Text(f, 1U, "original\n", 1) == 0); return 0;
        }
        if (strcmp(name, "current-draft") == 0) CHECK(Edit(f, 0U, "latest draft\n") == 0);
        CHECK(UmiDocumentSaveSessionProvidePath(f->session, f->path[0]) == UMI_STATUS_OK);
        CHECK(Text(f, 0U, strcmp(name,"current-draft")==0 ? "latest draft\n" : "first draft\n", 1) == 0);
    } else CHECK(Text(f, 0U, "first draft\n", 1) == 0);
    if (strcmp(name, "cancel-after") == 0 || strcmp(name, "partial-failure") == 0) {
        CHECK(Progress(f, UMI_DOCUMENT_SAVE_READY, 1U, 1U) == 0);
        if (strcmp(name, "cancel-after") == 0) {
            CHECK(UmiDocumentSaveSessionCancel(f->session) == UMI_STATUS_OK);
            CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_CANCELLED);
        } else {
            CHECK(umi_fs_write_text(f->path[1], "external\n") == UMI_STATUS_OK);
            CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_INVALID_STATE);
        }
        CHECK(Text(f, 1U, "second draft\n", 0) == 0);
        CHECK(Text(f, 0U, "first draft\n", 1) == 0); return 0;
    }
    if (strcmp(name, "mixed") == 0) {
        CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_OK);
        CHECK(Progress(f, UMI_DOCUMENT_SAVE_NEEDS_PATH, 1U, 2U) == 0);
        CHECK(Text(f, 2U, "original\n", 1) == 0);
        CHECK(UmiDocumentSaveSessionProvidePath(f->session, f->path[1]) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_OK);
        CHECK(Progress(f, UMI_DOCUMENT_SAVE_COMPLETE, 3U, 0U) == 0);
        CHECK(Text(f, 1U, "untitled draft\n", 1) == 0); CHECK(Text(f, 2U, "third draft\n", 1) == 0); return 0;
    }
    if (strcmp(name, "new-document") == 0 || strcmp(name, "other-tab") == 0) {
        UmiDocumentWorkingCopySnapshot active;
        CHECK(umi_document_coordinator_active_snapshot(f->documents, &active) == UMI_STATUS_OK && active.document_id == f->id[1]);
        CHECK(Text(f, 1U, "other unsaved draft\n", 0) == 0); CHECK(!umi_fs_exists(f->path[1]));
    }
    CHECK(Progress(f, UMI_DOCUMENT_SAVE_COMPLETE, 1U, 0U) == 0);
    CHECK(UmiDocumentSaveSessionStep(f->session) == UMI_STATUS_OK);
    CHECK(UmiDocumentSaveSessionProvidePath(f->session, f->path[0]) == UMI_STATUS_INVALID_STATE);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    /* The five additional scenarios extend the existing fixture; the previous
     * case list is preserved below and every original name remains active. */
    // const char *cases[] = {"invalid-api","empty","clean","named","untitled","mixed", "cancel-before", "cancel-after", "closed", "renamed", "already-saved", "readonly", "external-conflict", "new-document", "other-tab", "bad-path", "other-destination", "current-draft", "history-target", "partial-failure", "waiting-closed", "waiting-renamed", "waiting-other-tab", "legacy-preflight"};
    const char *cases[] = {"cancel-waiting-path", "cancel-retry", "cancel-complete", "cancel-failed", "progress-copy", "invalid-api","empty","clean","named","untitled","mixed", "cancel-before", "cancel-after", "closed", "renamed", "already-saved", "readonly", "external-conflict", "new-document", "other-tab", "bad-path", "other-destination", "current-draft", "history-target", "partial-failure", "waiting-closed", "waiting-renamed", "waiting-other-tab", "legacy-preflight"};
    int known = 0; for (size_t i=0U; i<sizeof(cases)/sizeof(cases[0]); ++i) if (strcmp(argv[1],cases[i])==0) known=1;
    if (!known) return 2;
    Fixture fixture = {0};
    int result = Start(&fixture, argv[1]);
    if (result == 0) result = Run(&fixture, argv[1]);
    Stop(&fixture);
    return result;
}
