/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/editor_workflow/save_all.c
 *
 * PURPOSE:
 *   Save the two source documents of an Umicom Notes exercise using the real
 *   Framework coordinator. A header starts untitled; the caller supplies its
 *   filename at the same boundary used by the graphical Save All workflow.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/save_session.h"
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
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "Line %d: %s\n", __LINE__, #x); goto cleanup; } } while (0)

/* The view contains the editor draft. Updating it is deliberately separate
 * from saving: a successful edit must not imply that a file was written. */
static UmiStatus SetDraft(UmiUiWorkbench *workbench, const char *viewId, const char *text)
{
    UmiUiDocumentViewSnapshot view;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiStatus status = umi_ui_document_view_model_find(views, viewId, &view);
    if (status != UMI_STATUS_OK) return status;
    view.dirty = 1;
    return UmiUiDocumentViewModelUpsertText(views, &view, text, strlen(text));
}

/* Read back independently. A Save request is not proof that the desired bytes
 * reached this file; the comparison below checks the observable result. */
static int FileEquals(const char *path, const char *expected)
{
    char *text = NULL; size_t length = 0U;
    UmiStatus status = umi_fs_read_text(path, &text, &length);
    int same = status == UMI_STATUS_OK && length == strlen(expected) &&
        memcmp(text, expected, length) == 0;
    free(text);
    return same;
}

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiDocumentSaveSession *session = NULL;
    char temp[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], leaf[96];
    char sourcePath[UMI_PATH_CAPACITY], headerPath[UMI_PATH_CAPACITY];
    char sourceView[UMI_UI_ID_CAPACITY], headerView[UMI_UI_ID_CAPACITY];
    const char *sourceText = "/* Umicom Notes: count the stored notes. */\nint CountNotes(void) { return 2; }\n";
    const char *headerText = "/* Umicom Notes public declaration. */\nint CountNotes(void);\n";
    int created = 0, result = 1;

    /* Use an isolated practice directory. Never overwrite a real project. */
    REQUIRE(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
    REQUIRE(snprintf(leaf, sizeof(leaf), "umicom-notes-save-all-%ld", (long)PROCESS_ID()) > 0);
    REQUIRE(umi_fs_join(root, sizeof(root), temp, leaf) == UMI_STATUS_OK);
    REQUIRE(!umi_fs_exists(root));
    REQUIRE(umi_fs_make_directories(root) == UMI_STATUS_OK); created = 1;
    REQUIRE(umi_fs_join(sourcePath, sizeof(sourcePath), root, "notes.c") == UMI_STATUS_OK);
    REQUIRE(umi_fs_join(headerPath, sizeof(headerPath), root, "notes.h") == UMI_STATUS_OK);
    REQUIRE(umi_fs_write_text(sourcePath, "int CountNotes(void) { return 1; }\n") == UMI_STATUS_OK);

    REQUIRE(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workbench_create("umicom.notes.save-all", commands, &workbench) == UMI_STATUS_OK);
    REQUIRE(umi_document_store_create(&store) == UMI_STATUS_OK);
    UmiDocumentProvider provider = umi_document_local_provider();
    REQUIRE(umi_document_coordinator_create(store, workbench, &provider, &documents) == UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_open(documents, sourcePath, sourceView, sizeof(sourceView)) == UMI_STATUS_OK);
    REQUIRE(SetDraft(workbench, sourceView, sourceText) == UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_new(documents, "notes.h", headerView, sizeof(headerView)) == UMI_STATUS_OK);
    REQUIRE(SetDraft(workbench, headerView, headerText) == UMI_STATUS_OK);

    REQUIRE(UmiDocumentSaveSessionCreate(documents, &session) == UMI_STATUS_OK);
    UmiDocumentSaveProgress progress;
    for (;;) {
        REQUIRE(UmiDocumentSaveSessionProgress(session, &progress) == UMI_STATUS_OK);
        if (progress.phase == UMI_DOCUMENT_SAVE_COMPLETE) break;
        if (progress.phase == UMI_DOCUMENT_SAVE_NEEDS_PATH) {
            /* A GUI shows a filename chooser here. This exercise deliberately
             * supplies its own new temporary header destination, not user data. */
            printf("Choose a filename for %s\n", progress.display_name);
            REQUIRE(UmiDocumentSaveSessionProvidePath(session, headerPath) == UMI_STATUS_OK);
        } else {
            REQUIRE(progress.phase == UMI_DOCUMENT_SAVE_READY);
            REQUIRE(UmiDocumentSaveSessionStep(session) == UMI_STATUS_OK);
        }
    }
    REQUIRE(progress.saved == 2U && progress.remaining == 0U);
    REQUIRE(FileEquals(sourcePath, sourceText));
    REQUIRE(FileEquals(headerPath, headerText));
    UmiDocumentWorkingCopySnapshot active;
    REQUIRE(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    REQUIRE(strcmp(active.view_id, headerView) == 0);
    printf("Verified %zu saved Notes source files. The active document did not change.\n", progress.saved);
    result = 0;
cleanup:
    /* Cancel before release. Earlier completed saves are not rolled back. The
     * exercise removes only the temporary directory that it created above. */
    if (session != NULL) (void)UmiDocumentSaveSessionCancel(session);
    UmiDocumentSaveSessionDestroy(session);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    if (created) (void)umi_fs_remove_tree(root);
    return result;
}
