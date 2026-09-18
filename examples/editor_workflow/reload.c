/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/editor_workflow/reload.c
 *
 * PURPOSE:
 *   Review a changed Notes file, protect an unsaved draft and reload with Undo.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define PROCESS_ID _getpid
#else
#include <unistd.h>
#define PROCESS_ID getpid
#endif

/* Compare a caller-owned copy with the text we expect to see in the editor. */
static UmiStatus CheckText(UmiUiDocumentViewModel *views, const char *viewId,
    const char *expected)
{
    char *text = NULL;
    size_t length = 0U;
    UmiStatus status = UmiUiDocumentViewModelCopyText(views, viewId, &text, &length);
    if (status == UMI_STATUS_OK &&
        (length != strlen(expected) || memcmp(text, expected, length) != 0))
        status = UMI_STATUS_INVALID_STATE;
    UmiUiDocumentViewModelFreeText(text);
    return status;
}

/* The exercise owns a new temporary directory, never an existing project. */
int main(void)
{
    const char *initial = "Umicom Notes: three saved notes\n";
    const char *draft = "Umicom Notes: four notes, with one unsaved\n";
    const char *external = "Umicom Notes: five notes saved by another editor\n";
    char temp[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY] = {0};
    char path[UMI_PATH_CAPACITY], name[100], viewId[UMI_UI_ID_CAPACITY];
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiDocumentReloadPlan *plan = NULL;
    UmiStatus status = UMI_STATUS_OK;
    int ownsDirectory = 0;
    (void)snprintf(name, sizeof name, "umicom-notes-reload-%ld", (long)PROCESS_ID());

    /* Stop on any failed operation; the cleanup block releases partial state. */
#define TRY(operation) do { status = (operation); if (status != UMI_STATUS_OK) goto cleanup; } while (0)
    TRY(umi_fs_temp_directory(temp, sizeof temp));
    TRY(umi_fs_join(root, sizeof root, temp, name));
    if (umi_fs_exists(root)) { status = UMI_STATUS_ALREADY_EXISTS; goto cleanup; }
    TRY(umi_fs_make_directories(root));
    ownsDirectory = 1;
    TRY(umi_fs_join(path, sizeof path, root, "notes.txt"));
    TRY(umi_fs_write_text(path, initial));
    TRY(umi_command_registry_create(&commands));
    TRY(umi_ui_workbench_create("umicom.notes.reload", commands, &workbench));
    TRY(umi_document_store_create(&store));
    TRY(umi_document_coordinator_create(store, workbench, NULL, &documents));
    TRY(umi_document_coordinator_open(documents, path, viewId, sizeof viewId));

    /* Updating the view represents typing. It does not write the saved file. */
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot view;
    TRY(umi_ui_document_view_model_find(views, viewId, &view));
    view.dirty = 1;
    TRY(UmiUiDocumentViewModelUpsertText(views, &view, draft, strlen(draft)));

    /* This write models a second editor. Reload itself never writes to disk. */
    TRY(umi_fs_write_text(path, external));
    UmiDocumentWorkingCopySnapshot snapshot;
    TRY(umi_document_coordinator_active_snapshot(documents, &snapshot));
    TRY(UmiDocumentCoordinatorPrepareReload(documents, snapshot.document_id, &plan));
    UmiDocumentReloadSummary summary;
    TRY(UmiDocumentReloadPlanSummary(plan, &summary));
    printf("Review %s: %zu draft bytes, %zu incoming bytes.\n",
        summary.display_name, summary.previous_bytes, summary.incoming_bytes);

    status = UmiDocumentCoordinatorApplyReload(documents, plan, 0);
    if (status != UMI_STATUS_INVALID_STATE) { status = UMI_STATUS_INVALID_STATE; goto cleanup; }
    TRY(CheckText(views, viewId, draft));
    puts("Without approval, the unsaved draft remains unchanged.");

    /* This deterministic exercise approves its own temporary data. A real UI
     * must obtain the user's decision first, and keep Cancel as the default. */
    TRY(UmiDocumentCoordinatorApplyReload(documents, plan, 1));
    TRY(CheckText(views, viewId, external));
    UmiDocumentReloadPlanDestroy(plan);
    plan = NULL;
    TRY(umi_document_coordinator_undo(documents));
    TRY(CheckText(views, viewId, draft));
    puts("Reload accepted. Undo restored the previous unsaved draft.");

    /* Undo changes editor memory, not the independently saved version. */
    char *saved = NULL;
    TRY(umi_fs_read_text(path, &saved, NULL));
    if (strcmp(saved, external) != 0) status = UMI_STATUS_INVALID_STATE;
    umi_fs_free_text(saved);
    if (status == UMI_STATUS_OK) puts("The saved file was not overwritten by Reload or Undo.");

cleanup:
    UmiDocumentReloadPlanDestroy(plan);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    if (ownsDirectory) (void)umi_fs_remove_tree(root);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Notes reload exercise: %s\n", umi_status_text(status));
        return 1;
    }
    return 0;
#undef TRY
}
