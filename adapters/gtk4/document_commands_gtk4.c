/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_commands_gtk4.c
 * PURPOSE:
 *   Route source editing through the document owner and protect asynchronous
 *   clipboard completion with a captured edit plan and an invalidatable token.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"
#include "umicom/ui/gtk4/automation.h"
#include <string.h>

/* The request owns its token and edit capture, not the window or coordinator. */
typedef struct ClipboardRequest {
    GObject *lifetime;
    UmiDocumentEditPlan *plan;
} ClipboardRequest;

static UmiStatus EditFindDocument(UmiGtk4Adapter *adapter, const char *viewId,
    UmiDocumentId *outId)
{
    if (adapter == NULL || adapter->edit_coordinator == NULL) return UMI_STATUS_UNAVAILABLE;
    UmiDocumentWorkingCopySnapshot snapshot;
    if (viewId == NULL) {
        UmiStatus status = umi_document_coordinator_active_snapshot(adapter->edit_coordinator, &snapshot);
        if (status == UMI_STATUS_OK) *outId = snapshot.document_id;
        return status;
    }
    for (size_t index = 0U; index < umi_document_coordinator_count(adapter->edit_coordinator); ++index) {
        UmiStatus status = umi_document_coordinator_at(adapter->edit_coordinator, index, &snapshot);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(snapshot.view_id, viewId) == 0) { *outId = snapshot.document_id; return UMI_STATUS_OK; }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Refresh first, notify last: application notification may destroy the owner. */
/* REFACTOR NOTE: Completion remains in Framework. The lifetime check below
 * prevents an old host callback after document refresh detaches its binding.
 * The former code is retained below for reference; do not enable both paths.
 */
// static UmiStatus EditFinished(UmiGtk4Adapter *adapter, UmiStatus status)
// {
//     UmiGtk4DocumentEditResultFn callback = adapter->edit_completed;
//     void *context = adapter->edit_context;
//     if (adapter->shell != NULL && adapter->window != NULL) {
//         UmiStatus refresh = umi_gtk4_refresh_documents(adapter,
//             umi_ui_application_shell_workbench(adapter->shell));
//         if (status == UMI_STATUS_OK) status = refresh;
//     }
//     if (callback != NULL) callback(context, status);
//     return status;
// }

static UmiStatus EditFinished(UmiGtk4Adapter *adapter, UmiStatus status)
{
    if (adapter == NULL) return status;
    GObject *lifetime = adapter->edit_lifetime != NULL
        ? g_object_ref(adapter->edit_lifetime) : NULL;
    UmiGtk4DocumentEditResultFn callback = adapter->edit_completed;
    void *context = adapter->edit_context;
    if (adapter->shell != NULL && adapter->window != NULL) {
        UmiStatus refresh = umi_gtk4_refresh_documents(adapter,
            umi_ui_application_shell_workbench(adapter->shell));
        if (status == UMI_STATUS_OK) status = refresh;
    }
    /* Refresh can notify native observers. Do not call a context belonging to
     * a binding that an observer detached or replaced while it was running. */
    int current = lifetime != NULL && g_object_get_data(lifetime, "adapter") == adapter;
    g_clear_object(&lifetime);
    if (current && callback != NULL) callback(context, status);
    return status;
}

static void EditWindowDestroyed(GtkWidget *window, gpointer data)
{
    (void)window;
    (void)UmiGtk4AdapterBindDocumentEditing(data, NULL, NULL, NULL);
}

UmiStatus UmiGtk4AdapterBindDocumentEditing(UmiGtk4Adapter *adapter,
    UmiDocumentCoordinator *coordinator, UmiGtk4DocumentEditResultFn completed,
    void *context)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (coordinator != NULL && (adapter->window == NULL || adapter->shell == NULL))
        return UMI_STATUS_INVALID_STATE;
    /* The former invalidation is retained: the same token now also owns a
     * location form, which must be closed before releasing the binding.
     * if (adapter->edit_lifetime != NULL)
     *     g_object_set_data(adapter->edit_lifetime, "adapter", NULL);
     */
    if (adapter->edit_lifetime != NULL) {
        GtkWidget *dialog = g_object_get_data(adapter->edit_lifetime, "location-dialog");
        g_object_set_data(adapter->edit_lifetime, "adapter", NULL);
        g_object_set_data(adapter->edit_lifetime, "location-dialog", NULL);
        if (dialog != NULL) gtk_window_destroy(GTK_WINDOW(dialog));
    }
    UmiGtk4DocumentSaveAllDetach(adapter);
    if (adapter->edit_cancel != NULL) g_cancellable_cancel(adapter->edit_cancel);
    g_clear_object(&adapter->edit_cancel);
    g_clear_object(&adapter->edit_lifetime);
    if (adapter->edit_window_destroy_handler != 0UL && adapter->window != NULL &&
        g_signal_handler_is_connected(adapter->window, adapter->edit_window_destroy_handler))
        g_signal_handler_disconnect(adapter->window, adapter->edit_window_destroy_handler);
    adapter->edit_window_destroy_handler = 0UL;
    adapter->edit_coordinator = coordinator;
    adapter->edit_completed = completed;
    adapter->edit_context = context;
    if (coordinator != NULL) {
        adapter->edit_lifetime = g_object_new(G_TYPE_OBJECT, NULL);
        g_object_set_data(adapter->edit_lifetime, "adapter", adapter);
        adapter->edit_window_destroy_handler = g_signal_connect(adapter->window,
            "destroy", G_CALLBACK(EditWindowDestroyed), adapter);
    }
    return UMI_STATUS_OK;
}

static void ClipboardReadComplete(GObject *source, GAsyncResult *result, gpointer data)
{
    ClipboardRequest *request = data;
    GError *error = NULL;
    char *text = gdk_clipboard_read_text_finish(GDK_CLIPBOARD(source), result, &error);
    UmiGtk4Adapter *adapter = g_object_get_data(request->lifetime, "adapter");
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (adapter != NULL) {
        status = text != NULL ? UmiDocumentCoordinatorApplyEdit(adapter->edit_coordinator,
            request->plan, text, strlen(text)) :
            (error != NULL && g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED)
                ? UMI_STATUS_CANCELLED : UMI_STATUS_UNAVAILABLE);
        g_clear_object(&adapter->edit_cancel);
    }
    g_free(text);
    g_clear_error(&error);
    UmiDocumentEditPlanDestroy(request->plan);
    g_object_unref(request->lifetime);
    g_free(request);
    if (adapter != NULL) (void)EditFinished(adapter, status);
}

int UmiGtk4AdapterDocumentCommandEnabled(UmiGtk4Adapter *adapter, const char *commandId)
{
    UmiDocumentEditCommand command;
    UmiDocumentId id = 0U;
    UmiDocumentEditState state;
    if (UmiDocumentEditCommandFromId(commandId, &command) != UMI_STATUS_OK ||
        EditFindDocument(adapter, NULL, &id) != UMI_STATUS_OK ||
        UmiDocumentCoordinatorGetEditState(adapter->edit_coordinator, id, &state) != UMI_STATUS_OK)
        return 0;
    switch (command) {
    case UMI_DOCUMENT_EDIT_COPY: return state.selection_bytes != 0U;
    case UMI_DOCUMENT_EDIT_SELECT_ALL: return state.text_bytes != 0U;
    case UMI_DOCUMENT_EDIT_UNDO: return state.can_undo;
    case UMI_DOCUMENT_EDIT_REDO: return state.can_redo;
    case UMI_DOCUMENT_EDIT_PASTE: return !state.read_only && adapter->edit_cancel == NULL;
    case UMI_DOCUMENT_EDIT_CUT:
    case UMI_DOCUMENT_EDIT_DELETE: return !state.read_only && state.selection_bytes != 0U;
    }
    return 0;
}

int UmiGtk4EditorHasDocument(UmiGtk4Adapter *adapter, const char *viewId)
{
    UmiDocumentId id = 0U;
    return EditFindDocument(adapter, viewId, &id) == UMI_STATUS_OK;
}

UmiStatus UmiGtk4EditorSynchronise(UmiGtk4Adapter *adapter, const char *viewId)
{
    UmiDocumentId id = 0U;
    UmiStatus status = EditFindDocument(adapter, viewId, &id);
    return status == UMI_STATUS_OK ? UmiDocumentCoordinatorSyncDocument(adapter->edit_coordinator, id) : status;
}

UmiStatus UmiGtk4EditorCommandForView(UmiGtk4Adapter *adapter,
    const char *commandId, const char *viewId)
{
    UmiDocumentEditCommand command;
    UmiDocumentEditState state;
    UmiDocumentId id = 0U;
/* REFACTOR NOTE: Early mapping and lookup failures now enter EditFinished,
 * the same Framework result path used by successful dispatch. Studio no longer
 * loses these errors when a toolbar discards the immediate return value.
 * The former code is retained below for reference; do not enable both paths.
 */
//     UmiStatus status = UmiDocumentEditCommandFromId(commandId, &command);
//     if (status != UMI_STATUS_OK) return status;
//     status = EditFindDocument(adapter, viewId, &id);
//     if (status != UMI_STATUS_OK) return status;

    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDocumentEditCommandFromId(commandId, &command);
    if (status != UMI_STATUS_OK) return EditFinished(adapter, status);
    status = EditFindDocument(adapter, viewId, &id);
    if (status != UMI_STATUS_OK) return EditFinished(adapter, status);
    status = UmiDocumentCoordinatorGetEditState(adapter->edit_coordinator, id, &state);
    if (status != UMI_STATUS_OK) return EditFinished(adapter, status);
    if (command == UMI_DOCUMENT_EDIT_UNDO)
        return EditFinished(adapter, UmiDocumentCoordinatorUndo(adapter->edit_coordinator, id));
    if (command == UMI_DOCUMENT_EDIT_REDO)
        return EditFinished(adapter, UmiDocumentCoordinatorRedo(adapter->edit_coordinator, id));
    if (command == UMI_DOCUMENT_EDIT_SELECT_ALL)
        return EditFinished(adapter, UmiDocumentCoordinatorSelectAll(adapter->edit_coordinator, id));
    if (command != UMI_DOCUMENT_EDIT_COPY && state.read_only)
        return EditFinished(adapter, UMI_STATUS_PERMISSION_DENIED);
    if (command == UMI_DOCUMENT_EDIT_PASTE && adapter->edit_cancel != NULL)
        return EditFinished(adapter, UMI_STATUS_BUSY);
    if (command != UMI_DOCUMENT_EDIT_PASTE && state.selection_bytes == 0U)
        return EditFinished(adapter, UMI_STATUS_NOT_FOUND);
    UmiDocumentEditPlan *plan = NULL;
    status = UmiDocumentCoordinatorPrepareEdit(adapter->edit_coordinator, id, &plan);
    if (status != UMI_STATUS_OK) return EditFinished(adapter, status);
    GdkClipboard *clipboard = gtk_widget_get_clipboard(GTK_WIDGET(adapter->window));
    if (command == UMI_DOCUMENT_EDIT_PASTE) {
        ClipboardRequest *request = g_try_new0(ClipboardRequest, 1);
        if (request == NULL) { UmiDocumentEditPlanDestroy(plan); return EditFinished(adapter, UMI_STATUS_OUT_OF_MEMORY); }
        request->lifetime = g_object_ref(adapter->edit_lifetime);
        request->plan = plan;
        adapter->edit_cancel = g_cancellable_new();
        gdk_clipboard_read_text_async(clipboard, adapter->edit_cancel, ClipboardReadComplete, request);
        return UMI_STATUS_OK;
    }
    /* Clipboard publication can emit a synchronous notification. A subscriber
     * may detach the editor, so retain the token rather than trust the adapter
     * pointer after publishing. No deletion occurs after such a detach. */
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    if (command == UMI_DOCUMENT_EDIT_COPY || command == UMI_DOCUMENT_EDIT_CUT) {
        const char *selected = NULL;
        size_t bytes = 0U;
        status = UmiDocumentEditPlanSelection(plan, &selected, &bytes);
        if (status == UMI_STATUS_OK) {
            char *copy = g_try_malloc(bytes + 1U);
            if (copy == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                memcpy(copy, selected, bytes); copy[bytes] = '\0';
                GdkContentProvider *provider = gdk_content_provider_new_typed(G_TYPE_STRING, copy);
                status = gdk_clipboard_set_content(clipboard, provider) ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
                g_object_unref(provider); g_free(copy);
            }
        }
    }
    adapter = g_object_get_data(lifetime, "adapter");
    if (adapter == NULL) status = UMI_STATUS_CANCELLED;
    else if (status == UMI_STATUS_OK && command != UMI_DOCUMENT_EDIT_COPY)
        status = UmiDocumentCoordinatorApplyEdit(adapter->edit_coordinator, plan, "", 0U);
    UmiDocumentEditPlanDestroy(plan);
    g_object_unref(lifetime);
    return adapter != NULL ? EditFinished(adapter, status) : status;
}

UmiStatus UmiGtk4AdapterDocumentCommand(UmiGtk4Adapter *adapter, const char *commandId)
{
    return UmiGtk4EditorCommandForView(adapter, commandId, NULL);
}

/* A host uses this only to choose who displays an immediate failure. It does
 * not execute a command and does not replace checks at dispatch time. */
int UmiGtk4AdapterDocumentHasCompletion(UmiGtk4Adapter *adapter)
{
    return adapter != NULL && adapter->edit_lifetime != NULL &&
        adapter->edit_coordinator != NULL && adapter->edit_completed != NULL;
}

UmiStatus UmiGtk4AdapterDocumentNavigate(UmiGtk4Adapter *adapter, const char *location)
{
    UmiDocumentId id = 0U;
    size_t length = location != NULL ? strlen(location) : 0U;
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = EditFindDocument(adapter, NULL, &id);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentCoordinatorNavigate(adapter->edit_coordinator,
            id, location, length, NULL);
    return EditFinished(adapter, status);
}

/* The form retains the existing binding token, never the borrowed coordinator.
 * This uses the same ownership model as delayed clipboard completion. */
typedef struct LocationPrompt {
    GObject *lifetime;
    UmiDocumentId document;
    GtkWidget *window;
    GtkWidget *entry;
    GtkWidget *message;
} LocationPrompt;

static void LocationPromptFree(gpointer data)
{
    LocationPrompt *prompt = data;
    g_object_unref(prompt->lifetime);
    g_free(prompt);
}

static void LocationPromptDestroyed(GtkWidget *widget, gpointer data)
{
    LocationPrompt *prompt = data;
    if (g_object_get_data(prompt->lifetime, "location-dialog") == widget)
        g_object_set_data(prompt->lifetime, "location-dialog", NULL);
}

/* Closing invalidates the form immediately, even when a test or native owner
 * retains the GtkWindow. Widget disposal may happen later than window closure. */
static void LocationPromptClose(GtkWidget *window)
{
    LocationPrompt *prompt = g_object_get_data(G_OBJECT(window), "umicom-location-prompt");
    if (prompt != NULL && g_object_get_data(prompt->lifetime, "location-dialog") == window)
        g_object_set_data(prompt->lifetime, "location-dialog", NULL);
    gtk_window_destroy(GTK_WINDOW(window));
}

static gboolean LocationPromptClosing(GtkWindow *window, gpointer data)
{
    (void)data;
    LocationPrompt *prompt = g_object_get_data(G_OBJECT(window), "umicom-location-prompt");
    if (prompt != NULL && g_object_get_data(prompt->lifetime, "location-dialog") == window)
        g_object_set_data(prompt->lifetime, "location-dialog", NULL);
    return FALSE; /* Let the window perform its normal close operation. */
}

static void LocationPromptCancel(GtkButton *button, gpointer data)
{
    (void)button;
    LocationPromptClose(GTK_WIDGET(data));
}

static void LocationPromptAccept(GtkWidget *source, gpointer data)
{
    GtkWidget *window = data;
    LocationPrompt *prompt = g_object_get_data(G_OBJECT(window), "umicom-location-prompt");
    if (prompt == NULL || gtk_widget_get_root(source) != GTK_ROOT(window) ||
        g_object_get_data(prompt->lifetime, "location-dialog") != window) return;
    UmiGtk4Adapter *adapter = g_object_get_data(prompt->lifetime, "adapter");
    if (adapter == NULL) { LocationPromptClose(window); return; }
    const char *location = gtk_editable_get_text(GTK_EDITABLE(prompt->entry));
    GObject *lifetime = g_object_ref(prompt->lifetime);
    UmiStatus status = UmiDocumentCoordinatorNavigate(adapter->edit_coordinator,
        prompt->document, location, strlen(location), NULL);
    /* Update or close the form BEFORE dispatching completion: the host callback
     * may destroy the adapter/window. Do not read prompt or adapter afterwards. */
    if (status == UMI_STATUS_OK) LocationPromptClose(window);
    else gtk_label_set_text(GTK_LABEL(prompt->message),
        status == UMI_STATUS_INVALID_STATE || status == UMI_STATUS_NOT_FOUND
            ? "The document changed or the line is unavailable. Cancel and check the intended document."
            : "Enter a positive line or line:column, for example 12:5.");
    adapter = g_object_get_data(lifetime, "adapter");
    g_object_unref(lifetime);
    if (adapter != NULL) (void)EditFinished(adapter, status);
}

UmiStatus UmiGtk4AdapterPromptDocumentLocation(UmiGtk4Adapter *adapter)
{
    UmiDocumentId id = 0U;
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = EditFindDocument(adapter, NULL, &id);
    if (status != UMI_STATUS_OK) return EditFinished(adapter, status);
    if (adapter->edit_lifetime == NULL || adapter->window == NULL)
        return EditFinished(adapter, UMI_STATUS_UNAVAILABLE);
    GtkWidget *existing = g_object_get_data(adapter->edit_lifetime, "location-dialog");
    if (existing != NULL) { gtk_window_present(GTK_WINDOW(existing)); return UMI_STATUS_OK; }
    LocationPrompt *prompt = g_try_new0(LocationPrompt, 1);
    if (prompt == NULL) return EditFinished(adapter, UMI_STATUS_OUT_OF_MEMORY);
    prompt->lifetime = g_object_ref(adapter->edit_lifetime);
    prompt->document = id;
    prompt->window = gtk_window_new();
    prompt->entry = gtk_entry_new();
    prompt->message = gtk_label_new("Enter line or line:column. Columns count UTF-8 bytes.");
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *go = gtk_button_new_with_label("Go to location");
    gtk_window_set_title(GTK_WINDOW(prompt->window), "Go to source location");
    gtk_window_set_transient_for(GTK_WINDOW(prompt->window), GTK_WINDOW(adapter->window));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(prompt->window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(prompt->window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(prompt->window), 440, -1);
    gtk_widget_set_margin_top(root, 18); gtk_widget_set_margin_bottom(root, 18);
    gtk_widget_set_margin_start(root, 18); gtk_widget_set_margin_end(root, 18);
    gtk_label_set_wrap(GTK_LABEL(prompt->message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(prompt->message), 0.0f);
    gtk_entry_set_placeholder_text(GTK_ENTRY(prompt->entry), "12:5");
    gtk_entry_set_max_length(GTK_ENTRY(prompt->entry), (int)UMI_DOCUMENT_LOCATION_TEXT_MAX);
    gtk_widget_set_tooltip_text(prompt->entry, "Line starts at 1; an omitted column starts at the beginning of the line.");
    gtk_box_append(GTK_BOX(root), prompt->message);
    gtk_box_append(GTK_BOX(root), prompt->entry);
    gtk_box_append(GTK_BOX(buttons), cancel); gtk_box_append(GTK_BOX(buttons), go);
    gtk_box_append(GTK_BOX(root), buttons);
    gtk_window_set_child(GTK_WINDOW(prompt->window), root);
    g_object_set_data_full(G_OBJECT(prompt->window), "umicom-location-prompt", prompt, LocationPromptFree);
    g_object_set_data(prompt->lifetime, "location-dialog", prompt->window);
    g_signal_connect(prompt->window, "destroy", G_CALLBACK(LocationPromptDestroyed), prompt);
    g_signal_connect(prompt->window, "close-request", G_CALLBACK(LocationPromptClosing), NULL);
    /* Object-bound signals disconnect when the form is finalised. A retained
     * child can survive destruction, so its callbacks also check the token. */
    g_signal_connect_object(cancel, "clicked", G_CALLBACK(LocationPromptCancel), prompt->window, 0);
    g_signal_connect_object(go, "clicked", G_CALLBACK(LocationPromptAccept), prompt->window, 0);
    g_signal_connect_object(prompt->entry, "activate", G_CALLBACK(LocationPromptAccept), prompt->window, 0);
    (void)umi_gtk4_automation_tag_widget(prompt->window, "document.location.window");
    (void)umi_gtk4_automation_tag_widget(prompt->entry, "document.location.input");
    (void)umi_gtk4_automation_tag_widget(prompt->message, "document.location.message");
    (void)umi_gtk4_automation_tag_widget(cancel, "document.location.cancel");
    (void)umi_gtk4_automation_tag_widget(go, "document.location.go");
    gtk_window_present(GTK_WINDOW(prompt->window));
    (void)gtk_widget_grab_focus(prompt->entry);
    return UMI_STATUS_OK;
}
