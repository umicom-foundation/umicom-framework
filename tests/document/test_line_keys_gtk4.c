/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_line_keys_gtk4.c
 * PURPOSE: Exercise real source-editor line shortcuts, document identity and Undo.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/document/commands.h"
/* This fixture uses native GTK types directly; the adapter API is opaque. */
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto cleanup; } } while (0)
typedef struct Result { UmiGtk4Adapter *adapter; unsigned calls; UmiStatus status; int destroy; } Result;
static void Completed(void *data, UmiStatus status)
{
    Result *result = data; ++result->calls; result->status = status;
    if (result->destroy) { umi_gtk4_adapter_destroy(result->adapter); result->adapter = NULL; }
}
static GtkWidget *FindEditor(GtkWidget *root)
{
    if (GTK_IS_TEXT_VIEW(root) && gtk_widget_has_css_class(root, "umicom-editor")) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = FindEditor(child); if (found != NULL) return found;
    }
    return NULL;
}
static GtkEventController *KeyController(GtkWidget *view)
{
    GListModel *controllers = gtk_widget_observe_controllers(view);
    GtkEventController *found = NULL;
    for (guint i = 0U; i < g_list_model_get_n_items(controllers); ++i) {
        GtkEventController *item = g_list_model_get_item(controllers, i);
        if (g_strcmp0(gtk_event_controller_get_name(item), "umicom-source-editor-commands") == 0) { found = item; break; }
        g_object_unref(item);
    }
    g_object_unref(controllers); return found;
}
static int ModelText(UmiUiDocumentViewModel *views, const char *view, const char *expected)
{
    char *text = NULL; size_t bytes = 0U;
    UmiStatus status = UmiUiDocumentViewModelCopyText(views, view, &text, &bytes);
    int same = status == UMI_STATUS_OK && bytes == strlen(expected) && strcmp(text, expected) == 0;
    UmiUiDocumentViewModelFreeText(text); return same;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1], *cases[] = {"duplicate", "delete", "move-up", "move-down", "indent", "outdent",
        "comment", "join", "canonical", "extra-meta", "extra-super", "unmatched", "read-only",
        "retired", "closed-document", "other-tab", "completion-close", "caps-lock"};
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (strcmp(mode, cases[i]) == 0) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    UmiCommandRegistry *commands = NULL; UmiUiWorkbench *workbench = NULL; UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL; UmiDocumentCoordinator *documents = NULL;
    GtkApplication *application = NULL; GtkWidget *editor = NULL; GtkEventController *keys = NULL;
    Result result = {0};
    application = gtk_application_new("org.umicom.line-key-test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("line-key-test", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.line-key-test", "Line keys", workbench, &shell) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    char first[UMI_UI_ID_CAPACITY], second[UMI_UI_ID_CAPACITY];
    CHECK(umi_document_coordinator_new(documents, "first.c", first, sizeof first) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, first, &view) == UMI_STATUS_OK);
    const char *source = "a\nbb\nccc", *expected = "a\nbb\nbb\nccc";
    guint key = GDK_KEY_d;
    GdkModifierType modifiers = GDK_CONTROL_MASK | GDK_SHIFT_MASK;
    if (strcmp(mode, "delete") == 0) { key = GDK_KEY_k; expected = "a\nccc"; }
    if (strcmp(mode, "move-up") == 0) { key = GDK_KEY_Up; modifiers = GDK_ALT_MASK; expected = "bb\na\nccc"; }
    if (strcmp(mode, "move-down") == 0) { key = GDK_KEY_Down; modifiers = GDK_ALT_MASK; expected = "a\nccc\nbb"; }
    if (strcmp(mode, "indent") == 0) { key = GDK_KEY_bracketright; modifiers = GDK_CONTROL_MASK; expected = "a\n    bb\nccc"; }
    if (strcmp(mode, "outdent") == 0) { key = GDK_KEY_bracketleft; modifiers = GDK_CONTROL_MASK; source = "a\n    bb\nccc"; expected = "a\nbb\nccc"; }
    if (strcmp(mode, "comment") == 0) { key = GDK_KEY_slash; modifiers = GDK_CONTROL_MASK; expected = "a\n// bb\nccc"; }
    if (strcmp(mode, "join") == 0) { key = GDK_KEY_j; expected = "a\nbb ccc"; }
    if (strcmp(mode, "extra-meta") == 0) modifiers |= GDK_META_MASK;
    if (strcmp(mode, "extra-super") == 0) modifiers |= GDK_SUPER_MASK;
    if (strcmp(mode, "unmatched") == 0) modifiers = GDK_CONTROL_MASK;
    if (strcmp(mode, "caps-lock") == 0) { key = GDK_KEY_D; modifiers |= GDK_LOCK_MASK; }
    view.cursor_offset = 2U; view.selection_length = 0U; view.dirty = 1;
    view.read_only = strcmp(mode, "read-only") == 0;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, source, strlen(source)) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_sync_active(documents) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot target, active;
    CHECK(umi_document_coordinator_active_snapshot(documents, &target) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &result.adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(result.adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(result.adapter, documents, Completed, &result) == UMI_STATUS_OK);
    editor = FindEditor(umi_gtk4_adapter_native_window(result.adapter)); CHECK(editor != NULL); g_object_ref(editor);
    keys = KeyController(editor); CHECK(keys != NULL);
    const int ignored = strcmp(mode, "extra-meta") == 0 || strcmp(mode, "extra-super") == 0 ||
        strcmp(mode, "unmatched") == 0 || strcmp(mode, "retired") == 0 || strcmp(mode, "closed-document") == 0;
    if (strcmp(mode, "retired") == 0) { umi_gtk4_adapter_destroy(result.adapter); result.adapter = NULL; }
    if (strcmp(mode, "closed-document") == 0) CHECK(umi_document_coordinator_close_active(documents, 1) == UMI_STATUS_OK);
    if (strcmp(mode, "other-tab") == 0) {
        CHECK(umi_document_coordinator_new(documents, "other.c", second, sizeof second) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    }
    result.destroy = strcmp(mode, "completion-close") == 0;
    if (strcmp(mode, "canonical") == 0) {
        CHECK(UmiGtk4AdapterDocumentCommandEnabled(result.adapter, UMI_DOCUMENT_COMMAND_DUPLICATE_LINE));
        CHECK(UmiGtk4AdapterDocumentCommand(result.adapter, UMI_DOCUMENT_COMMAND_DUPLICATE_LINE) == UMI_STATUS_OK);
    } else {
        if (strcmp(mode, "read-only") == 0) CHECK(!UmiGtk4AdapterDocumentCommandEnabled(result.adapter, UMI_DOCUMENT_COMMAND_DUPLICATE_LINE));
        gboolean handled = FALSE;
        g_signal_emit_by_name(keys, "key-pressed", key, 0U, modifiers, &handled);
        CHECK(handled == !ignored);
    }
    CHECK(result.calls == (ignored ? 0U : 1U));
    if (!ignored) CHECK(result.status == (strcmp(mode, "read-only") == 0 ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_OK));
    if (strcmp(mode, "closed-document") == 0) { CHECK(umi_document_coordinator_count(documents) == 0U); goto cleanup; }
    const int changed = !ignored && strcmp(mode, "read-only") != 0;
    CHECK(ModelText(views, first, changed ? expected : source));
    if (result.adapter != NULL && strcmp(mode, "other-tab") != 0) {
        GtkWidget *visible = FindEditor(umi_gtk4_adapter_native_window(result.adapter));
        CHECK(visible != NULL);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(visible));
        GtkTextIter begin, end; gtk_text_buffer_get_bounds(buffer, &begin, &end);
        char *native = gtk_text_buffer_get_text(buffer, &begin, &end, TRUE);
        int same = strcmp(native, changed ? expected : source) == 0; g_free(native);
        CHECK(same);
    }
    if (strcmp(mode, "other-tab") == 0) {
        UmiDocumentWorkingCopySnapshot current;
        CHECK(ModelText(views, second, ""));
        CHECK(umi_document_coordinator_active_snapshot(documents, &current) == UMI_STATUS_OK && current.document_id == active.document_id);
    }
    if (changed) {
        CHECK(UmiDocumentCoordinatorUndo(documents, target.document_id) == UMI_STATUS_OK && ModelText(views, first, source));
        CHECK(UmiDocumentCoordinatorRedo(documents, target.document_id) == UMI_STATUS_OK && ModelText(views, first, expected));
    }
cleanup:
    if (result.adapter != NULL) umi_gtk4_adapter_destroy(result.adapter);
    g_clear_object(&keys); g_clear_object(&editor);
    umi_document_coordinator_destroy(documents); umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell); umi_ui_workbench_destroy(workbench); umi_command_registry_destroy(commands);
    g_clear_object(&application); return failed;
}
