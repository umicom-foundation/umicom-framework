/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_coordinator.c
 * PURPOSE: Verify headless New/Edit/Undo/Redo/Find/Replace/Save/Close behaviour.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *coordinator = NULL;
    UmiDocumentWorkingCopySnapshot snapshot;
    UmiUiDocumentViewSnapshot view;
    char root[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    size_t offset;
    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("test.document.workbench", commands, &workbench) == UMI_STATUS_OK);
    assert(umi_document_store_create(&store) == UMI_STATUS_OK);
    assert(umi_document_coordinator_create(store, workbench, NULL, &coordinator) == UMI_STATUS_OK);
    assert(umi_document_coordinator_new(coordinator, "Example.c", NULL, 0U) == UMI_STATUS_OK);
    assert(umi_document_coordinator_active_snapshot(coordinator, &snapshot) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),
                                           snapshot.view_id, &view) == UMI_STATUS_OK);
    (void)snprintf(view.source_text, sizeof(view.source_text), "%s", "one\ntwo\nthree\n");
    view.dirty = 1;
    assert(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(workbench), &view) == UMI_STATUS_OK);
    assert(umi_document_coordinator_find(coordinator, "two", &offset) == UMI_STATUS_OK);
    assert(offset == 4U);
    assert(umi_document_coordinator_replace(coordinator, "two", "SECOND", &offset) == UMI_STATUS_OK);
    assert(umi_document_coordinator_undo(coordinator) == UMI_STATUS_OK);
    assert(umi_document_coordinator_redo(coordinator) == UMI_STATUS_OK);
    assert(umi_document_coordinator_go_to_line(coordinator, 3U, &offset) == UMI_STATUS_OK);
    assert(umi_fs_temp_directory(root, sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_join(path, sizeof(path), root, "umicom-b39-coordinator.c") == UMI_STATUS_OK);
    assert(umi_document_coordinator_save_active_as(coordinator, path) == UMI_STATUS_OK);
    assert(umi_document_coordinator_close_active(coordinator, 0) == UMI_STATUS_OK);
    assert(umi_document_coordinator_count(coordinator) == 0U);
    assert(remove(path) == 0);
    umi_document_coordinator_destroy(coordinator);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    (void)printf("document coordinator passed\n");
    return 0;
}
