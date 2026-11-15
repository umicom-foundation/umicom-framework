/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_navigation_commands_gtk4.c
 *
 * PURPOSE:
 *   Check source navigation command admission, review identity and draft preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/language_runtime/types.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); failed = 1; goto cleanup; \
} } while (0)

static GtkWindow *Review(void)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(window),
                "document.completion.review") != NULL) return window;
        g_object_unref(window);
    }
    return NULL;
}

/* No Request signal is emitted: these cases verify command admission,
 * review identity and source preservation independently of a language server. */
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *command = argv[1], *mode = argv[2];
    const char *commands_to_check[] = {
        "navigate.symbol", "navigate.definition", "navigate.references"};
    const char *modes[] = {"open", "read-only", "empty", "multiline",
        "busy", "unbind", "oversize", "unknown", "no-document"};
    int valid_command = 0, valid_mode = 0;
    for (size_t i = 0U; i < 3U; ++i)
        if (strcmp(command, commands_to_check[i]) == 0) valid_command = 1;
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(mode, modes[i]) == 0) valid_mode = 1;
    if (!valid_command || !valid_mode) return 2;
    if (!gtk_init_check()) return 77;

    int failed = 0;
    GtkApplication *application = NULL;
    GtkWindow *dialog = NULL;
    UmiCommandRegistry *registry = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    UmiUiDocumentViewModel *views = NULL;
    UmiDocumentWorkingCopySnapshot active = {0};
    UmiUiDocumentViewSnapshot view;
    char *base = NULL, *large = NULL, *after = NULL;
    size_t after_bytes = 0U;
    const char *source = strcmp(mode, "empty") == 0 ? "" : "int first;\nint second;\n";
    size_t source_bytes = strlen(source);

    base = g_dir_make_tmp("umicom-navigation-admission-XXXXXX", NULL);
    CHECK(base != NULL);
    application = gtk_application_new("org.umicom.navigation.commands", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&registry) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("navigation.commands", registry, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.navigation.commands", "Navigation",
        workbench, &shell) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, NULL, NULL) == UMI_STATUS_OK);
    CHECK(!UmiGtk4AdapterSourceNavigationCommandKnown(NULL));
    CHECK(!UmiGtk4AdapterSourceNavigationCommandEnabled(NULL, command));
    CHECK(UmiGtk4AdapterSourceNavigationCommandKnown(command));

    if (strcmp(mode, "no-document") == 0) {
        CHECK(!UmiGtk4AdapterSourceNavigationCommandEnabled(adapter, command));
        CHECK(UmiGtk4AdapterSourceNavigationCommand(adapter, command, NULL,
            "NavigationTest", base) != UMI_STATUS_OK);
        CHECK(Review() == NULL);
        goto cleanup;
    }
    CHECK(umi_document_coordinator_new(documents, "draft.c", NULL, 0U) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    views = umi_ui_workbench_documents(workbench);
    CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK);
    if (strcmp(mode, "oversize") == 0) {
        source_bytes = UMI_LANGUAGE_RUNTIME_JSON_CAPACITY;
        large = g_malloc(source_bytes + 1U);
        memset(large, 'x', source_bytes); large[source_bytes] = '\0';
        source = large;
    }
    view.cursor_offset = 0U;
    view.selection_length = strcmp(mode, "multiline") == 0 ? source_bytes : 0U;
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, source, source_bytes) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK);
    if (strcmp(mode, "read-only") == 0) {
        view.read_only = 1;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "unbind") == 0)
        CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);

    guint before_windows = g_list_model_get_n_items(gtk_window_get_toplevels());
    int enabled = UmiGtk4AdapterSourceNavigationCommandEnabled(adapter, command);
    CHECK(g_list_model_get_n_items(gtk_window_get_toplevels()) == before_windows);
    CHECK(!UmiGtk4AdapterCompletionReviewBusy(adapter));
    if (strcmp(mode, "unbind") == 0 || strcmp(mode, "oversize") == 0) {
        CHECK(!enabled);
        CHECK(UmiGtk4AdapterSourceNavigationCommand(adapter, command, NULL, "NavigationTest", base) ==
            (strcmp(mode, "unbind") == 0 ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_CAPACITY_EXCEEDED));
        goto inspect;
    }
    CHECK(enabled);
    if (strcmp(mode, "unknown") == 0) {
        const char *unknown[] = {"", "navigate.symbol.extra", "navigate.Symbol",
            "navigate", "navigate.definition ", "navigate.references.extra"};
        for (size_t i = 0U; i < sizeof unknown / sizeof unknown[0]; ++i) {
            CHECK(!UmiGtk4AdapterSourceNavigationCommandKnown(unknown[i]));
            CHECK(!UmiGtk4AdapterSourceNavigationCommandEnabled(adapter, unknown[i]));
            CHECK(UmiGtk4AdapterSourceNavigationCommand(adapter, unknown[i], NULL,
                "NavigationTest", base) == UMI_STATUS_INVALID_ARGUMENT);
        }
        CHECK(UmiGtk4AdapterSourceNavigationCommand(adapter, command, NULL, NULL, base) ==
            UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Review() == NULL);
        goto inspect;
    }
    CHECK(UmiGtk4AdapterSourceNavigationCommand(adapter, command, NULL,
        "NavigationTest", base) == UMI_STATUS_OK);
    dialog = Review();
    CHECK(dialog != NULL);
    const char *title = strcmp(command, "navigate.symbol") == 0 ? "Document symbols" :
        strcmp(command, "navigate.definition") == 0 ? "Definition locations" : "Reference locations";
    CHECK(strcmp(gtk_window_get_title(dialog), title) == 0);
    CHECK(g_object_get_data(G_OBJECT(dialog), "umicom-completion-pending") == NULL);
    GtkWidget *cancel = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(dialog),
        "document.completion.cancel");
    CHECK(GTK_IS_BUTTON(cancel) && gtk_window_get_default_widget(dialog) == cancel);
    if (strcmp(mode, "busy") == 0) {
        for (size_t i = 0U; i < 3U; ++i) {
            CHECK(!UmiGtk4AdapterSourceNavigationCommandEnabled(adapter, commands_to_check[i]));
            CHECK(UmiGtk4AdapterSourceNavigationCommand(adapter, commands_to_check[i], NULL,
                "NavigationTest", base) == UMI_STATUS_BUSY);
        }
        GtkWindow *same = Review();
        CHECK(same == dialog);
        g_clear_object(&same);
    }
    g_signal_emit_by_name(cancel, "clicked");
    CHECK(!UmiGtk4AdapterCompletionReviewBusy(adapter));
    CHECK(UmiGtk4AdapterSourceNavigationCommandEnabled(adapter, command));
inspect:
    CHECK(UmiUiDocumentViewModelCopyText(views, active.view_id, &after, &after_bytes) == UMI_STATUS_OK);
    CHECK(after_bytes == source_bytes && memcmp(after, source, source_bytes) == 0);
    CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK);
    CHECK(view.dirty && view.cursor_offset == 0U);
    CHECK(view.selection_length == (strcmp(mode, "multiline") == 0 ? source_bytes : 0U));
cleanup:
    UmiUiDocumentViewModelFreeText(after);
    if (adapter != NULL) (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (dialog != NULL) gtk_window_destroy(dialog);
    g_clear_object(&dialog);
    umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(registry);
    g_clear_object(&application);
    g_free(large);
    if (base != NULL) (void)g_rmdir(base);
    g_free(base);
    return failed;
}
