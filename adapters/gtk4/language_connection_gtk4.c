/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/language_connection_gtk4.c
 * PURPOSE: Keep language connection work off the GTK thread and discard stale results after input or owner changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/language_connection.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/language_runtime/server_probe.h"
#include "umicom/base/text.h"
#include "umicom/platform/path.h"
#include <string.h>

typedef struct LanguageCheckRequest
{
    GWeakRef panel;
    UmiCancellationToken *cancel;
    UmiLanguageServerProfile profile;
    char directory[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY], root[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];
    UmiLanguageRuntimeProbeResult report;
    UmiStatus status;
    int stale; /* Read and written only on the GTK thread. */
} LanguageCheckRequest;
typedef struct LanguageCheckPanel
{
    GtkWidget *panel, *program, *arguments, *directory, *check, *cancel, *message;
    GPtrArray *controls;
    int needsCheck;
    LanguageCheckRequest *pending; /* Borrowed until main-context completion. */
} LanguageCheckPanel;

static LanguageCheckPanel *LanguagePanel(GtkWidget *panel)
{
    return g_object_get_data(G_OBJECT(panel), "umicom-language-connection");
}
static int LanguagePanelLive(LanguageCheckPanel *panel)
{
    return panel != NULL && gtk_widget_get_root(panel->panel) != NULL && gtk_widget_get_mapped(panel->panel);
}
static void LanguageRequestFree(LanguageCheckRequest *request)
{
    g_weak_ref_clear(&request->panel);
    umi_cancellation_token_destroy(request->cancel);
    g_free(request);
}
static void LanguagePanelFree(gpointer data)
{
    LanguageCheckPanel *panel = data;
    if (panel->pending != NULL)
        umi_cancellation_token_request(panel->pending->cancel);
    g_ptr_array_unref(panel->controls);
    g_free(panel);
}
static void LanguagePanelUnmapped(GtkWidget *widget, gpointer data)
{
    (void)data;
    LanguageCheckPanel *panel = LanguagePanel(widget);
    if (panel != NULL && panel->pending != NULL)
    {
        panel->needsCheck = 1;
        panel->pending->stale = 1;
        umi_cancellation_token_request(panel->pending->cancel);
    }
}
/* A check may finish while its page is hidden. Restore controls when the
 * page is shown again; a cancelled hidden result must never leave Check stuck
 * disabled or display the earlier "checking" message indefinitely. */
static void LanguagePanelMapped(GtkWidget *widget, gpointer data)
{
    (void)data;
    g_object_ref(widget);
    LanguageCheckPanel *panel = LanguagePanel(widget);
    if (LanguagePanelLive(panel) && panel->pending == NULL)
    {
        if (panel->needsCheck)
        {
            panel->needsCheck = 0;
            gtk_label_set_text(GTK_LABEL(panel->message),
                               "The previous check was discarded. Check the current values again.");
        }
        if (LanguagePanelLive(panel))
            gtk_widget_set_sensitive(panel->cancel, FALSE);
        if (LanguagePanelLive(panel))
            gtk_widget_set_sensitive(panel->check, TRUE);
    }
    g_object_unref(widget);
}
static void LanguageDraftChanged(GtkEditable *editable, gpointer data)
{
    (void)editable;
    LanguageCheckPanel *panel = LanguagePanel(GTK_WIDGET(data));
    if (panel != NULL && panel->pending != NULL)
    {
        panel->needsCheck = 1;
        panel->pending->stale = 1;
        umi_cancellation_token_request(panel->pending->cancel);
    }
}
static void LanguageCheckWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    LanguageCheckRequest *request = data;
    /* All configuration was copied before dispatch. No widget, application
     * service or mutable draft is accessed from this worker. */
    request->status = UmiLanguageRuntimeProbe(&request->profile, request->root, request->directory, 5000U,
                                              request->cancel, &request->report);
    g_task_return_boolean(task, TRUE);
}
static void LanguageCheckComplete(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    (void)data;
    GTask *task = G_TASK(result);
    (void)g_task_propagate_boolean(task, NULL);
    LanguageCheckRequest *request = g_task_get_task_data(task);
    g_task_set_task_data(task, NULL, NULL);
    GtkWidget *widget = g_weak_ref_get(&request->panel);
    LanguageCheckPanel *panel = widget == NULL ? NULL : LanguagePanel(widget);
    if (panel != NULL && panel->pending == request)
    {
        panel->pending = NULL;
        g_object_set_data(G_OBJECT(panel->panel), "umicom-language-check-pending", NULL);
        if (LanguagePanelLive(panel))
        {
            panel->needsCheck = 0;
            char *text;
            if (request->stale)
                text = g_strdup("Inputs or panel visibility changed. Check the current values again.");
            else if (umi_cancellation_token_is_requested(request->cancel))
                text = g_strdup_printf("Connection check cancelled. Child cleanup: %s.",
                                       umi_status_text(request->report.shutdownStatus));
            else if (request->status == UMI_STATUS_OK)
                text = g_strdup_printf(
                    "Handshake succeeded and the server was closed.\n"
                    "Completion: %s; hover: %s; definitions: %s; references: %s; formatting: %s.\n"
                    "This checks the connection; it does not activate editor completion.",
                    request->report.capabilities.completion ? "yes" : "no",
                    request->report.capabilities.hover ? "yes" : "no",
                    request->report.capabilities.definition ? "yes" : "no",
                    request->report.capabilities.references ? "yes" : "no",
                    request->report.capabilities.formatting ? "yes" : "no");
            else
                text = g_strdup_printf("Connection check: %s. Initialization: %s. Shutdown: %s.\n"
                                       "Review the executable, arguments and project folder, and check the "
                                       "server's diagnostics.",
                                       umi_status_text(request->status),
                                       umi_status_text(request->report.initializationStatus),
                                       umi_status_text(request->report.shutdownStatus));
            gtk_label_set_text(GTK_LABEL(panel->message), text);
            g_free(text);
        }
        /* Keep Check disabled until every earlier notification has completed.
         * A reentrant click must not start a request that this completion then
         * accidentally cancels or overwrites. Strong control references keep
         * individual widgets alive if a notification removes their parent. */
        if (LanguagePanelLive(panel))
            gtk_widget_set_sensitive(panel->cancel, FALSE);
        if (LanguagePanelLive(panel))
            gtk_widget_set_sensitive(panel->check, TRUE);
    }
    g_clear_object(&widget);
    LanguageRequestFree(request);
}
static void LanguageCheckCancel(GtkButton *button, gpointer data)
{
    (void)button;
    LanguageCheckPanel *panel = LanguagePanel(GTK_WIDGET(data));
    if (panel != NULL && panel->pending != NULL)
    {
        umi_cancellation_token_request(panel->pending->cancel);
        gtk_label_set_text(GTK_LABEL(panel->message), "Cancelling the connection check...");
    }
}
static void LanguageCheckStart(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *widget = g_object_ref(GTK_WIDGET(data));
    LanguageCheckPanel *panel = LanguagePanel(widget);
    if (!LanguagePanelLive(panel) || panel->pending != NULL || !gtk_widget_is_sensitive(panel->check))
    {
        g_object_unref(widget);
        return;
    }
    LanguageCheckRequest *request = g_try_new0(LanguageCheckRequest, 1U);
    if (request == NULL)
    {
        gtk_label_set_text(GTK_LABEL(panel->message), "There is not enough memory to start the check.");
        g_object_unref(widget);
        return;
    }
    g_weak_ref_init(&request->panel, widget);
    UmiStatus status = umi_cancellation_token_create(&request->cancel);
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(request->profile.executable, sizeof(request->profile.executable),
                               gtk_editable_get_text(GTK_EDITABLE(panel->program)));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(request->profile.arguments, sizeof(request->profile.arguments),
                               gtk_editable_get_text(GTK_EDITABLE(panel->arguments)));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(request->directory, sizeof(request->directory),
                               gtk_editable_get_text(GTK_EDITABLE(panel->directory)));
    if (status == UMI_STATUS_OK &&
        (!umi_path_is_absolute(request->profile.executable) || !umi_path_is_absolute(request->directory)))
        status = UMI_STATUS_INVALID_ARGUMENT;
    gchar *uri = NULL;
    if (status == UMI_STATUS_OK)
    {
        uri = g_filename_to_uri(request->directory, NULL, NULL);
        status = uri == NULL ? UMI_STATUS_INVALID_ARGUMENT
                             : umi_text_copy(request->root, sizeof(request->root), uri);
    }
    g_free(uri);
    if (status != UMI_STATUS_OK)
    {
        LanguageRequestFree(request);
        gtk_label_set_text(GTK_LABEL(panel->message), "Enter an absolute executable path and project folder. "
                                                      "Keep each field within its supported length.");
        g_object_unref(widget);
        return;
    }
    (void)umi_text_copy(request->profile.id, sizeof(request->profile.id), "selected-language-server");
    request->profile.enabled = 1;
    panel->needsCheck = 0;
    panel->pending = request;
    g_object_set_data(G_OBJECT(panel->panel), "umicom-language-check-pending", GINT_TO_POINTER(1));
    gtk_widget_set_sensitive(panel->check, FALSE);
    if (LanguagePanelLive(panel))
        gtk_widget_set_sensitive(panel->cancel, TRUE);
    if (LanguagePanelLive(panel))
        gtk_label_set_text(GTK_LABEL(panel->message), "Checking the selected language server...");
    if (LanguagePanelLive(panel) && !umi_cancellation_token_is_requested(request->cancel))
    {
        GTask *task = g_task_new(NULL, NULL, LanguageCheckComplete, NULL);
        /* Completion owns GTK weak-reference disposal. The worker only owns
         * copied C values and its atomic cancellation token. */
        g_task_set_task_data(task, request, NULL);
        g_task_run_in_thread(task, LanguageCheckWorker);
        g_object_unref(task);
    }
    else
    {
        panel->pending = NULL;
        g_object_set_data(G_OBJECT(panel->panel), "umicom-language-check-pending", NULL);
        LanguageRequestFree(request);
        if (LanguagePanelLive(panel))
            gtk_widget_set_sensitive(panel->cancel, FALSE);
        if (LanguagePanelLive(panel))
            gtk_widget_set_sensitive(panel->check, TRUE);
    }
    g_object_unref(widget);
}
static GtkWidget *LanguageControl(LanguageCheckPanel *panel, GtkWidget *control, const char *id)
{
    gtk_box_append(GTK_BOX(panel->panel), control);
    g_ptr_array_add(panel->controls, g_object_ref(control));
    (void)umi_gtk4_automation_tag_widget(control, id);
    return control;
}
static GtkWidget *LanguageEntry(LanguageCheckPanel *panel, const char *title, const char *id,
                                const char *value)
{
    GtkWidget *label = gtk_label_new(title);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(panel->panel), label);
    GtkWidget *entry = LanguageControl(panel, gtk_entry_new(), id);
    gtk_editable_set_text(GTK_EDITABLE(entry), value == NULL ? "" : value);
    g_signal_connect_object(entry, "changed", G_CALLBACK(LanguageDraftChanged), panel->panel, 0);
    return entry;
}
UmiStatus UmiGtk4LanguageConnectionPanelCreate(const char *workspaceDirectory, GtkWidget **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    LanguageCheckPanel *panel = g_try_new0(LanguageCheckPanel, 1U);
    if (panel == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    panel->panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    panel->controls = g_ptr_array_new_with_free_func(g_object_unref);
    g_object_set_data_full(G_OBJECT(panel->panel), "umicom-language-connection", panel, LanguagePanelFree);
    (void)umi_gtk4_automation_tag_widget(panel->panel, "language.connection.panel");
    GtkWidget *intro =
        gtk_label_new("Check a trusted local language server. The check starts it in the selected "
                      "folder, reads its capabilities and closes it. No document text is sent, but the "
                      "server can inspect that folder.");
    gtk_label_set_wrap(GTK_LABEL(intro), TRUE);
    gtk_label_set_xalign(GTK_LABEL(intro), 0.0F);
    gtk_box_append(GTK_BOX(panel->panel), intro);
    panel->program =
        LanguageEntry(panel, "Server executable (absolute path)", "language.connection.executable", "");
    panel->arguments = LanguageEntry(panel, "Arguments (quoted values; no shell expansion)",
                                     "language.connection.arguments", "");
    panel->directory = LanguageEntry(panel, "Project folder (absolute path)", "language.connection.directory",
                                     workspaceDirectory);
    panel->check = LanguageControl(panel, gtk_button_new_with_label("Check language server"),
                                   "language.connection.check");
    panel->cancel =
        LanguageControl(panel, gtk_button_new_with_label("Cancel check"), "language.connection.cancel");
    panel->message =
        LanguageControl(panel, gtk_label_new("No connection check has run."), "language.connection.result");
    gtk_label_set_wrap(GTK_LABEL(panel->message), TRUE);
    gtk_label_set_selectable(GTK_LABEL(panel->message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->message), 0.0F);
    gtk_widget_set_sensitive(panel->cancel, FALSE);
    g_signal_connect_object(panel->check, "clicked", G_CALLBACK(LanguageCheckStart), panel->panel, 0);
    g_signal_connect_object(panel->cancel, "clicked", G_CALLBACK(LanguageCheckCancel), panel->panel, 0);
    g_signal_connect(panel->panel, "map", G_CALLBACK(LanguagePanelMapped), NULL);
    g_signal_connect(panel->panel, "unmap", G_CALLBACK(LanguagePanelUnmapped), NULL);
    *out = panel->panel;
    return UMI_STATUS_OK;
}
