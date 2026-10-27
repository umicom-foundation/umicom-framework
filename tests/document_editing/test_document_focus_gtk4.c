/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_editing/test_document_focus_gtk4.c
 *
 * PURPOSE:
 *   Verify explicit editor focus respects document identity and native callbacks.
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

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "line %d: %s\n", __LINE__, #condition); \
        failed = 1; goto cleanup; \
    } \
} while (0)

typedef struct FocusObservation {
    UmiGtk4Adapter *adapter;
    UmiUiWorkbench *workbench;
    const char *mode;
    const char *otherView;
    GtkWidget *expected;
    unsigned calls;
    UmiStatus status;
} FocusObservation;

/* Deliberately change state during the synchronous native notification. The
 * operation must respect the observer's new selection or retired binding. */
static void FocusChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    FocusObservation *observation = data;
    if (observation->calls != 0U ||
        gtk_window_get_focus(GTK_WINDOW(object)) != observation->expected) return;
    ++observation->calls;
    if (strcmp(observation->mode, "changed-document") == 0)
        observation->status = umi_ui_workbench_activate_document(
            observation->workbench, observation->otherView);
    else if (strcmp(observation->mode, "unbind") == 0)
        observation->status = UmiGtk4AdapterBindDocumentEditing(
            observation->adapter, NULL, NULL, NULL);
    else if (strcmp(observation->mode, "redirect") == 0)
        gtk_window_set_focus(GTK_WINDOW(object), NULL);
}

static GtkWidget *DocumentEditor(GtkWidget *widget, const char *viewId)
{
    if (widget == NULL) return NULL;
    const char *actual = g_object_get_data(G_OBJECT(widget), "umicom-view-id");
    if (actual != NULL && strcmp(actual, viewId) == 0)
        return g_object_get_data(G_OBJECT(widget), "umicom-editor-view");
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = DocumentEditor(child, viewId);
        if (found != NULL) return found;
    }
    return NULL;
}

static int DraftMatches(UmiUiDocumentViewModel *views, const char *viewId,
                        const char *expected, size_t offset)
{
    UmiUiDocumentViewSnapshot view;
    char *text = NULL;
    size_t length = 0U;
    if (umi_ui_document_view_model_find(views, viewId, &view) != UMI_STATUS_OK ||
        UmiUiDocumentViewModelCopyText(views, viewId, &text, &length) != UMI_STATUS_OK)
        return 0;
    int matches = length == strlen(expected) && memcmp(text, expected, length) == 0 &&
        view.cursor_offset == offset && view.selection_length == 0U && view.dirty;
    UmiUiDocumentViewModelFreeText(text);
    return matches;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    if (strcmp(mode, "active") != 0 && strcmp(mode, "changed-document") != 0 &&
        strcmp(mode, "unbind") != 0 && strcmp(mode, "hidden-editor") != 0 &&
        strcmp(mode, "redirect") != 0) return 2;
    if (!gtk_init_check()) return 77;

    int failed = 0;
    GtkApplication *application = NULL;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    GtkWindow *window = NULL;
    gulong handler = 0U;
    char first[UMI_UI_ID_CAPACITY], second[UMI_UI_ID_CAPACITY];
    FocusObservation observation = {0};
    const char *texts[] = {"first draft\n", "second\ndraft\n"};
    const size_t offsets[] = {2U, 8U};

    application = gtk_application_new("org.umicom.document.focus.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("document.focus", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.document.focus.test",
        "Document focus", workbench, &shell) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "first.c", first, sizeof first) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "second.c", second, sizeof second) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    const char *identities[] = {first, second};
    for (size_t index = 0U; index < 2U; ++index) {
        UmiUiDocumentViewSnapshot view;
        CHECK(umi_ui_document_view_model_find(views, identities[index], &view) == UMI_STATUS_OK);
        view.cursor_offset = offsets[index];
        view.selection_length = 0U;
        view.dirty = 1;
        CHECK(UmiUiDocumentViewModelUpsertText(
            views, &view, texts[index], strlen(texts[index])) == UMI_STATUS_OK);
    }
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, NULL, NULL) == UMI_STATUS_OK);
    window = GTK_WINDOW(umi_gtk4_adapter_native_window(adapter));
    CHECK(GTK_IS_WINDOW(window));
    GtkWidget *editor = DocumentEditor(GTK_WIDGET(window), second);
    CHECK(GTK_IS_TEXT_VIEW(editor));
    CHECK(editor != DocumentEditor(GTK_WIDGET(window), first));
    /* Prepared, unpresented windows are part of the embedding contract.
     * No external process or native window presentation is needed here. */
    CHECK(!gtk_widget_get_visible(GTK_WIDGET(window)));
    gtk_window_set_focus(window, NULL);
    observation = (FocusObservation){adapter, workbench, mode, first, editor, 0U, UMI_STATUS_OK};
    handler = g_signal_connect(window, "notify::focus-widget", G_CALLBACK(FocusChanged), &observation);
    if (strcmp(mode, "hidden-editor") == 0) gtk_widget_set_visible(editor, FALSE);

    UmiStatus status = UmiGtk4AdapterFocusActiveDocument(adapter);
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "changed-document") == 0) expected = UMI_STATUS_INVALID_STATE;
    else if (strcmp(mode, "unbind") == 0) expected = UMI_STATUS_CANCELLED;
    else if (strcmp(mode, "hidden-editor") == 0 || strcmp(mode, "redirect") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    CHECK(status == expected);
    CHECK(observation.status == UMI_STATUS_OK);
    CHECK(observation.calls == (strcmp(mode, "hidden-editor") == 0 ? 0U : 1U));
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    CHECK(strcmp(active.view_id, strcmp(mode, "changed-document") == 0 ? first : second) == 0);
    CHECK(DraftMatches(views, first, texts[0], offsets[0]));
    CHECK(DraftMatches(views, second, texts[1], offsets[1]));
    if (strcmp(mode, "active") == 0) {
        CHECK(gtk_window_get_focus(window) == editor);
        GtkTextIter begin, cursor;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor));
        gtk_text_buffer_get_start_iter(buffer, &begin);
        gtk_text_buffer_get_iter_at_mark(buffer, &cursor, gtk_text_buffer_get_insert(buffer));
        char *prefix = gtk_text_buffer_get_text(buffer, &begin, &cursor, TRUE);
        int positioned = prefix != NULL && strlen(prefix) == offsets[1];
        g_free(prefix);
        CHECK(positioned);
    }
    if (strcmp(mode, "redirect") == 0) CHECK(gtk_window_get_focus(window) == NULL);
cleanup:
    if (handler != 0U) g_signal_handler_disconnect(window, handler);
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
