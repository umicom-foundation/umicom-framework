/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_gateway.c
 *
 * PURPOSE:
 *   Own asynchronous Gateway discovery and an explicit local launch action.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/gateway_gtk4.h"
#include "ibkr_gateway_discovery.h"

struct UmiIbkrGatewayGtk {
    unsigned references;
    gboolean closed, busy;
    GtkWidget *root, *paths, *custom, *refresh, *open, *status;
    char **roots;
    GCancellable *cancel;
};
typedef struct GatewayJob {
    gboolean launch, incomplete;
    char *input;
    char **roots, **paths;
} GatewayJob;

static void Release(UmiIbkrGatewayGtk *ui)
{
    if (--ui->references != 0U) return;
    g_clear_object(&ui->cancel);
    g_strfreev(ui->roots);
    g_object_unref(ui->paths);
    g_object_unref(ui->custom);
    g_object_unref(ui->refresh);
    g_object_unref(ui->open);
    g_object_unref(ui->status);
    g_object_unref(ui->root);
    g_free(ui);
}
static void FreeJob(gpointer data)
{
    GatewayJob *job = data;
    g_free(job->input); g_strfreev(job->roots); g_strfreev(job->paths); g_free(job);
}
static void Work(GTask *task, gpointer object, gpointer data, GCancellable *cancel)
{
    (void)object;
    GatewayJob *job = data;
    GError *error = NULL;
    gboolean ok = job->launch ? UmiIbkrGatewayLaunch(job->input, cancel, &error)
        : UmiIbkrGatewayDiscover((const char *const *)job->roots, job->input,
            cancel, &job->paths, &job->incomplete, &error);
    if (ok) g_task_return_boolean(task, TRUE);
    else g_task_return_error(task, error != NULL ? error :
        g_error_new_literal(G_IO_ERROR, G_IO_ERROR_FAILED, "Gateway operation failed."));
}
static void Complete(GObject *object, GAsyncResult *result, gpointer data)
{
    (void)object;
    UmiIbkrGatewayGtk *ui = data;
    GTask *task = G_TASK(result);
    GatewayJob *job = g_task_get_task_data(task);
    GError *error = NULL;
    gboolean ok = g_task_propagate_boolean(task, &error);
    if (!ui->closed) {
        if (!job->launch) {
            const char *empty[] = {NULL};
            GtkStringList *model = gtk_string_list_new(ok ? (const char *const *)job->paths : empty);
            gtk_drop_down_set_model(GTK_DROP_DOWN(ui->paths), G_LIST_MODEL(model));
            g_object_unref(model);
            if (!ui->closed)
                gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->paths),
                    ok && job->paths[0] != NULL ? 0U : GTK_INVALID_LIST_POSITION);
        }
        if (!ui->closed) {
            const char *message = !ok ? (job->launch
                ? "Gateway could not be opened. Check the selected installed path."
                : "Gateway path could not be checked. Use an absolute local ibgateway launcher path.")
                : job->launch ? "Launch requested. Sign in to IB Gateway and complete its authentication. API connection is a separate step."
                : job->paths[0] != NULL ? (job->incomplete
                    ? "Gateway found; search was limited. Select a path or enter another installed path."
                    : "Gateway found. Choose Open IB Gateway to sign in.")
                : job->incomplete ? "Search was incomplete. Enter the installed Gateway launcher path and choose Refresh."
                : "No Gateway found in the standard locations. Enter its installed launcher path or download Gateway.";
            gtk_label_set_text(GTK_LABEL(ui->status), message);
        }
        if (!ui->closed) gtk_widget_set_sensitive(ui->custom, TRUE);
        if (!ui->closed) gtk_widget_set_sensitive(ui->paths, TRUE);
        if (!ui->closed) gtk_widget_set_sensitive(ui->refresh, TRUE);
        if (!ui->closed) gtk_widget_set_sensitive(ui->open,
            gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->paths)) != GTK_INVALID_LIST_POSITION);
        if (!ui->closed) ui->busy = FALSE;
    }
    g_clear_error(&error);
    Release(ui);
}
/* Notifications may close the host or emit another command. Acquire ownership
 * and mark busy before changing a widget, then recheck retirement afterward. */
static void Start(UmiIbkrGatewayGtk *ui, gboolean launch)
{
    if (ui->closed || ui->busy) return;
    GatewayJob *job = g_new0(GatewayJob, 1);
    job->launch = launch;
    if (launch) {
        GtkStringObject *selected = gtk_drop_down_get_selected_item(GTK_DROP_DOWN(ui->paths));
        if (selected == NULL) { g_free(job); return; }
        job->input = g_strdup(gtk_string_object_get_string(selected));
    } else {
        job->input = g_strdup(gtk_editable_get_text(GTK_EDITABLE(ui->custom)));
        job->roots = g_strdupv(ui->roots);
    }
    ++ui->references;
    ui->busy = TRUE;
    gtk_widget_set_sensitive(ui->open, FALSE);
    if (!ui->closed) gtk_widget_set_sensitive(ui->refresh, FALSE);
    if (!ui->closed) gtk_widget_set_sensitive(ui->custom, FALSE);
    if (!ui->closed) gtk_widget_set_sensitive(ui->paths, FALSE);
    if (!ui->closed) gtk_label_set_text(GTK_LABEL(ui->status),
        launch ? "Opening IB Gateway..." : "Checking local Gateway installations...");
    if (ui->closed) { FreeJob(job); Release(ui); return; }
    g_clear_object(&ui->cancel);
    ui->cancel = g_cancellable_new();
    GTask *task = g_task_new(NULL, ui->cancel, Complete, ui);
    g_task_set_task_data(task, job, FreeJob);
    g_task_run_in_thread(task, Work);
    g_object_unref(task);
}
static void Refresh(GtkButton *button, gpointer data) { (void)button; Start(data, FALSE); }
static void Open(GtkButton *button, gpointer data) { (void)button; Start(data, TRUE); }
static void CustomChanged(GtkEditable *editable, gpointer data)
{
    (void)editable;
    UmiIbkrGatewayGtk *ui = data;
    if (ui->closed || ui->busy) return;
    ++ui->references;
    ui->busy = TRUE;
    /* Editing invalidates the old selection. Never launch a stale default when
     * the user has just supplied a different custom installation path. */
    gtk_drop_down_set_model(GTK_DROP_DOWN(ui->paths), NULL);
    if (!ui->closed) gtk_widget_set_sensitive(ui->open, FALSE);
    if (!ui->closed) gtk_label_set_text(GTK_LABEL(ui->status), "Choose Refresh to check this installation path.");
    if (!ui->closed) ui->busy = FALSE;
    Release(ui);
}
static GtkWidget *Owned(GtkWidget *root, GtkWidget *widget, const char *name)
{
    gtk_widget_set_name(widget, name);
    gtk_box_append(GTK_BOX(root), widget);
    return g_object_ref(widget);
}
UmiIbkrGatewayGtk *UmiIbkrGatewayGtkCreate(const char *const *roots)
{
    UmiIbkrGatewayGtk *ui = g_new0(UmiIbkrGatewayGtk, 1);
    ui->references = 1U;
    if (roots != NULL) {
        size_t count = 0U;
        while (count < 8U && roots[count] != NULL) ++count;
        ui->roots = g_new0(char *, count + 1U);
        for (size_t i = 0U; i < count; ++i) ui->roots[i] = g_strdup(roots[i]);
    }
    ui->root = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_widget_set_name(ui->root, "ibkr-gateway-panel");
    GtkWidget *label = gtk_label_new("IB Gateway on this computer");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(ui->root), label);
    ui->paths = Owned(ui->root, gtk_drop_down_new(NULL, NULL), "ibkr-gateway-installations");
    gtk_accessible_update_property(GTK_ACCESSIBLE(ui->paths),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Installed Gateway launcher", -1);
    ui->custom = Owned(ui->root, gtk_entry_new(), "ibkr-gateway-custom-path");
    gtk_entry_set_max_length(GTK_ENTRY(ui->custom), 4096);
    gtk_entry_set_placeholder_text(GTK_ENTRY(ui->custom), "Optional: full installed Gateway launcher path");
    gtk_accessible_update_property(GTK_ACCESSIBLE(ui->custom),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Custom Gateway launcher path", -1);
    ui->refresh = Owned(ui->root, gtk_button_new_with_label("Refresh installations"), "ibkr-gateway-refresh");
    ui->open = Owned(ui->root, gtk_button_new_with_label("Open IB Gateway"), "ibkr-gateway-open");
    ui->status = Owned(ui->root, gtk_label_new(""), "ibkr-gateway-status");
    gtk_label_set_wrap(GTK_LABEL(ui->status), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(ui->status), 54);
    gtk_label_set_xalign(GTK_LABEL(ui->status), 0.0F);
    GtkWidget *download = gtk_link_button_new_with_label(
        "https://www.interactivebrokers.com/en/trading/ibgateway-latest.php", "Download IB Gateway");
    gtk_box_append(GTK_BOX(ui->root), download);
    g_signal_connect(ui->refresh, "clicked", G_CALLBACK(Refresh), ui);
    g_signal_connect(ui->open, "clicked", G_CALLBACK(Open), ui);
    g_signal_connect(ui->custom, "changed", G_CALLBACK(CustomChanged), ui);
    Start(ui, FALSE);
    return ui;
}
GtkWidget *UmiIbkrGatewayGtkWidget(UmiIbkrGatewayGtk *ui)
{
    return ui != NULL && !ui->closed ? ui->root : NULL;
}
void UmiIbkrGatewayGtkDestroy(UmiIbkrGatewayGtk *ui)
{
    if (ui == NULL || ui->closed) return;
    ui->closed = TRUE;
    if (ui->cancel != NULL) g_cancellable_cancel(ui->cancel);
    g_signal_handlers_disconnect_by_data(ui->refresh, ui);
    g_signal_handlers_disconnect_by_data(ui->open, ui);
    g_signal_handlers_disconnect_by_data(ui->custom, ui);
    gtk_widget_set_sensitive(ui->root, FALSE);
    Release(ui);
}
