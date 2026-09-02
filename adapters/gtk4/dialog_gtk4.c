/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/dialog_gtk4.c
 *
 * PURPOSE:
 *   Present the next pending Framework dialog request as a small transient GTK4
 *   window without exposing the native window through the public dialog contract.
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

typedef struct DialogContext {
    UmiUiDialogService *service;
    UmiUiDialogRequest request;
    GtkWindow *window;
} DialogContext;

/* Provide the on dialog button operation used by this module and its client applications. */
static void on_dialog_button(GtkButton *button, gpointer user_data)
{
    DialogContext *context = (DialogContext *)user_data;
    UmiUiDialogResult result;
    const char *index_text = (const char *)g_object_get_data(G_OBJECT(button), "umicom-button-index");
    (void)memset(&result, 0, sizeof(result));
    result.request_id = context->request.request_id;
    result.accepted = 1;
    result.button_index = index_text != NULL ? (int32_t)g_ascii_strtoll(index_text, NULL, 10) : 0;
    (void)umi_ui_dialog_complete(context->service, &result);
    gtk_window_destroy(context->window);
    g_free(context);
}

/*
 * Provide the gtk4 process dialog operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_process_dialog(UmiGtk4Adapter *adapter,
                                  UmiUiWorkbench *workbench)
{
    UmiUiDialogService *service;
    UmiUiDialogRequest request;
    UmiStatus status;
    DialogContext *context;
    GtkWidget *box;
    GtkWidget *label;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    service = umi_ui_workbench_dialogs(workbench);
    status = umi_ui_dialog_next(service, &request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    context = g_new0(DialogContext, 1);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    context->service = service;
    context->request = request;
    context->window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_title(context->window, request.title);
    gtk_window_set_transient_for(context->window, adapter->window);
    gtk_window_set_modal(context->window, TRUE);
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    label = gtk_label_new(request.message);
    gtk_box_append(GTK_BOX(box), label);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request.button_count; ++index) {
        GtkWidget *button = gtk_button_new_with_label(request.buttons[index]);
        char *index_text = g_strdup_printf("%zu", index);
        g_object_set_data_full(G_OBJECT(button), "umicom-button-index", index_text, g_free);
        g_signal_connect(button, "clicked", G_CALLBACK(on_dialog_button), context);
        gtk_box_append(GTK_BOX(box), button);
    }
    gtk_window_set_child(context->window, box);
    gtk_window_present(context->window);
    return UMI_STATUS_OK;
}
