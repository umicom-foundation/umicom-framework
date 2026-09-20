/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/editor_gtk4.c
 *
 * PURPOSE:
 *   Render Framework document views as usable GTK4 text editors, optionally
 *   using GtkSourceView 5 when the dependency is available.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "gtk4_internal.h"

#include <string.h>

#include "umicom/editor/presentation.h"

#if defined(UMICOM_GTK4_HAS_SOURCEVIEW5)
#include <gtksourceview/gtksource.h>
#endif

typedef struct UmiGtk4EditorBinding {
    UmiGtk4Adapter *adapter;
    char view_id[UMI_UI_ID_CAPACITY];
} UmiGtk4EditorBinding;

/* One bounded outer user action can delete selected text before pasting.
 * Keep its accepted starting state until the whole action has completed. */
typedef struct UmiGtk4EditorAction {
    UmiUiDocumentViewSnapshot document;
    char *text;
    int insert_offset;
    int bound_offset;
    bool rejected;
} UmiGtk4EditorAction;

/* Removed documents invalidate their old closures before any identity reuse. */
static void invalidate_editor_identity(UmiGtk4Adapter *adapter, const char *view_id);

/*
 * Provide the effective editor group operation used by this module and its client
 * applications.
 */
static const char *effective_editor_group(
    const UmiUiDocumentViewSnapshot *document)
{
    return document->group_id[0] != '\0'
        ? document->group_id
        : UMI_UI_PRIMARY_EDITOR_GROUP_ID;
}

/*
 * Provide the notebook for group operation used by this module and its client
 * applications.
 */
static GtkWidget *notebook_for_group(UmiGtk4Adapter *adapter,
                                     const char *group_id)
{
    return strcmp(group_id, UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0
        ? adapter->secondary_document_notebook
        : adapter->document_notebook;
}

/* Remove only closed or replaced documents; ordinary refreshes keep the real
 * text views, undo history, selection and scroll adjustments in place. */
static void prune_notebook(UmiGtk4Adapter *adapter, GtkWidget *notebook,
                           UmiUiDocumentViewModel *documents)
{
    int page_index = 0;
    while (page_index < gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook))) {
        GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page_index);
        const char *id = g_object_get_data(G_OBJECT(page), "umicom-view-id");
        GtkWidget *view = g_object_get_data(G_OBJECT(page), "umicom-editor-view");
        UmiUiDocumentViewSnapshot current;
        const UmiUiDocumentViewSnapshot *previous = view != NULL
            ? g_object_get_data(G_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view))), "umicom-editor-snapshot") : NULL;
        if (id == NULL || view == NULL ||
            umi_ui_document_view_model_find(documents, id, &current) != UMI_STATUS_OK ||
            (previous != NULL && strcmp(previous->document_id, current.document_id) != 0)) {
            if (id != NULL) invalidate_editor_identity(adapter, id);
            gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), page_index);
        } else {
            ++page_index;
        }
    }
}

/* View identity stays stable when a tab moves between the two editor groups. */
static GtkWidget *find_document_page(UmiGtk4Adapter *adapter, const char *view_id,
                                    GtkWidget **out_notebook)
{
    GtkWidget *notebooks[2] = {adapter->document_notebook, adapter->secondary_document_notebook};
    for (size_t group = 0U; group < 2U; ++group) {
        int count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebooks[group]));
        for (int index = 0; index < count; ++index) {
            GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebooks[group]), index);
            const char *id = g_object_get_data(G_OBJECT(page), "umicom-view-id");
            if (id != NULL && strcmp(id, view_id) == 0) {
                *out_notebook = notebooks[group];
                return page;
            }
        }
    }
    *out_notebook = NULL;
    return NULL;
}

/*
 * Provide the on document page switched operation used by this module and its client
 * applications.
 */
static void on_document_page_switched(GtkNotebook *notebook,
                                      GtkWidget *page,
                                      guint page_number,
                                      gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *view_id;
    UmiUiWorkbench *workbench;
    (void)notebook;
    (void)page_number;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->shell == NULL || page == NULL ||
        adapter->applying_document_state) return;
    view_id = (const char *)g_object_get_data(G_OBJECT(page), "umicom-view-id");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view_id == NULL) return;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    (void)umi_ui_workbench_activate_document(workbench, view_id);
}

/*
 * Provide the group for notebook operation used by this module and its client
 * applications.
 */
static const char *group_for_notebook(UmiGtk4Adapter *adapter,
                                      GtkNotebook *notebook)
{
    return GTK_WIDGET(notebook) == adapter->secondary_document_notebook
        ? UMI_UI_SECONDARY_EDITOR_GROUP_ID
        : UMI_UI_PRIMARY_EDITOR_GROUP_ID;
}

/*
 * Provide the synchronise document page operation used by this module and its client
 * applications.
 */
static void synchronise_document_page(GtkNotebook *notebook,
                                      GtkWidget *page,
                                      guint page_number,
                                      gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    UmiUiWorkbench *workbench;
    const char *view_id;
    const char *group_id;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->shell == NULL || page == NULL ||
        adapter->applying_document_state) {
        return;
    }
    view_id = (const char *)g_object_get_data(G_OBJECT(page),
                                              "umicom-view-id");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view_id == NULL) return;
    group_id = group_for_notebook(adapter, notebook);
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    status = umi_ui_document_view_model_place(
        umi_ui_workbench_documents(workbench), view_id, group_id,
        (size_t)page_number);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        (void)umi_ui_workbench_activate_document(workbench, view_id);
        gtk_label_set_text(GTK_LABEL(adapter->status_label),
                           "Editor layout updated");
    }
}

/*
 * Provide the on document page added operation used by this module and its client
 * applications.
 */
static void on_document_page_added(GtkNotebook *notebook,
                                   GtkWidget *page,
                                   guint page_number,
                                   gpointer user_data)
{
    synchronise_document_page(notebook, page, page_number, user_data);
}

/*
 * Provide the on document page reordered operation used by this module and its client
 * applications.
 */
static void on_document_page_reordered(GtkNotebook *notebook,
                                       GtkWidget *page,
                                       guint page_number,
                                       gpointer user_data)
{
    synchronise_document_page(notebook, page, page_number, user_data);
}

/*
 * Provide the editor binding free operation used by this module and its client
 * applications.
 */
static void editor_binding_free(gpointer data, GClosure *closure)
{
    UmiGtk4EditorBinding *binding = data;
    /* GClosureNotify supplies the owning closure for advanced finalisers.
     * This binding owns no closure resources, but retaining the exact GTK
     * callback signature keeps strict C23 builds type-safe on Windows. */
    (void)closure;
    if (binding != NULL && binding->adapter != NULL && binding->adapter->editor_bindings != NULL)
        (void)g_ptr_array_remove_fast(binding->adapter->editor_bindings, binding);
    g_free(data);
}

/* A retained buffer from a closed document may still emit signals. Removing
 * its borrowed owner prevents it from editing a later document with that ID. */
static void invalidate_editor_identity(UmiGtk4Adapter *adapter, const char *view_id)
{
    guint index = 0U;
    if (adapter == NULL || adapter->editor_bindings == NULL) return;
    while (index < adapter->editor_bindings->len) {
        UmiGtk4EditorBinding *binding = g_ptr_array_index(adapter->editor_bindings, index);
        if (strcmp(binding->view_id, view_id) == 0) {
            binding->adapter = NULL;
            g_ptr_array_remove_index_fast(adapter->editor_bindings, index);
        } else ++index;
    }
}

/* Closure storage belongs to GTK; only its adapter reference is invalidated.
 * Disconnect notebook signals too, because a parent can retain the root. */
void umi_gtk4_release_document_bindings(UmiGtk4Adapter *adapter)
{
    GtkWidget *notebooks[2];
    gulong *handlers[6];
    if (adapter == NULL) return;
    notebooks[0] = adapter->editor_notebooks_initialised
        ? g_weak_ref_get(&adapter->editor_notebooks[0]) : NULL;
    notebooks[1] = adapter->editor_notebooks_initialised
        ? g_weak_ref_get(&adapter->editor_notebooks[1]) : NULL;
    handlers[0] = &adapter->document_page_switch_handler;
    handlers[1] = &adapter->document_page_added_handler;
    handlers[2] = &adapter->document_page_reordered_handler;
    handlers[3] = &adapter->secondary_document_page_switch_handler;
    handlers[4] = &adapter->secondary_document_page_added_handler;
    handlers[5] = &adapter->secondary_document_page_reordered_handler;
    for (size_t index = 0U; index < 6U; ++index) {
        GtkWidget *notebook = notebooks[index / 3U];
        if (notebook != NULL && *handlers[index] != 0UL &&
            g_signal_handler_is_connected(notebook, *handlers[index]))
            g_signal_handler_disconnect(notebook, *handlers[index]);
        *handlers[index] = 0UL;
    }
    if (adapter->editor_notebooks_initialised) {
        for (size_t index = 0U; index < 2U; ++index) {
            g_clear_object(&notebooks[index]);
            g_weak_ref_clear(&adapter->editor_notebooks[index]);
        }
        adapter->editor_notebooks_initialised = 0;
    }
    if (adapter->editor_bindings != NULL) {
        for (guint index = 0U; index < adapter->editor_bindings->len; ++index) {
            UmiGtk4EditorBinding *binding = g_ptr_array_index(adapter->editor_bindings, index);
            binding->adapter = NULL;
        }
        g_ptr_array_free(adapter->editor_bindings, TRUE);
        adapter->editor_bindings = NULL;
    }
}

/* Release the temporary draft owned by a single native user action. */
static void editor_action_free(gpointer data)
{
    UmiGtk4EditorAction *action = data;
    if (action != NULL) { g_free(action->text); g_free(action); }
}

/* A text revision avoids copying large drafts during unrelated status ticks.
 * The snapshot key keeps its original public type for existing integrations. */
static uint64_t EditorContentRevision(GtkTextBuffer *buffer)
{
    const uint64_t *revision = g_object_get_data(G_OBJECT(buffer), "umicom-editor-text-revision");
    return revision != NULL ? *revision : 0U;
}

/* Keep native typing in sync without resetting the GtkTextBuffer undo stack. */
static void EditorRememberRevision(GtkTextBuffer *buffer, UmiUiDocumentViewModel *model,
    const char *viewId)
{
    UmiUiDocumentTextInfo info;
    if (UmiUiDocumentViewModelTextInfo(model, viewId, &info) != UMI_STATUS_OK) return;
    uint64_t *revision = g_object_get_data(G_OBJECT(buffer), "umicom-editor-text-revision");
    if (revision == NULL) {
        revision = g_try_new(uint64_t, 1);
        if (revision == NULL) return; /* A later refresh compares the full text. */
        g_object_set_data_full(G_OBJECT(buffer), "umicom-editor-text-revision", revision, g_free);
    }
    *revision = info.text_revision;
}

/* Report the shared document limit rather than the legacy preview size. */
static void report_editor_capacity(UmiGtk4Adapter *adapter)
{
    char message[160];
    (void)g_snprintf(message, sizeof(message),
        "Insertion not applied: this editor supports up to %zu UTF-8 bytes per document",
        (size_t)UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES);
    gtk_label_set_text(GTK_LABEL(adapter->status_label), message);
}

/* Capture the pre-paste selection and draft before GTK can delete anything.
 * GTK emits this signal only for the outermost grouped user action. */
static void on_editor_begin_user_action(GtkTextBuffer *buffer, gpointer user_data)
{
    UmiGtk4EditorBinding *binding = user_data;
    UmiGtk4EditorAction *action;
    UmiUiWorkbench *workbench;
    GtkTextIter start, end;
    if (binding == NULL || binding->adapter == NULL || binding->adapter->shell == NULL ||
        binding->adapter->applying_document_state) return;
    workbench = umi_ui_application_shell_workbench(binding->adapter->shell);
    action = g_new0(UmiGtk4EditorAction, 1);
    if (umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),
        binding->view_id, &action->document) != UMI_STATUS_OK) {
        editor_action_free(action);
        return;
    }
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    action->text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    gtk_text_buffer_get_iter_at_mark(buffer, &start, gtk_text_buffer_get_insert(buffer));
    gtk_text_buffer_get_iter_at_mark(buffer, &end, gtk_text_buffer_get_selection_bound(buffer));
    action->insert_offset = gtk_text_iter_get_offset(&start);
    action->bound_offset = gtk_text_iter_get_offset(&end);
    g_object_set_data_full(G_OBJECT(buffer), "umicom-editor-action", action, editor_action_free);
}

/* Roll back a rejected grouped paste, including any preceding deletion.
 * This restores accepted content and native selection; the model revision
 * still advances because provisional edits and their rollback were observed. */
static void on_editor_end_user_action(GtkTextBuffer *buffer, gpointer user_data)
{
    UmiGtk4EditorBinding *binding = user_data;
    UmiGtk4EditorAction *action = g_object_steal_data(G_OBJECT(buffer), "umicom-editor-action");
    UmiUiDocumentViewSnapshot current;
    UmiUiWorkbench *workbench;
    if (action == NULL) return;
    if (!action->rejected && binding != NULL && binding->adapter != NULL &&
        UmiGtk4EditorHasDocument(binding->adapter, binding->view_id) && !binding->adapter->applying_document_state) {
        UmiStatus syncStatus = UmiGtk4EditorSynchronise(binding->adapter, binding->view_id);
        if (syncStatus != UMI_STATUS_OK && binding->adapter->status_label != NULL)
            gtk_label_set_text(GTK_LABEL(binding->adapter->status_label),
                "The draft is open, but its history group could not be recorded.");
    }
    if (!action->rejected || binding == NULL || binding->adapter == NULL ||
        binding->adapter->shell == NULL || action->text == NULL) {
        editor_action_free(action);
        return;
    }
    workbench = umi_ui_application_shell_workbench(binding->adapter->shell);
    if (umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),
        binding->view_id, &current) == UMI_STATUS_OK &&
        strcmp(current.document_id, action->document.document_id) == 0 &&
        strcmp(current.uri, action->document.uri) == 0) {
        GtkTextIter insert, bound;
        UmiUiDocumentViewSnapshot *cached;
        const int was_applying = binding->adapter->applying_document_state;
        binding->adapter->applying_document_state = 1;
        gtk_text_buffer_set_text(buffer, action->text, -1);
        gtk_text_buffer_get_iter_at_offset(buffer, &insert, action->insert_offset);
        gtk_text_buffer_get_iter_at_offset(buffer, &bound, action->bound_offset);
        gtk_text_buffer_select_range(buffer, &insert, &bound);
        current.dirty = action->document.dirty;
        current.preview = action->document.preview;
        current.cursor_offset = action->document.cursor_offset;
        current.selection_length = action->document.selection_length;
        (void)UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench),
            &current, action->text, strlen(action->text));
        (void)umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),
            current.view_id, &current);
        EditorRememberRevision(buffer, umi_ui_workbench_documents(workbench), current.view_id);
        cached = g_object_get_data(G_OBJECT(buffer), "umicom-editor-snapshot");
        if (cached != NULL) {
            (void)g_strlcpy(cached->source_text, current.source_text, sizeof(cached->source_text));
            cached->cursor_offset = current.cursor_offset;
            cached->selection_length = current.selection_length;
        }
        binding->adapter->applying_document_state = was_applying;
        report_editor_capacity(binding->adapter);
    }
    editor_action_free(action);
}

/* Reject an oversized insertion before GTK changes its working copy. The
 * bound applies to the complete draft, not to the small ABI preview. Grouped
 * paste rollback retains any text deleted before this insertion was checked. */
static void on_editor_insert_text(GtkTextBuffer *buffer, GtkTextIter *location,
                                  char *text, int length, gpointer user_data)
{
    UmiGtk4EditorBinding *binding = user_data;
    GtkTextIter start, end;
    char *current;
    size_t existing_length, inserted_length;
    const size_t maximum = UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES;
    (void)location;
    if (binding == NULL || binding->adapter == NULL ||
        binding->adapter->applying_document_state) return;
    if (text == NULL) return;
    inserted_length = length < 0 ? strlen(text) : (size_t)length;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    current = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    existing_length = current != NULL ? strlen(current) : 0U;
    g_free(current);
    if (existing_length > maximum || inserted_length > maximum - existing_length ||
        !g_utf8_validate(text, (gssize)inserted_length, NULL)) {
        UmiGtk4EditorAction *action = g_object_get_data(G_OBJECT(buffer), "umicom-editor-action");
        if (action != NULL) action->rejected = true;
        g_signal_stop_emission_by_name(buffer, "insert-text");
        report_editor_capacity(binding->adapter);
    }
}

/* Publish accepted native edits without truncating the complete document.
 * The last applied source is remembered so status refreshes preserve the
 * existing buffer, selection, scroll position and undo history. */
static void on_editor_buffer_changed(GtkTextBuffer *text_buffer,
                                     gpointer user_data)
{
    UmiGtk4EditorBinding *binding = (UmiGtk4EditorBinding *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiDocumentViewSnapshot document;
    GtkTextIter start;
    GtkTextIter end;
    char *text;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || binding->adapter == NULL ||
        binding->adapter->shell == NULL || binding->adapter->applying_document_state) return;
    workbench = umi_ui_application_shell_workbench(binding->adapter->shell);
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),
                                         binding->view_id,
                                         &document) != UMI_STATUS_OK) return;
    gtk_text_buffer_get_bounds(text_buffer, &start, &end);
    text = gtk_text_buffer_get_text(text_buffer, &start, &end, TRUE);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return;
    /* Defensive recovery for a buffer producer that bypassed insert-text.
     * Restore the complete last accepted snapshot, never a truncated prefix. */
    UmiUiDocumentViewModel *documents = umi_ui_workbench_documents(workbench);
    size_t length = strlen(text);
    UmiStatus status = document.read_only ? UMI_STATUS_PERMISSION_DENIED :
        (length > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES ? UMI_STATUS_CAPACITY_EXCEEDED :
        (!g_utf8_validate(text, -1, NULL) ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK));
    document.dirty = 1;
    document.preview = 0;
    if (status == UMI_STATUS_OK)
        status = UmiUiDocumentViewModelUpsertText(documents, &document, text, length);
    if (status != UMI_STATUS_OK) {
        char *accepted = NULL;
        size_t acceptedLength = 0U;
        UmiGtk4EditorAction *action = g_object_get_data(G_OBJECT(text_buffer), "umicom-editor-action");
        if (action != NULL) action->rejected = true;
        if (UmiUiDocumentViewModelCopyText(documents, document.view_id,
                &accepted, &acceptedLength) == UMI_STATUS_OK) {
            binding->adapter->applying_document_state = 1;
            gtk_text_buffer_set_text(text_buffer, accepted, (int)acceptedLength);
            binding->adapter->applying_document_state = 0;
            UmiUiDocumentViewModelFreeText(accepted);
        }
        gtk_label_set_text(GTK_LABEL(binding->adapter->status_label),
            "Edit not applied: check the document size, read-only state and available memory");
        g_free(text);
        return;
    }
    /* Keep the inline preview for metadata consumers, and record the complete
     * draft revision so a timer cannot replace a tail edit with old text. */
    (void)umi_ui_document_view_model_find(documents, document.view_id, &document);
    EditorRememberRevision(text_buffer, documents, document.view_id);
    UmiUiDocumentViewSnapshot *cached = g_object_get_data(G_OBJECT(text_buffer), "umicom-editor-snapshot");
    if (cached != NULL) (void)g_strlcpy(cached->source_text, document.source_text, sizeof(cached->source_text));
    gtk_label_set_text(GTK_LABEL(binding->adapter->status_label),
                       "Modified — use File / Save to persist the document");
    g_free(text);
}

/* Source-editor shortcuts and clipboard actions share the same coordinator
 * as Edit menu commands. Other text fields keep GTK's normal editing. */
static int EditorHasCommandOwner(UmiGtk4EditorBinding *binding)
{
    return binding != NULL && binding->adapter != NULL &&
        UmiGtk4EditorHasDocument(binding->adapter, binding->view_id) &&
        !binding->adapter->applying_document_state;
}

static void EditorClipboardAction(GtkTextView *view, gpointer data, const char *signal, const char *command)
{
    UmiGtk4EditorBinding *binding = data;
    if (!EditorHasCommandOwner(binding)) return;
    g_signal_stop_emission_by_name(view, signal);
    (void)UmiGtk4EditorCommandForView(binding->adapter, command, binding->view_id);
}
static void EditorCopy(GtkTextView *view, gpointer data)
{ EditorClipboardAction(view, data, "copy-clipboard", "edit.copy"); }
static void EditorCut(GtkTextView *view, gpointer data)
{ EditorClipboardAction(view, data, "cut-clipboard", "edit.cut"); }
static void EditorPaste(GtkTextView *view, gpointer data)
{ EditorClipboardAction(view, data, "paste-clipboard", "edit.paste"); }
static void EditorUndo(GtkTextBuffer *buffer, gpointer data)
{
    UmiGtk4EditorBinding *binding = data;
    if (!EditorHasCommandOwner(binding)) return;
    g_signal_stop_emission_by_name(buffer, "undo");
    (void)UmiGtk4EditorCommandForView(binding->adapter, "edit.undo", binding->view_id);
}
static void EditorRedo(GtkTextBuffer *buffer, gpointer data)
{
    UmiGtk4EditorBinding *binding = data;
    if (!EditorHasCommandOwner(binding)) return;
    g_signal_stop_emission_by_name(buffer, "redo");
    (void)UmiGtk4EditorCommandForView(binding->adapter, "edit.redo", binding->view_id);
}
static gboolean EditorHistoryKey(GtkEventControllerKey *controller, guint keyval,
    guint keycode, GdkModifierType state, gpointer data)
{
    UmiGtk4EditorBinding *binding = data;
    (void)controller; (void)keycode;
    if (!EditorHasCommandOwner(binding)) return FALSE;
    GdkModifierType modifiers = state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK | GDK_ALT_MASK | GDK_SUPER_MASK);
    const char *command = NULL;
    guint key = gdk_keyval_to_lower(keyval);
    if (modifiers == GDK_CONTROL_MASK && key == GDK_KEY_z) command = "edit.undo";
    if ((modifiers == GDK_CONTROL_MASK && key == GDK_KEY_y) ||
        (modifiers == (GDK_CONTROL_MASK | GDK_SHIFT_MASK) && key == GDK_KEY_z)) command = "edit.redo";
    if (modifiers == GDK_CONTROL_MASK && key == GDK_KEY_a) command = "edit.select-all";
    if (command == NULL) return FALSE;
    (void)UmiGtk4EditorCommandForView(binding->adapter, command, binding->view_id);
    return TRUE;
}

/*
 * Provide the editor binding new operation used by this module and its client
 * applications.
 */
static UmiGtk4EditorBinding *editor_binding_new(UmiGtk4Adapter *adapter,
                                                const char *view_id)
{
    UmiGtk4EditorBinding *binding = g_new0(UmiGtk4EditorBinding, 1);
    if (adapter->editor_bindings == NULL) adapter->editor_bindings = g_ptr_array_new();
    binding->adapter = adapter;
    (void)g_strlcpy(binding->view_id,
                    view_id,
                    sizeof(binding->view_id));
    g_ptr_array_add(adapter->editor_bindings, binding);
    return binding;
}

/* GTK iterators count Unicode characters, whereas Framework navigation uses
 * UTF-8 byte positions. Read only the prefix needed for that conversion. */
static size_t editor_iter_byte_offset(GtkTextBuffer *buffer, const GtkTextIter *iter)
{
    GtkTextIter start;
    char *prefix;
    size_t result;
    gtk_text_buffer_get_start_iter(buffer, &start);
    prefix = gtk_text_buffer_get_text(buffer, &start, iter, TRUE);
    result = prefix != NULL ? strlen(prefix) : 0U;
    g_free(prefix);
    return result;
}

/* Convert a validated UTF-8 byte position into GTK's character coordinate.
 * A malformed byte boundary is clamped to the preceding complete character. */
static int editor_byte_character_offset(const char *text, size_t byte_offset)
{
    const size_t length = strlen(text);
    size_t offset = byte_offset < length ? byte_offset : length;
    while (offset > 0U && (((unsigned char)text[offset] & 0xc0U) == 0x80U)) --offset;
    return (int)g_utf8_pointer_to_offset(text, text + offset);
}

/* Keep native caret movement visible to Find, Go To and saved editor state.
 * Programmatic reconciliation blocks this path to avoid a feedback loop. */
static void on_editor_mark_set(GtkTextBuffer *buffer, GtkTextIter *location,
                               GtkTextMark *mark, gpointer user_data)
{
    UmiGtk4EditorBinding *binding = user_data;
    UmiUiDocumentViewSnapshot document;
    UmiUiDocumentViewSnapshot *cached;
    UmiUiWorkbench *workbench;
    GtkTextIter start, end;
    size_t cursor, selection;
    (void)location;
    if (binding == NULL || binding->adapter == NULL || binding->adapter->shell == NULL ||
        binding->adapter->applying_document_state) return;
    if (mark != gtk_text_buffer_get_insert(buffer) && mark != gtk_text_buffer_get_selection_bound(buffer)) return;
    workbench = umi_ui_application_shell_workbench(binding->adapter->shell);
    if (umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench), binding->view_id, &document) != UMI_STATUS_OK) return;
    if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
        cursor = editor_iter_byte_offset(buffer, &start);
        selection = editor_iter_byte_offset(buffer, &end) - cursor;
    } else {
        gtk_text_buffer_get_iter_at_mark(buffer, &start, gtk_text_buffer_get_insert(buffer));
        cursor = editor_iter_byte_offset(buffer, &start);
        selection = 0U;
    }
    if (document.cursor_offset != cursor || document.selection_length != selection) {
        document.cursor_offset = cursor;
        document.selection_length = selection;
        (void)umi_ui_document_view_model_upsert(umi_ui_workbench_documents(workbench), &document);
    }
    cached = g_object_get_data(G_OBJECT(buffer), "umicom-editor-snapshot");
    if (cached != NULL) { cached->cursor_offset = cursor; cached->selection_length = selection; }
}

/* Insertion can move GTK's cursor implicitly without an explicit mark move.
 * The cursor-position notification keeps byte-oriented model state current
 * after normal typing as well as direct selection changes. */
static void on_editor_cursor_position_changed(GObject *object,
                                               GParamSpec *property,
                                               gpointer user_data)
{
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(object);
    (void)property;
    on_editor_mark_set(buffer, NULL, gtk_text_buffer_get_insert(buffer), user_data);
}

/*
 * Provide the on editor close clicked operation used by this module and its client
 * applications.
 */
static void on_editor_close_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4EditorBinding *binding = (UmiGtk4EditorBinding *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiWorkbenchSnapshot workbench_snapshot;
    UmiUiDocumentViewSnapshot document;
    UmiStatus status;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || binding->adapter == NULL ||
        binding->adapter->shell == NULL) return;
    workbench = umi_ui_application_shell_workbench(binding->adapter->shell);
    status = umi_ui_document_view_model_find(
        umi_ui_workbench_documents(workbench), binding->view_id, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (document.dirty) {
        gtk_label_set_text(GTK_LABEL(binding->adapter->status_label),
                           "Save or revert the modified editor before closing it");
        return;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (document.pinned) {
        gtk_label_set_text(GTK_LABEL(binding->adapter->status_label),
                           "Unpin the editor before closing it");
        return;
    }
    status = umi_ui_workbench_snapshot(workbench, &workbench_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return;
    status = umi_ui_document_view_model_remove(
        umi_ui_workbench_documents(workbench), binding->view_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(workbench_snapshot.active_document_view,
                   binding->view_id) == 0) {
            char next_view_id[UMI_UI_ID_CAPACITY];
            const char *closed_group = effective_editor_group(&document);
            status = umi_ui_document_view_model_activate_group(
                umi_ui_workbench_documents(workbench), closed_group,
                next_view_id, sizeof(next_view_id));
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_NOT_FOUND) {
                const char *other_group = strcmp(
                    closed_group, UMI_UI_PRIMARY_EDITOR_GROUP_ID) == 0
                    ? UMI_UI_SECONDARY_EDITOR_GROUP_ID
                    : UMI_UI_PRIMARY_EDITOR_GROUP_ID;
                status = umi_ui_document_view_model_activate_group(
                    umi_ui_workbench_documents(workbench), other_group,
                    next_view_id, sizeof(next_view_id));
            }
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) {
                (void)umi_ui_workbench_activate_document(workbench,
                                                         next_view_id);
            }
        }
        (void)umi_gtk4_refresh_documents(binding->adapter, workbench);
    }
}

/*
 * Provide the on editor pin clicked operation used by this module and its client
 * applications.
 */
static void on_editor_pin_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4EditorBinding *binding = (UmiGtk4EditorBinding *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiDocumentViewSnapshot document;
    UmiStatus status;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || binding->adapter == NULL ||
        binding->adapter->shell == NULL) return;
    workbench = umi_ui_application_shell_workbench(binding->adapter->shell);
    status = umi_ui_document_view_model_find(
        umi_ui_workbench_documents(workbench), binding->view_id, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return;
    status = umi_ui_document_view_model_set_pinned(
        umi_ui_workbench_documents(workbench),
        binding->view_id,
        !document.pinned);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        (void)umi_gtk4_refresh_documents(binding->adapter, workbench);
    }
}

/*
 * Provide the create editor widget operation used by this module and its client
 * applications.
 */
static GtkWidget *create_editor_widget(UmiUiWorkbench *workbench,
                                       const UmiUiDocumentViewSnapshot *document,
                                       GtkTextBuffer **out_buffer)
{
    GtkWidget *view;
    UmiEditorPresentationSnapshot presentation;
    UmiStatus presentation_status = umi_editor_presentation_default(
        document->language_id, &presentation);
#if defined(UMICOM_GTK4_HAS_SOURCEVIEW5)
    GtkSourceBuffer *source_buffer = gtk_source_buffer_new(NULL);
    GtkSourceLanguageManager *languages = gtk_source_language_manager_get_default();
    GtkSourceLanguage *language = NULL;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (document->language_id[0] != '\0') {
        language = gtk_source_language_manager_get_language(
            languages, document->language_id);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (language != NULL) gtk_source_buffer_set_language(source_buffer, language);
    gtk_source_buffer_set_highlight_syntax(source_buffer, TRUE);
    view = gtk_source_view_new_with_buffer(source_buffer);
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view),
                                          document->show_line_numbers != 0);
    gtk_source_view_set_auto_indent(
        GTK_SOURCE_VIEW(view), presentation_status == UMI_STATUS_OK
            ? presentation.auto_indent != 0 : TRUE);
    gtk_source_view_set_indent_on_tab(
        GTK_SOURCE_VIEW(view), presentation_status == UMI_STATUS_OK
            ? presentation.indent_on_tab != 0 : TRUE);
    gtk_source_view_set_tab_width(
        GTK_SOURCE_VIEW(view), presentation_status == UMI_STATUS_OK
            ? presentation.tab_width : 4U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (presentation_status == UMI_STATUS_OK) {
        gtk_source_buffer_set_highlight_matching_brackets(
            source_buffer, presentation.highlight_matching_brackets != 0);
        gtk_source_view_set_right_margin_position(
            GTK_SOURCE_VIEW(view), presentation.right_margin_column);
        gtk_source_view_set_show_right_margin(
            GTK_SOURCE_VIEW(view), presentation.show_right_margin != 0);
    }
    *out_buffer = GTK_TEXT_BUFFER(source_buffer);
    g_object_unref(source_buffer);
#else
    (void)presentation_status;
    (void)presentation;
    view = gtk_text_view_new();
    *out_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
#endif
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),
                                document->word_wrap
                                    ? GTK_WRAP_WORD_CHAR
                                    : GTK_WRAP_NONE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view), 8);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(view), 8);
    gtk_widget_add_css_class(view, "umicom-editor");
    (void)umi_gtk4_configure_editor_theme(view, workbench);
    return view;
}

/*
 * Provide the gtk4 refresh documents operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_documents(UmiGtk4Adapter *adapter,
                                      UmiUiWorkbench *workbench)
{
    UmiUiDocumentViewModel *documents;
    UmiUiWorkbenchState state;
    size_t index;
    int group_positions[2] = {0, 0};

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->document_notebook == NULL || adapter->secondary_document_notebook == NULL)
        return UMI_STATUS_INVALID_STATE;
    if (!adapter->editor_notebooks_initialised) {
        g_weak_ref_init(&adapter->editor_notebooks[0], adapter->document_notebook);
        g_weak_ref_init(&adapter->editor_notebooks[1], adapter->secondary_document_notebook);
        adapter->editor_notebooks_initialised = 1;
    }
    documents = umi_ui_workbench_documents(workbench);
    /* Stage only changed text/selection before touching pages. Routine status
     * refreshes read metadata and revisions, not every full source file. */
    char *fullTexts[UMI_UI_DOCUMENT_VIEW_MAX] = {0};
    size_t fullLengths[UMI_UI_DOCUMENT_VIEW_MAX] = {0};
    for (index = 0U; index < umi_ui_document_view_model_count(documents); ++index) {
        UmiUiDocumentViewSnapshot document;
        UmiUiDocumentTextInfo info;
        GtkWidget *notebook = NULL;
        UmiStatus status = umi_ui_document_view_model_at(documents, index, &document);
        if (status == UMI_STATUS_OK)
            status = UmiUiDocumentViewModelTextInfo(documents, document.view_id, &info);
        GtkWidget *page = status == UMI_STATUS_OK
            ? find_document_page(adapter, document.view_id, &notebook) : NULL;
        GtkWidget *view = page != NULL ? g_object_get_data(G_OBJECT(page), "umicom-editor-view") : NULL;
        GtkTextBuffer *buffer = view != NULL ? gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)) : NULL;
        const UmiUiDocumentViewSnapshot *previous = buffer != NULL
            ? g_object_get_data(G_OBJECT(buffer), "umicom-editor-snapshot") : NULL;
        if (status == UMI_STATUS_OK && (previous == NULL ||
            strcmp(previous->document_id, document.document_id) != 0 ||
            EditorContentRevision(buffer) != info.text_revision ||
            previous->cursor_offset != document.cursor_offset ||
            previous->selection_length != document.selection_length)) {
            status = UmiUiDocumentViewModelCopyText(documents, document.view_id,
                &fullTexts[index], &fullLengths[index]);
            if (status == UMI_STATUS_OK && !g_utf8_validate(fullTexts[index], (gssize)fullLengths[index], NULL))
                status = UMI_STATUS_INVALID_ARGUMENT;
        }
        if (status != UMI_STATUS_OK) {
            for (size_t release = 0U; release <= index; ++release)
                UmiUiDocumentViewModelFreeText(fullTexts[release]);
            return status;
        }
    }
    adapter->applying_document_state = 1;
    prune_notebook(adapter, adapter->document_notebook, documents);
    prune_notebook(adapter, adapter->secondary_document_notebook, documents);
    /* Store and disconnect the signal ID rather than using GLib's callback
     * matching convenience macro. That macro passes a function pointer through
     * gpointer, which ISO C correctly diagnoses under -Wpedantic because data
     * and function pointers are distinct types. */
    if (adapter->document_page_switch_handler != 0UL) {
        g_signal_handler_disconnect(adapter->document_notebook,
                                    adapter->document_page_switch_handler);
        adapter->document_page_switch_handler = 0UL;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (adapter->secondary_document_page_switch_handler != 0UL) {
        g_signal_handler_disconnect(
            adapter->secondary_document_notebook,
            adapter->secondary_document_page_switch_handler);
        adapter->secondary_document_page_switch_handler = 0UL;
    }
    (void)umi_ui_workbench_state_snapshot(workbench, &state);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(state.active_editor_group,
               UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0) {
        gtk_widget_remove_css_class(adapter->document_notebook, "active");
        gtk_widget_add_css_class(adapter->secondary_document_notebook,
                                 "active");
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        gtk_widget_add_css_class(adapter->document_notebook, "active");
        gtk_widget_remove_css_class(adapter->secondary_document_notebook,
                                    "active");
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_ui_document_view_model_count(documents); ++index) {
        UmiUiDocumentViewSnapshot document;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_ui_document_view_model_at(documents, index, &document) == UMI_STATUS_OK) {
            GtkWidget *old_notebook = NULL;
            GtkWidget *scroll = find_document_page(adapter, document.view_id, &old_notebook);
            const bool new_page = scroll == NULL;
            GtkTextBuffer *text_buffer = NULL;
            GtkWidget *view = new_page ? create_editor_widget(workbench, &document, &text_buffer)
                : g_object_get_data(G_OBJECT(scroll), "umicom-editor-view");
            UmiUiDocumentViewSnapshot *previous;
            bool tab_changed;
            bool text_changed = false;
            GtkWidget *tab_box;
            GtkWidget *tab;
            GtkWidget *notebook = notebook_for_group(
                adapter, effective_editor_group(&document));
            const size_t group_index = notebook == adapter->secondary_document_notebook ? 1U : 0U;
            int page_index;

            if (!new_page) text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
            previous = g_object_get_data(G_OBJECT(text_buffer), "umicom-editor-snapshot");
            tab_changed = previous == NULL || strcmp(previous->title, document.title) != 0 ||
                strcmp(previous->icon_name, document.icon_name) != 0 ||
                previous->dirty != document.dirty || previous->preview != document.preview ||
                previous->read_only != document.read_only || previous->pinned != document.pinned ||
                previous->closable != document.closable;
            if (new_page) scroll = gtk_scrolled_window_new();
            tab_box = tab_changed ? gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4)
                : gtk_notebook_get_tab_label(GTK_NOTEBOOK(old_notebook), scroll);
            tab = tab_changed ? gtk_label_new(document.title) : NULL;

            gtk_widget_add_css_class(scroll, "umicom-editor-scroll");
            /* Rebuild only changed tab decoration, never the editor beneath it.
             * Routine status ticks leave tab focus and controls intact too. */
            if (tab_changed) {
            gtk_widget_add_css_class(tab_box, "umicom-document-tab");
            /* Apply this branch only when its contract condition is satisfied. */
            if (document.icon_name[0] != '\0') {
                GtkWidget *icon = gtk_image_new_from_icon_name(document.icon_name);
                gtk_widget_add_css_class(icon, "umicom-document-icon");
                gtk_box_append(GTK_BOX(tab_box), icon);
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (document.dirty) {
                GtkWidget *dirty = gtk_label_new("●");
                gtk_widget_add_css_class(dirty, "accent");
                gtk_widget_set_tooltip_text(dirty, "Modified — not yet saved");
                gtk_box_append(GTK_BOX(tab_box), dirty);
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (document.preview) {
                gtk_widget_add_css_class(tab_box, "preview");
            }
            gtk_box_append(GTK_BOX(tab_box), tab);
            /* Apply this branch only when its contract condition is satisfied. */
            if (document.read_only) {
                GtkWidget *read_only =
                    gtk_image_new_from_icon_name("changes-prevent-symbolic");
                gtk_widget_add_css_class(read_only, "umicom-document-state");
                gtk_widget_set_tooltip_text(read_only, "Read-only editor");
                gtk_box_append(GTK_BOX(tab_box), read_only);
            }
            {
                GtkWidget *pin = gtk_button_new_from_icon_name(
                    document.pinned
                        ? "view-pin-symbolic"
                        : "view-pin-outline-symbolic");
                UmiGtk4EditorBinding *pin_binding =
                    editor_binding_new(adapter, document.view_id);
                gtk_widget_add_css_class(pin, "umicom-document-tab-button");
                gtk_widget_set_can_focus(pin, FALSE);
                gtk_widget_set_tooltip_text(
                    pin, document.pinned ? "Unpin editor" : "Pin editor");
                g_signal_connect_data(pin,
                                      "clicked",
                                      G_CALLBACK(on_editor_pin_clicked),
                                      pin_binding,
                                      editor_binding_free,
                                      0);
                gtk_box_append(GTK_BOX(tab_box), pin);
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (document.closable) {
                GtkWidget *close =
                    gtk_button_new_from_icon_name("window-close-symbolic");
                UmiGtk4EditorBinding *close_binding =
                    editor_binding_new(adapter, document.view_id);
                gtk_widget_add_css_class(close, "umicom-document-tab-button");
                gtk_widget_set_can_focus(close, FALSE);
                gtk_widget_set_tooltip_text(close, "Close editor");
                g_signal_connect_data(close,
                                      "clicked",
                                      G_CALLBACK(on_editor_close_clicked),
                                      close_binding,
                                      editor_binding_free,
                                      0);
                gtk_box_append(GTK_BOX(tab_box), close);
            }
            }
            /* Compare the last applied model copy, not unrelated workbench
             * revisions. Native typing already updated its existing buffer. */
            if (fullTexts[index] != NULL) {
                GtkTextIter start, end;
                char *current_text;
                gtk_text_buffer_get_bounds(text_buffer, &start, &end);
                current_text = gtk_text_buffer_get_text(text_buffer, &start, &end, TRUE);
                if (current_text == NULL || strcmp(current_text, fullTexts[index]) != 0) {
                    /* An authoritative external edit supersedes a pending
                     * native action; its old draft must not be replayed. */
                    g_object_set_data(G_OBJECT(text_buffer), "umicom-editor-action", NULL);
                    gtk_text_buffer_set_text(text_buffer, fullTexts[index], (int)fullLengths[index]);
                    text_changed = true;
                }
                g_free(current_text);
            }
            if (fullTexts[index] != NULL && (previous == NULL || text_changed ||
                previous->cursor_offset != document.cursor_offset ||
                previous->selection_length != document.selection_length))
            {
                GtkTextIter cursor;
                GtkTextIter selection_end;
                size_t text_length = fullLengths[index];
                size_t cursor_offset = document.cursor_offset <= text_length
                    ? document.cursor_offset : text_length;
                size_t selection_length = document.selection_length <=
                    text_length - cursor_offset
                    ? document.selection_length : text_length - cursor_offset;
                gtk_text_buffer_get_iter_at_offset(text_buffer, &cursor,
                    editor_byte_character_offset(fullTexts[index], cursor_offset));
                gtk_text_buffer_get_iter_at_offset(text_buffer, &selection_end,
                    editor_byte_character_offset(fullTexts[index], cursor_offset + selection_length));
                gtk_text_buffer_select_range(text_buffer, &cursor, &selection_end);
            }
            gtk_text_view_set_editable(GTK_TEXT_VIEW(view),
                                       !document.read_only);
            gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),
                document.word_wrap ? GTK_WRAP_WORD_CHAR : GTK_WRAP_NONE);
            /* Presentation changes do not require a different GtkTextView. */
#if defined(UMICOM_GTK4_HAS_SOURCEVIEW5)
            if (GTK_SOURCE_IS_VIEW(view)) {
                GtkSourceLanguage *language = document.language_id[0] != '\0'
                    ? gtk_source_language_manager_get_language(gtk_source_language_manager_get_default(), document.language_id)
                    : NULL;
                gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(text_buffer), language);
                gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), document.show_line_numbers != 0);
            }
#endif
            (void)umi_gtk4_configure_editor_theme(view, workbench);
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                           GTK_POLICY_AUTOMATIC,
                                           GTK_POLICY_AUTOMATIC);
            if (new_page) {
            UmiGtk4EditorBinding *binding = editor_binding_new(adapter, document.view_id);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
            g_object_set_data(G_OBJECT(scroll), "umicom-editor-view", view);
            g_object_set_data_full(G_OBJECT(scroll), "umicom-view-id",
                                   g_strdup(document.view_id), g_free);
            g_signal_connect_data(text_buffer,
                                  "changed",
                                  G_CALLBACK(on_editor_buffer_changed),
                                  binding,
                                  editor_binding_free,
                                  0);
            g_signal_connect_data(text_buffer, "mark-set", G_CALLBACK(on_editor_mark_set),
                editor_binding_new(adapter, document.view_id), editor_binding_free, 0);
            g_signal_connect_data(text_buffer, "notify::cursor-position", G_CALLBACK(on_editor_cursor_position_changed),
                editor_binding_new(adapter, document.view_id), editor_binding_free, 0);
            g_signal_connect_data(text_buffer, "insert-text", G_CALLBACK(on_editor_insert_text),
                editor_binding_new(adapter, document.view_id), editor_binding_free, 0);
            g_signal_connect_data(text_buffer, "begin-user-action", G_CALLBACK(on_editor_begin_user_action),
                editor_binding_new(adapter, document.view_id), editor_binding_free, 0);
            g_signal_connect_data(text_buffer, "end-user-action", G_CALLBACK(on_editor_end_user_action),
                editor_binding_new(adapter, document.view_id), editor_binding_free, 0);
            g_signal_connect_data(view, "copy-clipboard", G_CALLBACK(EditorCopy),
                editor_binding_new(adapter, document.view_id), editor_binding_free, 0);
            g_signal_connect_data(view, "cut-clipboard", G_CALLBACK(EditorCut),
                editor_binding_new(adapter, document.view_id), editor_binding_free, 0);
            g_signal_connect_data(view, "paste-clipboard", G_CALLBACK(EditorPaste),
                editor_binding_new(adapter, document.view_id), editor_binding_free, 0);
            g_signal_connect_data(text_buffer, "undo", G_CALLBACK(EditorUndo),
                editor_binding_new(adapter, document.view_id), editor_binding_free, 0);
            g_signal_connect_data(text_buffer, "redo", G_CALLBACK(EditorRedo),
                editor_binding_new(adapter, document.view_id), editor_binding_free, 0);
            GtkEventController *historyKeys = gtk_event_controller_key_new();
            gtk_event_controller_set_propagation_phase(historyKeys, GTK_PHASE_CAPTURE);
            g_signal_connect_data(historyKeys, "key-pressed", G_CALLBACK(EditorHistoryKey),
                editor_binding_new(adapter, document.view_id), editor_binding_free, 0);
            gtk_widget_add_controller(view, historyKeys);
            }
            if (new_page) {
                page_index = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, tab_box);
            } else if (old_notebook != notebook) {
                /* Both child and tab need temporary references while neither
                 * notebook owns them. Reparenting preserves their live state. */
                g_object_ref(scroll);
                g_object_ref_sink(tab_box);
                gtk_notebook_remove_page(GTK_NOTEBOOK(old_notebook),
                    gtk_notebook_page_num(GTK_NOTEBOOK(old_notebook), scroll));
                page_index = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, tab_box);
                g_object_unref(tab_box);
                g_object_unref(scroll);
            } else {
                page_index = gtk_notebook_page_num(GTK_NOTEBOOK(notebook), scroll);
                if (tab_changed) gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), scroll, tab_box);
            }
            gtk_notebook_reorder_child(GTK_NOTEBOOK(notebook), scroll, group_positions[group_index]++);
            page_index = gtk_notebook_page_num(GTK_NOTEBOOK(notebook), scroll);
            gtk_notebook_set_tab_reorderable(
                GTK_NOTEBOOK(notebook),
                scroll,
                TRUE);
            gtk_notebook_set_tab_detachable(
                GTK_NOTEBOOK(notebook),
                scroll,
                TRUE);
            gtk_widget_set_tooltip_text(tab_box,
                document.uri[0] != '\0' ? document.uri : document.document_id);
            if (previous == NULL) {
                previous = g_new0(UmiUiDocumentViewSnapshot, 1);
                g_object_set_data_full(G_OBJECT(text_buffer), "umicom-editor-snapshot", previous, g_free);
            }
            *previous = document;
            EditorRememberRevision(text_buffer, documents, document.view_id);
            /* Apply this operation only while the related capability or state is available. */
            if (document.active) {
                gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook),
                                              page_index);
            }
        }
    }
    for (size_t release = 0U; release < UMI_UI_DOCUMENT_VIEW_MAX; ++release)
        UmiUiDocumentViewModelFreeText(fullTexts[release]);
    adapter->document_page_switch_handler =
        g_signal_connect(adapter->document_notebook,
                         "switch-page",
                         G_CALLBACK(on_document_page_switched),
                         adapter);
    adapter->secondary_document_page_switch_handler =
        g_signal_connect(adapter->secondary_document_notebook,
                         "switch-page",
                         G_CALLBACK(on_document_page_switched),
                         adapter);
    /* Apply this branch only when its contract condition is satisfied. */
    if (adapter->document_page_added_handler == 0UL) {
        adapter->document_page_added_handler =
            g_signal_connect(adapter->document_notebook,
                             "page-added",
                             G_CALLBACK(on_document_page_added),
                             adapter);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (adapter->document_page_reordered_handler == 0UL) {
        adapter->document_page_reordered_handler =
            g_signal_connect(adapter->document_notebook,
                             "page-reordered",
                             G_CALLBACK(on_document_page_reordered),
                             adapter);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (adapter->secondary_document_page_added_handler == 0UL) {
        adapter->secondary_document_page_added_handler =
            g_signal_connect(adapter->secondary_document_notebook,
                             "page-added",
                             G_CALLBACK(on_document_page_added),
                             adapter);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (adapter->secondary_document_page_reordered_handler == 0UL) {
        adapter->secondary_document_page_reordered_handler =
            g_signal_connect(adapter->secondary_document_notebook,
                             "page-reordered",
                             G_CALLBACK(on_document_page_reordered),
                             adapter);
    }
    adapter->applying_document_state = 0;
    return UMI_STATUS_OK;
}
