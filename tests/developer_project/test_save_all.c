/*-----------------------------------------------------------------------------
 * Umicom Framework / Studio qualification
 * File: framework/tests/developer_project/test_save_all.c
 * PURPOSE: Save multiple documents with conflict protection and no active-tab change.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); return EXIT_FAILURE; } } while (0)

#include "umicom/document/document.h"
static int Edit(UmiUiWorkbench *workbench, const char *id, const char *text)
{
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench), id, &view) == UMI_STATUS_OK);
    snprintf(view.source_text, sizeof(view.source_text), "%s", text); view.dirty = 1;
    CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(workbench), &view) == UMI_STATUS_OK);
    return EXIT_SUCCESS;
}
int main(void)
{
    UmiCommandRegistry *commands = NULL; UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL; UmiDocumentCoordinator *documents = NULL;
    UmiDocumentWorkingCopySnapshot active;
    char first[UMI_UI_ID_CAPACITY], second[UMI_UI_ID_CAPACITY], draft[UMI_UI_ID_CAPACITY];
    char *text = NULL; size_t saved = 0U;
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.save-all", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "first.txt", first, sizeof(first)) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_save_active_as(documents, "first.txt") == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "second.txt", second, sizeof(second)) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_save_active_as(documents, "second.txt") == UMI_STATUS_OK);
    CHECK(Edit(workbench, first, "Umicom Notes: first\n") == EXIT_SUCCESS);
    CHECK(Edit(workbench, second, "Umicom Notes: second\n") == EXIT_SUCCESS);
    CHECK(umi_document_coordinator_new(documents, "draft.txt", draft, sizeof(draft)) == UMI_STATUS_OK);
    CHECK(Edit(workbench, draft, "Unsaved title\n") == EXIT_SUCCESS);
    CHECK(UmiDocumentCoordinatorSaveAll(documents, &saved) == UMI_STATUS_INVALID_STATE && saved == 0U);
    CHECK(umi_fs_read_text("first.txt", &text, NULL) == UMI_STATUS_OK && text[0] == '\0'); umi_fs_free_text(text);
    CHECK(umi_document_coordinator_save_active_as(documents, "draft.txt") == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_activate_document(workbench, first) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorSaveAll(documents, &saved) == UMI_STATUS_OK && saved == 2U);
    CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    CHECK(strcmp(active.view_id, first) == 0);
    CHECK(umi_fs_read_text("second.txt", &text, NULL) == UMI_STATUS_OK);
    CHECK(strcmp(text, "Umicom Notes: second\n") == 0); umi_fs_free_text(text);
    CHECK(Edit(workbench, first, "local draft\n") == EXIT_SUCCESS);
    CHECK(umi_fs_write_text("first.txt", "outside edit\n") == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorSaveAll(documents, &saved) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_fs_read_text("first.txt", &text, NULL) == UMI_STATUS_OK);
    CHECK(strcmp(text, "outside edit\n") == 0); umi_fs_free_text(text);
    umi_document_coordinator_destroy(documents); umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench); umi_command_registry_destroy(commands);
    CHECK(remove("first.txt") == 0); CHECK(remove("second.txt") == 0); CHECK(remove("draft.txt") == 0);
    puts("Save All passed."); return EXIT_SUCCESS;
}
