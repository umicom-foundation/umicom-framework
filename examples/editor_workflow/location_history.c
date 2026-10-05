/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/editor_workflow/location_history.c
 * PURPOSE: Follow a source jump and return through the existing document owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/navigation_history.h"
#include <stdio.h>

/* Each owner is released in reverse creation order. The example uses an
 * untitled working copy and never reads or writes a user's file. */
int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiStatus status = umi_command_registry_create(&commands);
    if (status == UMI_STATUS_OK) status = umi_ui_workbench_create("example.navigation", commands, &workbench);
    if (status == UMI_STATUS_OK) status = umi_document_store_create(&store);
    if (status == UMI_STATUS_OK) status = umi_document_coordinator_create(store, workbench, NULL, &documents);
    char view_id[UMI_UI_ID_CAPACITY];
    if (status == UMI_STATUS_OK) status = umi_document_coordinator_new(documents, "notes.c", view_id, sizeof(view_id));
    UmiUiDocumentViewSnapshot view;
    if (status == UMI_STATUS_OK) status = umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench), view_id, &view);
    if (status == UMI_STATUS_OK) {
        view.dirty = 1;
        const char text[] = "first line\nsecond line\n";
        status = UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench), &view, text, sizeof(text) - 1U);
    }
    if (status == UMI_STATUS_OK) status = UmiDocumentCoordinatorGoToPosition(documents, 2U, 1U, NULL);
    UmiDocumentNavigationHistorySnapshot history;
    if (status == UMI_STATUS_OK) status = UmiDocumentCoordinatorNavigationSnapshot(documents, &history);
    if (status == UMI_STATUS_OK) status = UmiDocumentCoordinatorTravel(documents, -1, history.revision, NULL);
    if (status == UMI_STATUS_OK) status = umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench), view_id, &view);
    if (status == UMI_STATUS_OK && (view.cursor_offset != 0U || !view.dirty)) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) puts("Returned to the first line; the unsaved draft is still open.");
    else fprintf(stderr, "Navigation failed: %s\n", umi_status_text(status));
    umi_document_coordinator_destroy(documents); umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench); umi_command_registry_destroy(commands);
    return status == UMI_STATUS_OK ? 0 : 1;
}
