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
static UmiStatus EditFinished(UmiGtk4Adapter *adapter, UmiStatus status)
{
    UmiGtk4DocumentEditResultFn callback = adapter->edit_completed;
    void *context = adapter->edit_context;
    if (adapter->shell != NULL && adapter->window != NULL) {
        UmiStatus refresh = umi_gtk4_refresh_documents(adapter,
            umi_ui_application_shell_workbench(adapter->shell));
        if (status == UMI_STATUS_OK) status = refresh;
    }
    if (callback != NULL) callback(context, status);
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
    if (adapter->edit_lifetime != NULL)
        g_object_set_data(adapter->edit_lifetime, "adapter", NULL);
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
    UmiStatus status = UmiDocumentEditCommandFromId(commandId, &command);
    if (status != UMI_STATUS_OK) return status;
    status = EditFindDocument(adapter, viewId, &id);
    if (status != UMI_STATUS_OK) return status;
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
