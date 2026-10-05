/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_closing/test_navigation_history_gtk4.c
 * PURPOSE: Check navigation binding decisions and completion lifetime with real GTK services.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/document_commands.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto cleanup; } } while (0)
typedef struct Result { unsigned count; UmiStatus status; UmiGtk4Adapter *detach; } Result;
static void Complete(void *context, UmiStatus status)
{
    Result *result = context;
    ++result->count; result->status = status;
    if (result->detach != NULL) (void)UmiGtk4AdapterBindDocumentEditing(result->detach, NULL, NULL, NULL);
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    if (strcmp(name, "link") == 0) return
        UmiGtk4AdapterTravelDocument(NULL, -1) == UMI_STATUS_INVALID_ARGUMENT &&
        UmiGtk4AdapterClearNavigationHistory(NULL) == UMI_STATUS_INVALID_ARGUMENT &&
        !UmiGtk4AdapterNavigationHistoryEnabled(NULL, 0) ? 0 : 1;
    const char *cases[] = {"roundtrip", "clear", "save-busy", "close-busy", "complete-unbind", "unbound", "missing"};
    int known = 0;
    for (size_t index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index)
        if (strcmp(cases[index], name) == 0) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    GtkApplication *application = NULL;
    UmiDocumentWorkingCopySnapshot active;
    UmiUiDocumentViewSnapshot view;
    UmiDocumentNavigationHistorySnapshot history;
    Result result = {0};
    application = gtk_application_new("org.umicom.navigation.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.navigation.native", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.navigation.test", "Umicom Notes", workbench, &shell) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "notes.c", NULL, 0U) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench), active.view_id, &view) == UMI_STATUS_OK);
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench), &view, "first\nsecond\nthird\n", 19U) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorGoToPosition(documents, 2U, 1U, NULL) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Complete, &result) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterNavigationHistoryEnabled(adapter, -1));
    CHECK(!UmiGtk4AdapterNavigationHistoryEnabled(adapter, 1));
    if (strcmp(name, "save-busy") == 0 || strcmp(name, "close-busy") == 0) {
        if (strcmp(name, "save-busy") == 0) CHECK(UmiGtk4AdapterDocumentSaveAll(adapter, NULL, NULL) == UMI_STATUS_OK);
        else CHECK(UmiGtk4AdapterRequestDocumentClose(adapter, NULL) == UMI_STATUS_OK);
        CHECK(!UmiGtk4AdapterNavigationHistoryEnabled(adapter, -1));
        CHECK(UmiGtk4AdapterTravelDocument(adapter, -1) == UMI_STATUS_BUSY);
        CHECK(UmiGtk4AdapterClearNavigationHistory(adapter) == UMI_STATUS_BUSY);
        CHECK(result.count == 2U && result.status == UMI_STATUS_BUSY);
        goto cleanup;
    }
    if (strcmp(name, "unbound") == 0) {
        CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        CHECK(UmiGtk4AdapterTravelDocument(adapter, -1) == UMI_STATUS_UNAVAILABLE);
        CHECK(!UmiGtk4AdapterNavigationHistoryEnabled(adapter, -1) && result.count == 0U);
        goto cleanup;
    }
    if (strcmp(name, "clear") == 0) {
        CHECK(UmiGtk4AdapterClearNavigationHistory(adapter) == UMI_STATUS_OK);
        CHECK(result.count == 1U && !UmiGtk4AdapterNavigationHistoryEnabled(adapter, -1));
        CHECK(UmiDocumentCoordinatorNavigationSnapshot(documents, &history) == UMI_STATUS_OK && history.count == 0U);
        goto cleanup;
    }
    if (strcmp(name, "missing") == 0) {
        CHECK(UmiDocumentCoordinatorClose(documents, active.document_id, 1) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorNavigationSnapshot(documents, &history) == UMI_STATUS_OK);
        uint64_t revision = history.revision;
        CHECK(UmiGtk4AdapterTravelDocument(adapter, -1) == UMI_STATUS_NOT_FOUND);
        CHECK(result.count == 1U && result.status == UMI_STATUS_NOT_FOUND);
        CHECK(UmiDocumentCoordinatorNavigationSnapshot(documents, &history) == UMI_STATUS_OK && history.revision == revision);
        goto cleanup;
    }
    if (strcmp(name, "complete-unbind") == 0) result.detach = adapter;
    CHECK(UmiGtk4AdapterTravelDocument(adapter, -1) == UMI_STATUS_OK);
    CHECK(result.count == 1U && result.status == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench), active.view_id, &view) == UMI_STATUS_OK);
    CHECK(view.cursor_offset == 0U && view.dirty);
    if (result.detach == NULL) {
        CHECK(UmiGtk4AdapterTravelDocument(adapter, 1) == UMI_STATUS_OK);
        CHECK(result.count == 2U && UmiGtk4AdapterNavigationHistoryEnabled(adapter, -1));
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench), active.view_id, &view) == UMI_STATUS_OK);
        CHECK(view.cursor_offset == 6U && strcmp(view.source_text, "first\nsecond\nthird\n") == 0);
    } else CHECK(!UmiGtk4AdapterNavigationHistoryEnabled(adapter, 1));
cleanup:
    if (adapter != NULL) (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    umi_gtk4_adapter_destroy(adapter); umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store); umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench); umi_command_registry_destroy(commands);
    g_clear_object(&application);
    return failed;
}
