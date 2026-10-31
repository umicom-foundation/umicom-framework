/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_editing/test_group_focus_gtk4.c
 *
 * PURPOSE:
 *   Verify split source focus selects the document used by menu commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/document_commands.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)

static GtkWidget *Editor(GtkWidget *widget, const char *id)
{
    if (widget == NULL) return NULL;
    const char *actual = g_object_get_data(G_OBJECT(widget), "umicom-view-id");
    if (actual != NULL && strcmp(actual, id) == 0)
        return g_object_get_data(G_OBJECT(widget), "umicom-editor-view");
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
        child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Editor(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}

static void Pump(void)
{
    for (unsigned i = 0U; i < 128U && g_main_context_pending(NULL); ++i)
        g_main_context_iteration(NULL, FALSE);
}

static int Active(UmiDocumentCoordinator *documents, const char *id)
{
    UmiDocumentWorkingCopySnapshot active;
    return umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK &&
        strcmp(active.view_id, id) == 0;
}

int main(int argc, char **argv)
{
    const char *modes[] = {"primary", "secondary", "switch-back", "menu-target",
        "read-only", "refresh-preserves", "non-source", "repeat", "rows", "closed-page"};
    if (argc != 2) return 2;
    size_t mode = 0U;
    while (mode < G_N_ELEMENTS(modes) && strcmp(argv[1], modes[mode]) != 0) ++mode;
    if (mode == G_N_ELEMENTS(modes)) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    GtkApplication *application = NULL;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    GtkWindow *window = NULL, *retainedWindow = NULL;
    GtkWidget *retainedPage = NULL;
    char first[UMI_UI_ID_CAPACITY], second[UMI_UI_ID_CAPACITY];
    application = gtk_application_new("org.umicom.editor.group.focus", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("editor.group.focus", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.editor.group.focus",
        "Source focus", workbench, &shell) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "first.c", first, sizeof first) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "second.c", second, sizeof second) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot a, b;
    CHECK(umi_ui_document_view_model_find(views, first, &a) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, second, &b) == UMI_STATUS_OK);
    CHECK(UmiUiDocumentViewModelUpsertText(views, &a, "first source", 12U) == UMI_STATUS_OK);
    CHECK(UmiUiDocumentViewModelUpsertText(views, &b, "second source", 13U) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_place(views, second, UMI_UI_SECONDARY_EDITOR_GROUP_ID, 0U) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, first, &a) == UMI_STATUS_OK);
    a.active = 1;
    a.read_only = mode == 4U;
    CHECK(umi_ui_document_view_model_upsert(views, &a) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, second, &b) == UMI_STATUS_OK);
    b.active = 1;
    CHECK(umi_ui_document_view_model_upsert(views, &b) == UMI_STATUS_OK);
    UmiUiWorkbenchState state;
    CHECK(umi_ui_workbench_state_snapshot(workbench, &state) == UMI_STATUS_OK);
    state.editor_split_mode = mode == 8U ? UMI_UI_EDITOR_SPLIT_ROWS : UMI_UI_EDITOR_SPLIT_COLUMNS;
    CHECK(umi_ui_workbench_state_apply(workbench, &state) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, NULL, NULL) == UMI_STATUS_OK);
    window = GTK_WINDOW(umi_gtk4_adapter_native_window(adapter));
    GtkWidget *left = Editor(GTK_WIDGET(window), first);
    GtkWidget *right = Editor(GTK_WIDGET(window), second);
    CHECK(GTK_IS_TEXT_VIEW(left) && GTK_IS_TEXT_VIEW(right));
    GtkTextBuffer *leftBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(left));
    GtkTextBuffer *rightBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(right));
    gtk_window_present(window);
    Pump();
    gtk_window_set_focus(window, NULL);
    CHECK(umi_ui_workbench_activate_document(workbench, second) == UMI_STATUS_OK);
    /* Move actual native focus without changing notebook pages. Mouse and
     * keyboard entry into the other visible group must also change menu ownership. */
    gtk_window_set_focus(window, left);
    CHECK(gtk_window_get_focus(window) == left);
    CHECK(Active(documents, first));
    if (mode == 1U || mode == 2U || mode == 7U) {
        unsigned count = mode == 7U ? 8U : 1U;
        for (unsigned i = 0U; i < count; ++i) {
            gtk_window_set_focus(window, right);
            CHECK(Active(documents, second));
            if (mode != 1U) {
                gtk_window_set_focus(window, left);
                CHECK(Active(documents, first));
            }
        }
    } else if (mode == 3U || mode == 4U) {
        if (mode == 4U) CHECK(!gtk_text_view_get_editable(GTK_TEXT_VIEW(left)));
        CHECK(UmiGtk4AdapterDocumentCommand(adapter, "edit.select-all") == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(views, first, &a) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(views, second, &b) == UMI_STATUS_OK);
        CHECK(a.cursor_offset == 0U && a.selection_length == 12U);
        CHECK(b.selection_length == 0U);
    } else if (mode == 5U) {
        CHECK(umi_gtk4_adapter_refresh(adapter) == UMI_STATUS_OK);
        CHECK(Editor(GTK_WIDGET(window), first) == left && Editor(GTK_WIDGET(window), second) == right);
        CHECK(gtk_text_view_get_buffer(GTK_TEXT_VIEW(left)) == leftBuffer);
        CHECK(gtk_text_view_get_buffer(GTK_TEXT_VIEW(right)) == rightBuffer);
        CHECK(gtk_window_get_focus(window) == left && Active(documents, first));
    } else if (mode == 6U) {
        gtk_window_set_focus(window, NULL);
        CHECK(Active(documents, first));
    } else if (mode == 9U) {
        /* Retain a real closed page through pruning; its controller must no
         * longer select that identity when another native host focuses it. */
        retainedPage = gtk_widget_get_ancestor(left, GTK_TYPE_SCROLLED_WINDOW);
        CHECK(retainedPage != NULL);
        g_object_ref(retainedPage);
        CHECK(umi_ui_document_view_model_remove(views, first) == UMI_STATUS_OK);
        CHECK(umi_ui_workbench_activate_document(workbench, second) == UMI_STATUS_OK);
        CHECK(umi_gtk4_adapter_refresh(adapter) == UMI_STATUS_OK);
        CHECK(gtk_widget_get_parent(retainedPage) == NULL);
        retainedWindow = GTK_WINDOW(gtk_window_new());
        gtk_window_set_child(retainedWindow, retainedPage);
        gtk_window_present(retainedWindow);
        Pump();
        gtk_window_set_focus(retainedWindow, left);
        CHECK(Active(documents, second));
    }
cleanup:
    if (retainedWindow != NULL) gtk_window_destroy(retainedWindow);
    if (retainedPage != NULL) g_object_unref(retainedPage);
    if (adapter != NULL) (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    if (application != NULL) g_object_unref(application);
    return failed;
}
