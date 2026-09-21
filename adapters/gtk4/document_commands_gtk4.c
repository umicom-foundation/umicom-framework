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
#include <stdio.h>

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

/* The close form shares this existing editing lifetime token. */
static void ClosePromptClose(GtkWidget *window);
static void CloseRunDetach(GObject *lifetime);

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
        /* Invalidate before cancelling a native chooser: its late reply must
         * not dereference the coordinator or a replaced Studio context. */
        GtkWidget *closeDialog = g_object_get_data(adapter->edit_lifetime, "close-dialog");
        if (closeDialog != NULL) ClosePromptClose(closeDialog);
        /* Cancel the captured sequence before its borrowed coordinator ends. */
        CloseRunDetach(adapter->edit_lifetime);
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
    /* Previous availability considered only another clipboard read:
     * case UMI_DOCUMENT_EDIT_PASTE: return !state.read_only && adapter->edit_cancel == NULL;
     * Save All and delayed Paste now exclude one another in this Framework
     * adapter. Ordinary typing and immediate Copy/Undo remain available. */
    case UMI_DOCUMENT_EDIT_PASTE: return !state.read_only && adapter->edit_cancel == NULL &&
        /* Former final predicate retained: !UmiGtk4AdapterDocumentSaveAllBusy(adapter);
         * Framework now also excludes an unanswered close question. */
        !UmiGtk4AdapterDocumentSaveAllBusy(adapter) && !UmiGtk4AdapterDocumentCloseBusy(adapter);
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
    /* Preserve the previous rule, extended here to the shared save owner:
     * if (command == UMI_DOCUMENT_EDIT_PASTE && adapter->edit_cancel != NULL)
     *     return EditFinished(adapter, UMI_STATUS_BUSY);
     * Recheck at invocation, not only when the menu was last painted. */
    if (command == UMI_DOCUMENT_EDIT_PASTE && (adapter->edit_cancel != NULL ||
        /* Previous final condition: UmiGtk4AdapterDocumentSaveAllBusy(adapter))).
         * Close questions now exclude delayed clipboard mutation too. */
        UmiGtk4AdapterDocumentSaveAllBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter)))
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

/* Keep menu and keyboard switching on the same public document contract. */
UmiStatus UmiGtk4AdapterCycleDocument(UmiGtk4Adapter *adapter, int direction)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = adapter->edit_coordinator != NULL
        ? UmiDocumentCoordinatorCycle(adapter->edit_coordinator, direction, NULL)
        : UMI_STATUS_UNAVAILABLE;
    return EditFinished(adapter, status);
}


/* A sequence owns captured IDs and at most one reviewed draft. The editing
 * token, idle source and prompt each have explicit references; no application
 * callback can keep a destroyed adapter/coordinator alive. */
typedef struct CloseRun {
    unsigned references;
    GObject *lifetime;
    UmiDocumentCloseSession *session;
    UmiGtk4DocumentCloseResultFn completed;
    void *context;
    guint idle;
    int finished;
} CloseRun;
static CloseRun *CloseRunRef(CloseRun *run) { ++run->references; return run; }
static void CloseRunUnref(gpointer data)
{
    CloseRun *run = data;
    if (--run->references != 0U) return;
    UmiDocumentCloseSessionDestroy(run->session);
    g_object_unref(run->lifetime);
    g_free(run);
}
static UmiGtk4Adapter *CloseRunOwner(CloseRun *run)
{
    if (run == NULL || run->finished ||
        g_object_get_data(run->lifetime, "close-run") != run) return NULL;
    UmiGtk4Adapter *adapter = g_object_get_data(run->lifetime, "adapter");
    return adapter != NULL && adapter->edit_coordinator != NULL ? adapter : NULL;
}
static gboolean CloseRunPump(gpointer data);
static void CloseRunSchedule(CloseRun *run)
{
    if (run->idle == 0U && CloseRunOwner(run) != NULL &&
        g_object_get_data(run->lifetime, "close-dialog") == NULL)
        run->idle = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, CloseRunPump,
            CloseRunRef(run), CloseRunUnref);
}
/* Caller holds an idle, prompt or temporary reference. Remove ownership before
 * the completion callback, which is allowed to start a different run. */
static void CloseRunFinish(CloseRun *run, UmiStatus status)
{
    UmiGtk4Adapter *adapter = CloseRunOwner(run);
    if (adapter == NULL) return;
    UmiDocumentCloseProgress progress;
    if (UmiDocumentCloseSessionProgress(run->session, &progress) != UMI_STATUS_OK) return;
    if (status != UMI_STATUS_OK) {
        progress.last_status = status;
        progress.phase = status == UMI_STATUS_CANCELLED
            ? UMI_DOCUMENT_CLOSE_CANCELLED : UMI_DOCUMENT_CLOSE_FAILED;
    }
    run->finished = 1;
    g_object_set_data(run->lifetime, "close-run", NULL);
    if (run->idle != 0U) { guint idle=run->idle; run->idle=0U; (void)g_source_remove(idle); }
    if (adapter->shell != NULL && adapter->window != NULL) {
        UmiStatus refresh = umi_gtk4_refresh_documents(adapter,
            umi_ui_application_shell_workbench(adapter->shell));
        if (progress.last_status == UMI_STATUS_OK && refresh != UMI_STATUS_OK) {
            progress.last_status = refresh;
            progress.phase = UMI_DOCUMENT_CLOSE_FAILED;
        }
    }
    if (g_object_get_data(run->lifetime, "adapter") == adapter && run->completed != NULL)
        run->completed(run->context, &progress);
    CloseRunUnref(run); /* Release the editing token's run ownership. */
}
static void CloseRunDetach(GObject *lifetime)
{
    if (lifetime == NULL) return;
    CloseRun *run = g_object_get_data(lifetime, "close-run");
    if (run == NULL) return;
    run->finished = 1;
    g_object_set_data(lifetime, "close-run", NULL);
    (void)UmiDocumentCloseSessionCancel(run->session);
    if (run->idle != 0U) { guint idle=run->idle; run->idle=0U; (void)g_source_remove(idle); }
    CloseRunUnref(run);
}

/* The window owns this question; a pending native filename reply retains the
 * window. The question owns only its capture and token, never the coordinator. */
typedef struct ClosePrompt {
    GObject *lifetime;
    UmiDocumentClosePlan *plan;
    CloseRun *run; /* Retained only for a group question; plan then belongs to its session. */
    GCancellable *cancel;
    GtkWidget *message;
    int choosing;
    int closing;
} ClosePrompt;

static UmiStatus ClosePromptSummary(ClosePrompt *prompt, UmiDocumentCloseSummary *summary)
{
    if (prompt->run == NULL) return UmiDocumentClosePlanSummary(prompt->plan, summary);
    UmiDocumentCloseProgress progress;
    UmiStatus status = UmiDocumentCloseSessionProgress(prompt->run->session, &progress);
    if (status == UMI_STATUS_OK) *summary = progress.current;
    return status;
}
static UmiStatus ClosePromptCheck(UmiGtk4Adapter *adapter, ClosePrompt *prompt)
{
    return prompt->run != NULL ? UmiDocumentCloseSessionCheck(prompt->run->session) :
        UmiDocumentCoordinatorCheckClose(adapter->edit_coordinator, prompt->plan);
}
static UmiStatus ClosePromptRespond(UmiGtk4Adapter *adapter, ClosePrompt *prompt,
    UmiDocumentCloseDecision decision, const char *path)
{
    return prompt->run != NULL ? UmiDocumentCloseSessionRespond(prompt->run->session, decision, path) :
        UmiDocumentCoordinatorApplyClose(adapter->edit_coordinator, prompt->plan, decision, path);
}

static void ClosePromptFree(gpointer data)
{
    ClosePrompt *prompt = data;
    UmiDocumentClosePlanDestroy(prompt->plan);
    if (prompt->run != NULL) CloseRunUnref(prompt->run);
    g_clear_object(&prompt->cancel);
    g_clear_object(&prompt->lifetime);
    g_free(prompt);
}

static UmiGtk4Adapter *ClosePromptOwner(GtkWidget *window, ClosePrompt *prompt)
{
    if (prompt == NULL || prompt->closing ||
        g_object_get_data(prompt->lifetime, "close-dialog") != window) return NULL;
    UmiGtk4Adapter *adapter = g_object_get_data(prompt->lifetime, "adapter");
    return adapter != NULL && adapter->edit_coordinator != NULL ? adapter : NULL;
}

static void ClosePromptClose(GtkWidget *window)
{
    /* Cancelling can invoke arbitrary observers. Hold the object until every
     * field needed here has been used, and clear the lookup before cancellation. */
    g_object_ref(window);
    ClosePrompt *prompt = g_object_get_data(G_OBJECT(window), "umicom-close-prompt");
    if (prompt != NULL) {
        prompt->closing = 1;
        if (g_object_get_data(prompt->lifetime, "close-dialog") == window)
            g_object_set_data(prompt->lifetime, "close-dialog", NULL);
        g_cancellable_cancel(prompt->cancel);
    }
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
}

/* Previous single-plan completion is retained for migration reference.
 * CloseRunFinish now supplies a final group summary; single closes still use
 * EditFinished. No save, discard or document store has moved into Studio. */
// static void ClosePromptFinish(GtkWidget *window, UmiStatus status)
// {
//     ClosePrompt *prompt = g_object_get_data(G_OBJECT(window), "umicom-close-prompt");
//     if (prompt == NULL) return;
//     GObject *lifetime = g_object_ref(prompt->lifetime);
//     ClosePromptClose(window);
//     UmiGtk4Adapter *adapter = g_object_get_data(lifetime, "adapter");
//     if (adapter != NULL) (void)EditFinished(adapter, status);
//     g_object_unref(lifetime);
// }
static void ClosePromptFinish(GtkWidget *window, UmiStatus status)
{
    ClosePrompt *prompt = g_object_get_data(G_OBJECT(window), "umicom-close-prompt");
    if (prompt == NULL || prompt->closing) return;
    GObject *lifetime = g_object_ref(prompt->lifetime);
    CloseRun *run = prompt->run != NULL ? CloseRunRef(prompt->run) : NULL;
    ClosePromptClose(window);
    UmiGtk4Adapter *adapter = g_object_get_data(lifetime, "adapter");
    if (run != NULL) {
        if (status != UMI_STATUS_OK) {
            if (status == UMI_STATUS_CANCELLED) (void)UmiDocumentCloseSessionCancel(run->session);
            CloseRunFinish(run, status);
        } else if (CloseRunOwner(run) != NULL) {
            UmiStatus refresh = umi_gtk4_refresh_documents(adapter,
                umi_ui_application_shell_workbench(adapter->shell));
            if (refresh != UMI_STATUS_OK) CloseRunFinish(run, refresh);
            else CloseRunSchedule(run);
        }
        CloseRunUnref(run);
    } else if (adapter != NULL) (void)EditFinished(adapter, status);
    g_object_unref(lifetime);
}

static gboolean ClosePromptClosing(GtkWindow *window, gpointer data)
{
    (void)data;
    ClosePrompt *prompt = g_object_get_data(G_OBJECT(window), "umicom-close-prompt");
    if (ClosePromptOwner(GTK_WIDGET(window), prompt) != NULL)
        ClosePromptFinish(GTK_WIDGET(window), UMI_STATUS_CANCELLED);
    else ClosePromptClose(GTK_WIDGET(window));
    return TRUE;
}

static void ClosePromptDestroyed(GtkWidget *window, gpointer data)
{
    (void)data;
    ClosePrompt *prompt = g_object_get_data(G_OBJECT(window), "umicom-close-prompt");
    if (prompt != NULL) {
        int unexpected = !prompt->closing;
        CloseRun *run = prompt->run != NULL ? CloseRunRef(prompt->run) : NULL;
        prompt->closing = 1;
        if (g_object_get_data(prompt->lifetime, "close-dialog") == window)
            g_object_set_data(prompt->lifetime, "close-dialog", NULL);
        g_cancellable_cancel(prompt->cancel);
        if (unexpected && run != NULL) {
            (void)UmiDocumentCloseSessionCancel(run->session);
            CloseRunFinish(run, UMI_STATUS_CANCELLED);
        }
        if (run != NULL) CloseRunUnref(run);
    }
}

static void ClosePromptCancel(GtkButton *button, gpointer data)
{
    GtkWidget *window = data;
    ClosePrompt *prompt = g_object_get_data(G_OBJECT(window), "umicom-close-prompt");
    if (gtk_widget_get_root(GTK_WIDGET(button)) != GTK_ROOT(window) ||
        ClosePromptOwner(window, prompt) == NULL) return;
    ClosePromptFinish(window, UMI_STATUS_CANCELLED);
}

/* A chooser response is always finished and freed, even after the binding was
 * removed. Its saved window reference does not make its document owner live. */
static void ClosePathChosen(GObject *source, GAsyncResult *result, gpointer data)
{
    GtkWidget *window = data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, &error);
    char *path = file != NULL ? g_file_get_path(file) : NULL;
    ClosePrompt *prompt = g_object_get_data(G_OBJECT(window), "umicom-close-prompt");
    UmiGtk4Adapter *adapter = ClosePromptOwner(window, prompt);
    if (adapter != NULL) {
        prompt->choosing = 0;
        UmiStatus status;
        if (g_cancellable_is_cancelled(prompt->cancel)) status = UMI_STATUS_CANCELLED;
        else if (path == NULL) {
            int dismissed = error != NULL &&
                (g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) ||
                 g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED) ||
                 g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED));
            status = dismissed ? UMI_STATUS_CANCELLED : UMI_STATUS_UNAVAILABLE;
        } else if (UmiGtk4AdapterDocumentSaveAllBusy(adapter) || adapter->edit_cancel != NULL)
            status = UMI_STATUS_BUSY;
        /* The same service is now selected through ClosePromptRespond in this
         * Framework file. Retain the previous single-plan call for reference. */
        // else status = UmiDocumentCoordinatorApplyClose(adapter->edit_coordinator,
        //     prompt->plan, UMI_DOCUMENT_CLOSE_SAVE, path);
        else status = ClosePromptRespond(adapter, prompt, UMI_DOCUMENT_CLOSE_SAVE, path);
        /* A failed write leaves the document available for a fresh question. */
        ClosePromptFinish(window, status);
    }
    g_free(path);
    g_clear_object(&file);
    g_clear_error(&error);
    g_object_unref(window);
}

static void ClosePromptApply(GtkButton *button, gpointer data)
{
    GtkWidget *window = data;
    ClosePrompt *prompt = g_object_get_data(G_OBJECT(window), "umicom-close-prompt");
    UmiGtk4Adapter *adapter = ClosePromptOwner(window, prompt);
    if (adapter == NULL || prompt->choosing ||
        gtk_widget_get_root(GTK_WIDGET(button)) != GTK_ROOT(window)) return;
    UmiDocumentCloseDecision decision = (UmiDocumentCloseDecision)GPOINTER_TO_INT(
        g_object_get_data(G_OBJECT(button), "umicom-close-decision"));
    UmiDocumentCloseSummary summary;
    /* ClosePromptSummary/ClosePromptCheck in this Framework file keep the
     * single-plan route and also handle the session-owned plan. Previous code: */
    // UmiStatus status = UmiDocumentClosePlanSummary(prompt->plan, &summary);
    // if (status == UMI_STATUS_OK) status = UmiDocumentCoordinatorCheckClose(
    //     adapter->edit_coordinator, prompt->plan);
    UmiStatus status = ClosePromptSummary(prompt, &summary);
    if (status == UMI_STATUS_OK) status = ClosePromptCheck(adapter, prompt);
    if (status == UMI_STATUS_OK &&
        (UmiGtk4AdapterDocumentSaveAllBusy(adapter) || adapter->edit_cancel != NULL))
        status = UMI_STATUS_BUSY;
    if (status != UMI_STATUS_OK) { ClosePromptFinish(window, status); return; }
    if (decision == UMI_DOCUMENT_CLOSE_SAVE && !summary.has_path) {
        if (prompt->run != NULL) {
            status = ClosePromptRespond(adapter, prompt, decision, NULL);
            if (status != UMI_STATUS_OK) { ClosePromptFinish(window, status); return; }
        }
        GtkFileDialog *dialog = gtk_file_dialog_new();
        gtk_file_dialog_set_title(dialog, "Save document before closing");
        gtk_file_dialog_set_initial_name(dialog, summary.display_name);
        gtk_file_dialog_set_modal(dialog, TRUE);
        prompt->choosing = 1;
        gtk_file_dialog_save(dialog, GTK_WINDOW(window), prompt->cancel,
            ClosePathChosen, g_object_ref(window));
        g_object_unref(dialog);
        return;
    }
    /* ClosePromptRespond in this Framework file keeps the same owner and
     * checks for both a single close and the current group target. Previous code: */
    // status = UmiDocumentCoordinatorApplyClose(adapter->edit_coordinator,
    //     prompt->plan, decision, NULL);
    status = ClosePromptRespond(adapter, prompt, decision, NULL);
    ClosePromptFinish(window, status);
}

/* REFACTOR: The former single-document entry point is retained below.
 * Its controls now live in ClosePromptShow in this same Framework file, shared
 * by File Close and Close All/Close Others. The new wrapper preserves the
 * public name, reviewed-plan owner and completion route. Do not enable both. */
// UmiStatus UmiGtk4AdapterRequestDocumentClose(UmiGtk4Adapter *adapter,
//     const char *viewId)
// {
//     if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
//     if (adapter->edit_lifetime == NULL || adapter->edit_coordinator == NULL ||
//         adapter->window == NULL || adapter->shell == NULL) return UMI_STATUS_UNAVAILABLE;
//     if (UmiGtk4AdapterDocumentSaveAllBusy(adapter) || adapter->edit_cancel != NULL)
//         return EditFinished(adapter, UMI_STATUS_BUSY);
//     if (g_object_get_data(adapter->edit_lifetime, "close-dialog") != NULL)
//         return EditFinished(adapter, UMI_STATUS_BUSY);
//     UmiDocumentId id = 0U;
//     UmiDocumentClosePlan *plan = NULL;
//     UmiDocumentCloseSummary summary;
//     UmiStatus status = EditFindDocument(adapter, viewId, &id);
//     if (status == UMI_STATUS_OK)
//         status = UmiDocumentCoordinatorPrepareClose(adapter->edit_coordinator, id, &plan);
//     if (status == UMI_STATUS_OK) status = UmiDocumentClosePlanSummary(plan, &summary);
//     if (status != UMI_STATUS_OK) { UmiDocumentClosePlanDestroy(plan); return EditFinished(adapter, status); }
//     if (!summary.dirty) {
//         status = UmiDocumentCoordinatorApplyClose(adapter->edit_coordinator,
//             plan, UMI_DOCUMENT_CLOSE_UNMODIFIED, NULL);
//         UmiDocumentClosePlanDestroy(plan);
//         return EditFinished(adapter, status);
//     }
//     ClosePrompt *prompt = g_try_new0(ClosePrompt, 1);
//     if (prompt == NULL) { UmiDocumentClosePlanDestroy(plan); return EditFinished(adapter, UMI_STATUS_OUT_OF_MEMORY); }
//     prompt->plan = plan;
//     prompt->lifetime = g_object_ref(adapter->edit_lifetime);
//     prompt->cancel = g_cancellable_new();
//     GtkWidget *window = gtk_window_new();
//     g_object_set_data_full(G_OBJECT(window), "umicom-close-prompt", prompt, ClosePromptFree);
//     g_object_set_data(prompt->lifetime, "close-dialog", window);
//     gtk_window_set_title(GTK_WINDOW(window), "Close source document");
//     gtk_window_set_transient_for(GTK_WINDOW(window), adapter->window);
//     gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
//     gtk_window_set_modal(GTK_WINDOW(window), TRUE);
//     gtk_window_set_default_size(GTK_WINDOW(window), 520, -1);
//     GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
//     gtk_widget_set_margin_top(box, 18); gtk_widget_set_margin_bottom(box, 18);
//     gtk_widget_set_margin_start(box, 18); gtk_widget_set_margin_end(box, 18);
//     gtk_window_set_child(GTK_WINDOW(window), box);
//     char message[UMI_DOCUMENT_NAME_CAPACITY + 320U];
//     (void)snprintf(message, sizeof(message),
//         "%s has unsaved work.\nSave and Close writes the draft first. Discard and Close removes the open draft without changing the saved file. Cancel keeps it open.",
//         summary.display_name);
//     prompt->message = gtk_label_new(message);
//     gtk_label_set_wrap(GTK_LABEL(prompt->message), TRUE);
//     gtk_label_set_xalign(GTK_LABEL(prompt->message), 0.0F);
//     gtk_box_append(GTK_BOX(box), prompt->message);
//     GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
//     GtkWidget *cancel = gtk_button_new_with_label("Cancel");
//     GtkWidget *discard = gtk_button_new_with_label("Discard and Close");
//     GtkWidget *save = gtk_button_new_with_label("Save and Close");
//     gtk_box_append(GTK_BOX(buttons), cancel); gtk_box_append(GTK_BOX(buttons), discard);
//     gtk_box_append(GTK_BOX(buttons), save); gtk_box_append(GTK_BOX(box), buttons);
//     gtk_window_set_default_widget(GTK_WINDOW(window), cancel);
//     gtk_widget_set_sensitive(save, !summary.read_only);
//     if (summary.read_only) gtk_widget_set_tooltip_text(save,
//         "This document is read-only. Cancel to preserve it or explicitly discard its open draft.");
//     g_object_set_data(G_OBJECT(discard), "umicom-close-decision", GINT_TO_POINTER(UMI_DOCUMENT_CLOSE_DISCARD));
//     g_object_set_data(G_OBJECT(save), "umicom-close-decision", GINT_TO_POINTER(UMI_DOCUMENT_CLOSE_SAVE));
//     (void)umi_gtk4_automation_tag_widget(window, "document.close.dialog");
//     (void)umi_gtk4_automation_tag_widget(prompt->message, "document.close.message");
//     (void)umi_gtk4_automation_tag_widget(cancel, "document.close.cancel");
//     (void)umi_gtk4_automation_tag_widget(discard, "document.close.discard");
//     (void)umi_gtk4_automation_tag_widget(save, "document.close.save");
//     g_signal_connect_object(cancel, "clicked", G_CALLBACK(ClosePromptCancel), G_OBJECT(window), 0);
//     g_signal_connect_object(discard, "clicked", G_CALLBACK(ClosePromptApply), G_OBJECT(window), 0);
//     g_signal_connect_object(save, "clicked", G_CALLBACK(ClosePromptApply), G_OBJECT(window), 0);
//     g_signal_connect(window, "close-request", G_CALLBACK(ClosePromptClosing), NULL);
//     g_signal_connect(window, "destroy", G_CALLBACK(ClosePromptDestroyed), NULL);
//     gtk_window_set_focus(GTK_WINDOW(window), cancel);
//     gtk_window_present(GTK_WINDOW(window));
//     return UMI_STATUS_OK;
// }

static UmiStatus ClosePromptShow(UmiGtk4Adapter *adapter,
    UmiDocumentClosePlan *plan, CloseRun *run, UmiDocumentCloseSummary summary)
{
    ClosePrompt *prompt = g_try_new0(ClosePrompt, 1);
    if (prompt == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    prompt->plan = plan;
    prompt->run = run != NULL ? CloseRunRef(run) : NULL;
    prompt->lifetime = g_object_ref(adapter->edit_lifetime);
    prompt->cancel = g_cancellable_new();
    GtkWidget *window = gtk_window_new();
    g_object_set_data_full(G_OBJECT(window), "umicom-close-prompt", prompt, ClosePromptFree);
    g_object_set_data(prompt->lifetime, "close-dialog", window);
    gtk_window_set_title(GTK_WINDOW(window), "Close source document");
    gtk_window_set_transient_for(GTK_WINDOW(window), adapter->window);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window), 520, -1);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    gtk_widget_set_margin_top(box, 18); gtk_widget_set_margin_bottom(box, 18);
    gtk_widget_set_margin_start(box, 18); gtk_widget_set_margin_end(box, 18);
    gtk_window_set_child(GTK_WINDOW(window), box);
    char message[UMI_DOCUMENT_NAME_CAPACITY + 320U];
    (void)snprintf(message, sizeof(message),
        "%s has unsaved work.\nSave and Close writes the draft first. Discard and Close removes the open draft without changing the saved file. Cancel keeps it open.",
        summary.display_name);
    if (run != NULL) {
        UmiDocumentCloseProgress progress;
        char progressText[UMI_DOCUMENT_NAME_CAPACITY + 384U];
        if (UmiDocumentCloseSessionProgress(run->session, &progress) == UMI_STATUS_OK &&
            UmiDocumentCloseProgressFormat(&progress, progressText, sizeof(progressText)) == UMI_STATUS_OK) {
            GtkWidget *progressLabel = gtk_label_new(progressText);
            gtk_label_set_wrap(GTK_LABEL(progressLabel), TRUE);
            gtk_label_set_xalign(GTK_LABEL(progressLabel), 0.0F);
            (void)umi_gtk4_automation_tag_widget(progressLabel, "document.close.progress");
            gtk_box_append(GTK_BOX(box), progressLabel);
        }
        (void)snprintf(message, sizeof(message),
            "%s has unsaved work.\nSave and Close writes this draft first. Discard and Close removes only this open draft. Cancel stops the sequence and keeps all remaining documents open.",
            summary.display_name);
    }
    prompt->message = gtk_label_new(message);
    gtk_label_set_wrap(GTK_LABEL(prompt->message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(prompt->message), 0.0F);
    gtk_box_append(GTK_BOX(box), prompt->message);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *cancel = gtk_button_new_with_label("Cancel");
    GtkWidget *discard = gtk_button_new_with_label("Discard and Close");
    GtkWidget *save = gtk_button_new_with_label("Save and Close");
    gtk_box_append(GTK_BOX(buttons), cancel); gtk_box_append(GTK_BOX(buttons), discard);
    gtk_box_append(GTK_BOX(buttons), save); gtk_box_append(GTK_BOX(box), buttons);
    gtk_window_set_default_widget(GTK_WINDOW(window), cancel);
    gtk_widget_set_sensitive(save, !summary.read_only);
    if (summary.read_only) gtk_widget_set_tooltip_text(save,
        "This document is read-only. Cancel to preserve it or explicitly discard its open draft.");
    g_object_set_data(G_OBJECT(discard), "umicom-close-decision", GINT_TO_POINTER(UMI_DOCUMENT_CLOSE_DISCARD));
    g_object_set_data(G_OBJECT(save), "umicom-close-decision", GINT_TO_POINTER(UMI_DOCUMENT_CLOSE_SAVE));
    (void)umi_gtk4_automation_tag_widget(window, "document.close.dialog");
    (void)umi_gtk4_automation_tag_widget(prompt->message, "document.close.message");
    (void)umi_gtk4_automation_tag_widget(cancel, "document.close.cancel");
    (void)umi_gtk4_automation_tag_widget(discard, "document.close.discard");
    (void)umi_gtk4_automation_tag_widget(save, "document.close.save");
    g_signal_connect_object(cancel, "clicked", G_CALLBACK(ClosePromptCancel), G_OBJECT(window), 0);
    g_signal_connect_object(discard, "clicked", G_CALLBACK(ClosePromptApply), G_OBJECT(window), 0);
    g_signal_connect_object(save, "clicked", G_CALLBACK(ClosePromptApply), G_OBJECT(window), 0);
    g_signal_connect(window, "close-request", G_CALLBACK(ClosePromptClosing), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(ClosePromptDestroyed), NULL);
    /* A native surface may end while a chooser/test still retains the window.
     * Invalidate now, rather than waiting for final object disposal. */
    g_signal_connect(window, "unrealize", G_CALLBACK(ClosePromptDestroyed), NULL);
    gtk_window_set_focus(GTK_WINDOW(window), cancel);
    gtk_window_present(GTK_WINDOW(window));
    return UMI_STATUS_OK;
}

UmiStatus UmiGtk4AdapterRequestDocumentClose(UmiGtk4Adapter *adapter,
    const char *viewId)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_lifetime == NULL || adapter->edit_coordinator == NULL ||
        adapter->window == NULL || adapter->shell == NULL) return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4AdapterDocumentSaveAllBusy(adapter) || adapter->edit_cancel != NULL ||
        UmiGtk4AdapterDocumentCloseBusy(adapter)) return EditFinished(adapter, UMI_STATUS_BUSY);
    UmiDocumentId id = 0U;
    UmiDocumentClosePlan *plan = NULL;
    UmiDocumentCloseSummary summary;
    UmiStatus status = EditFindDocument(adapter, viewId, &id);
    if (status == UMI_STATUS_OK) status = UmiDocumentCoordinatorPrepareClose(adapter->edit_coordinator, id, &plan);
    if (status == UMI_STATUS_OK) status = UmiDocumentClosePlanSummary(plan, &summary);
    if (status != UMI_STATUS_OK) { UmiDocumentClosePlanDestroy(plan); return EditFinished(adapter, status); }
    if (!summary.dirty) {
        status = UmiDocumentCoordinatorApplyClose(adapter->edit_coordinator, plan, UMI_DOCUMENT_CLOSE_UNMODIFIED, NULL);
        UmiDocumentClosePlanDestroy(plan);
        return EditFinished(adapter, status);
    }
    status = ClosePromptShow(adapter, plan, NULL, summary);
    if (status != UMI_STATUS_OK) { UmiDocumentClosePlanDestroy(plan); return EditFinished(adapter, status); }
    return UMI_STATUS_OK;
}

static gboolean CloseRunPump(gpointer data)
{
    CloseRun *run = data;
    run->idle = 0U;
    UmiGtk4Adapter *adapter = CloseRunOwner(run);
    if (adapter == NULL) return G_SOURCE_REMOVE;
    UmiStatus status = UmiDocumentCloseSessionStep(run->session);
    adapter = CloseRunOwner(run);
    if (adapter == NULL) return G_SOURCE_REMOVE;
    UmiDocumentCloseProgress progress;
    UmiStatus observed = UmiDocumentCloseSessionProgress(run->session, &progress);
    if (observed != UMI_STATUS_OK) { CloseRunFinish(run, observed); return G_SOURCE_REMOVE; }
    if (status != UMI_STATUS_OK) { CloseRunFinish(run, status); return G_SOURCE_REMOVE; }
    if (progress.phase == UMI_DOCUMENT_CLOSE_DECISION || progress.phase == UMI_DOCUMENT_CLOSE_PATH) {
        status = ClosePromptShow(adapter, NULL, run, progress.current);
        if (status != UMI_STATUS_OK) CloseRunFinish(run, status);
    } else if (progress.phase == UMI_DOCUMENT_CLOSE_READY) {
        status = umi_gtk4_refresh_documents(adapter,
            umi_ui_application_shell_workbench(adapter->shell));
        if (status != UMI_STATUS_OK) CloseRunFinish(run, status);
        else CloseRunSchedule(run);
    } else CloseRunFinish(run, progress.last_status);
    return G_SOURCE_REMOVE;
}

int UmiGtk4AdapterCloseDocumentsBusy(const UmiGtk4Adapter *adapter)
{
    return adapter != NULL && adapter->edit_lifetime != NULL &&
        g_object_get_data(adapter->edit_lifetime, "close-run") != NULL;
}

int UmiGtk4AdapterDocumentCloseBusy(const UmiGtk4Adapter *adapter)
{
    return UmiGtk4AdapterCloseDocumentsBusy(adapter) ||
        (adapter != NULL && adapter->edit_lifetime != NULL &&
         g_object_get_data(adapter->edit_lifetime, "close-dialog") != NULL);
}

UmiStatus UmiGtk4AdapterCloseDocuments(UmiGtk4Adapter *adapter,
    UmiDocumentCloseScope scope, UmiGtk4DocumentCloseResultFn completed, void *context)
{
    if (adapter == NULL || (scope != UMI_DOCUMENT_CLOSE_ALL && scope != UMI_DOCUMENT_CLOSE_OTHERS))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_lifetime == NULL || adapter->edit_coordinator == NULL ||
        adapter->window == NULL || adapter->shell == NULL) return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4AdapterDocumentCloseBusy(adapter) || UmiGtk4AdapterDocumentSaveAllBusy(adapter) ||
        adapter->edit_cancel != NULL) return UMI_STATUS_BUSY;
    UmiDocumentId keep = 0U;
    UmiStatus status;
    if (scope == UMI_DOCUMENT_CLOSE_OTHERS) {
        status = EditFindDocument(adapter, NULL, &keep);
        if (status != UMI_STATUS_OK) return status;
    }
    CloseRun *run = g_try_new0(CloseRun, 1);
    if (run == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiDocumentCloseSessionCreate(adapter->edit_coordinator, scope, keep, &run->session);
    if (status != UMI_STATUS_OK) { g_free(run); return status; }
    run->references = 1U;
    run->lifetime = g_object_ref(adapter->edit_lifetime);
    run->completed = completed;
    run->context = context;
    g_object_set_data(run->lifetime, "close-run", run);
    CloseRunSchedule(run);
    return UMI_STATUS_OK;
}

UmiStatus UmiGtk4AdapterCancelCloseDocuments(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiGtk4AdapterCloseDocumentsBusy(adapter)) return UMI_STATUS_OK;
    CloseRun *run = CloseRunRef(g_object_get_data(adapter->edit_lifetime, "close-run"));
    UmiStatus status = UmiDocumentCloseSessionCancel(run->session);
    if (status == UMI_STATUS_OK) {
        GtkWidget *window = g_object_get_data(run->lifetime, "close-dialog");
        if (window != NULL) ClosePromptFinish(window, UMI_STATUS_CANCELLED);
        else CloseRunSchedule(run);
    }
    CloseRunUnref(run);
    return status;
}

UmiStatus UmiGtk4AdapterCloseDocumentsProgress(const UmiGtk4Adapter *adapter,
    UmiDocumentCloseProgress *outProgress)
{
    if (adapter == NULL || outProgress == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiGtk4AdapterCloseDocumentsBusy(adapter)) return UMI_STATUS_NOT_FOUND;
    CloseRun *run = g_object_get_data(adapter->edit_lifetime, "close-run");
    if (CloseRunOwner(run) != adapter) return UMI_STATUS_INVALID_STATE;
    /* Former direct publication retained for reference:
     * return UmiDocumentCloseSessionProgress(run->session, outProgress);
     * The shared validator in src/document/close_session.c now checks a private
     * snapshot first. Failed observations leave caller-owned storage intact. */
    UmiDocumentCloseProgress progress;
    UmiStatus status = UmiDocumentCloseSessionProgress(run->session, &progress);
    if (status == UMI_STATUS_OK) status = UmiDocumentCloseProgressValidate(&progress);
    if (status == UMI_STATUS_OK) *outProgress = progress;
    return status;
}
