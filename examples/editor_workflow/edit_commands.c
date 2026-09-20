/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/editor_workflow/edit_commands.c
 *
 * PURPOSE:
 *   Edit a Notes source document, keep a second draft intact, undo and redo the
 *   change, then explicitly save and read the intended file back.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/edit.h"
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

/* A copied view string belongs to the caller until FreeText releases it. */
static UmiStatus CheckDraft(UmiUiDocumentViewModel *views, const char *viewId,
    const char *expected)
{
    char *text = NULL;
    size_t bytes = 0U;
    UmiStatus status = UmiUiDocumentViewModelCopyText(views, viewId, &text, &bytes);
    if (status == UMI_STATUS_OK &&
        (bytes != strlen(expected) || memcmp(text, expected, bytes) != 0))
        status = UMI_STATUS_INVALID_STATE;
    UmiUiDocumentViewModelFreeText(text);
    return status;
}

int main(void)
{
    const char *source = "#include <stdio.h>\nint main(void) { puts(\"Umicom Notes: 3 saved notes\"); return 0; }\n";
    const char *updated = "#include <stdio.h>\nint main(void) { puts(\"Umicom Notes: 5 saved notes\"); return 0; }\n";
    const char *otherDraft = "Review the note search before the next build.\n";
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiDocumentEditPlan *plan = NULL;
    UmiStatus status = UMI_STATUS_OK;
    char temp[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY] = {0};
    char path[UMI_PATH_CAPACITY], leaf[100];
    char sourceView[UMI_UI_ID_CAPACITY], otherView[UMI_UI_ID_CAPACITY];
    int ownsDirectory = 0;
    char *saved = NULL;

    /* TRY stops at the first failed contract and still visits cleanup. */
#define TRY(call) do { status = (call); if (status != UMI_STATUS_OK) goto cleanup; } while (0)
    (void)snprintf(leaf, sizeof leaf, "umicom-notes-edit-%ld", (long)PROCESS_ID());
    TRY(umi_fs_temp_directory(temp, sizeof temp));
    TRY(umi_fs_join(root, sizeof root, temp, leaf));
    if (umi_fs_exists(root)) { status = UMI_STATUS_ALREADY_EXISTS; goto cleanup; }
    TRY(umi_fs_make_directories(root));
    ownsDirectory = 1;
    TRY(umi_fs_join(path, sizeof path, root, "notes.c"));
    TRY(umi_fs_write_text(path, source));
    TRY(umi_command_registry_create(&commands));
    TRY(umi_ui_workbench_create("umicom.notes.editing", commands, &workbench));
    TRY(umi_document_store_create(&store));
    TRY(umi_document_coordinator_create(store, workbench, NULL, &documents));
    TRY(umi_document_coordinator_open(documents, path, sourceView, sizeof sourceView));

    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiDocumentWorkingCopySnapshot target, active;
    UmiUiDocumentViewSnapshot view;
    TRY(umi_document_coordinator_active_snapshot(documents, &target));
    UmiDocumentEditCommand command;
    TRY(UmiDocumentEditCommandFromId("edit.paste", &command));
    if (command != UMI_DOCUMENT_EDIT_PASTE) { status = UMI_STATUS_INVALID_STATE; goto cleanup; }
    TRY(UmiDocumentCoordinatorSelectAll(documents, target.document_id));
    UmiDocumentEditState editState;
    TRY(UmiDocumentCoordinatorGetEditState(documents, target.document_id, &editState));
    if (editState.selection_bytes != strlen(source)) { status = UMI_STATUS_INVALID_STATE; goto cleanup; }
    TRY(umi_ui_document_view_model_find(views, sourceView, &view));
    /* This ASCII digit occupies one byte. General UTF-8 selections must end
     * on complete characters, even when one character uses several bytes. */
    const char *digit = strstr(source, "3 saved");
    if (digit == NULL) { status = UMI_STATUS_INVALID_STATE; goto cleanup; }
    view.cursor_offset = (size_t)(digit - source);
    view.selection_length = 1U;
    TRY(umi_ui_document_view_model_upsert(views, &view));
    TRY(UmiDocumentCoordinatorPrepareEdit(documents, target.document_id, &plan));
    const char *selection = NULL;
    size_t selectedBytes = 0U;
    TRY(UmiDocumentEditPlanSelection(plan, &selection, &selectedBytes));
    if (selectedBytes != 1U || selection[0] != '3') {
        status = UMI_STATUS_INVALID_STATE; goto cleanup;
    }

    /* Model a user changing tabs while clipboard text is being delivered.
     * The example supplies that text directly; it does not read the clipboard. */
    TRY(umi_document_coordinator_new(documents, "Review notes", otherView, sizeof otherView));
    TRY(umi_ui_document_view_model_find(views, otherView, &view));
    view.dirty = 1;
    TRY(UmiUiDocumentViewModelUpsertText(views, &view, otherDraft, strlen(otherDraft)));
    TRY(umi_document_coordinator_active_snapshot(documents, &active));
    TRY(UmiDocumentCoordinatorSyncDocument(documents, active.document_id));
    TRY(UmiDocumentCoordinatorApplyEdit(documents, plan, "5", 1U));
    UmiDocumentEditPlanDestroy(plan);
    plan = NULL;
    TRY(CheckDraft(views, sourceView, updated));
    TRY(CheckDraft(views, otherView, otherDraft));
    TRY(umi_document_coordinator_active_snapshot(documents, &active));
    if (strcmp(active.view_id, otherView) != 0) { status = UMI_STATUS_INVALID_STATE; goto cleanup; }
    puts("The source changed from 3 to 5. The second draft stayed active and unchanged.");

    TRY(UmiDocumentCoordinatorUndo(documents, target.document_id));
    TRY(CheckDraft(views, sourceView, source));
    TRY(UmiDocumentCoordinatorRedo(documents, target.document_id));
    TRY(CheckDraft(views, sourceView, updated));
    puts("Undo and Redo used the same document history.");

    /* Draft operations do not write the file. Saving is an explicit step. */
    TRY(umi_fs_read_text(path, &saved, NULL));
    if (strcmp(saved, source) != 0) { status = UMI_STATUS_INVALID_STATE; goto cleanup; }
    umi_fs_free_text(saved);
    saved = NULL;
    TRY(UmiDocumentCoordinatorSaveAs(documents, target.document_id, path));
    TRY(umi_fs_read_text(path, &saved, NULL));
    if (strcmp(saved, updated) != 0) { status = UMI_STATUS_INVALID_STATE; goto cleanup; }
    TRY(CheckDraft(views, otherView, otherDraft));
    puts("Explicit Save wrote the expected source; reading it back confirmed the result.");

cleanup:
    umi_fs_free_text(saved);
    UmiDocumentEditPlanDestroy(plan);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    if (ownsDirectory) {
        UmiStatus removed = umi_fs_remove_tree(root);
        if (status == UMI_STATUS_OK) status = removed;
    }
    if (status != UMI_STATUS_OK) fprintf(stderr, "Notes editing: %s\n", umi_status_text(status));
    return status == UMI_STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
