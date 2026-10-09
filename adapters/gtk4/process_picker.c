/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/process_picker.c
 * PURPOSE: Own asynchronous process discovery separately from the lifetime of process-selection widgets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/process_picker.h"
#include "umicom/ui/gtk4/automation.h"
#include <stdint.h>
#include <string.h>
#define PROCESS_PAGE_ROWS 20U
typedef struct ProcessProvider
{
    gint references;
    UmiGtk4ProcessProvider callbacks;
} ProcessProvider;
typedef struct ProcessPanel
{
    GtkWidget *root, *rows, *search, *status, *previous, *next, *refresh, *stop;
    UmiDesktopProcessCatalog *catalog;
    UmiDesktopProcessReport report;
    size_t first;
    GCancellable *cancel;
    ProcessProvider *provider;
    UmiGtk4ProcessChoose choose;
    void *context;
    GDestroyNotify destroy;
} ProcessPanel;
typedef struct ProcessCaptureJob
{
    GWeakRef root;
    GCancellable *cancel;
    ProcessProvider *provider;
    UmiDesktopProcessCatalog *catalog;
    UmiStatus status;
} ProcessCaptureJob;
typedef struct ProcessChoice
{
    UmiDesktopSystemProcess process;
    GWeakRef root;
} ProcessChoice;
static void ProcessProviderRelease(ProcessProvider *provider)
{
    if (g_atomic_int_dec_and_test(&provider->references))
    {
        if (provider->callbacks.destroy != NULL)
            provider->callbacks.destroy(provider->callbacks.context);
        g_free(provider);
    }
}
static void ProcessChoiceFree(gpointer data)
{
    ProcessChoice *choice = data;
    g_weak_ref_clear(&choice->root);
    g_free(choice);
}
static void ProcessPanelFree(gpointer data)
{
    ProcessPanel *panel = data;
    if (panel->cancel != NULL)
        g_cancellable_cancel(panel->cancel);
    g_clear_object(&panel->cancel);
    UmiDesktopProcessCatalogDestroy(panel->catalog);
    ProcessProviderRelease(panel->provider);
    if (panel->destroy != NULL)
        panel->destroy(panel->context);
    g_free(panel);
}
static void ProcessCaptureFree(gpointer data)
{
    ProcessCaptureJob *job = data;
    UmiDesktopProcessCatalogDestroy(job->catalog);
    ProcessProviderRelease(job->provider);
    g_clear_object(&job->cancel);
    g_weak_ref_clear(&job->root);
    g_free(job);
}
static int ProcessCaptureCancelled(void *context)
{
    return g_cancellable_is_cancelled(G_CANCELLABLE(context));
}
static void ProcessCaptureRun(GTask *task, gpointer object, gpointer data, GCancellable *cancel)
{
    (void)object;
    (void)cancel;
    ProcessCaptureJob *job = data;
    UmiDesktopProcessCaptureOptions options = {.cancelled = ProcessCaptureCancelled,
                                               .context = job->cancel};
    if (job->provider->callbacks.capture != NULL)
        job->status = job->provider->callbacks.capture(job->provider->callbacks.context, &options,
                                                       &job->catalog);
    else
        job->status = UmiDesktopProcessCatalogCapture(&options, &job->catalog);
    if (job->status == UMI_STATUS_OK && job->catalog == NULL)
        job->status = UMI_STATUS_INVALID_STATE;
    g_task_return_boolean(task, TRUE);
}
static void ProcessRowsClear(ProcessPanel *panel)
{
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(panel->rows)) != NULL)
        gtk_box_remove(GTK_BOX(panel->rows), child);
}
static void ProcessChoose(GtkButton *button, gpointer data)
{
    ProcessChoice *choice = data;
    GtkWidget *root = g_weak_ref_get(&choice->root);
    if (root == NULL)
        return;
    ProcessPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-process-picker");
    if (panel != NULL && panel->cancel == NULL && gtk_widget_get_mapped(GTK_WIDGET(button)) &&
        gtk_widget_is_ancestor(GTK_WIDGET(button), root) &&
        gtk_widget_get_sensitive(GTK_WIDGET(button)))
    {
        /* Copy before calling the host: it may close the panel, rebuild rows,
         * or change workspace while handling the explicit selection. */
        UmiDesktopSystemProcess process = choice->process;
        UmiStatus status = panel->choose(panel->context, &process);
        gtk_label_set_text(
            GTK_LABEL(panel->status),
            status == UMI_STATUS_OK
                ? "Process copied to the attach form. Review it, then choose Attach to process."
                : umi_status_text(status));
    }
    g_object_unref(root);
}
static gboolean ProcessMatches(const UmiDesktopSystemProcess *process, const char *query)
{
    char pid[32];
    g_snprintf(pid, sizeof pid, "%llu", (unsigned long long)process->pid);
    if (g_str_has_prefix(query, "pid:"))
        return strcmp(pid, query + 4) == 0;
    char *name = g_ascii_strdown(process->name, -1);
    gboolean match = strstr(pid, query) != NULL || strstr(name, query) != NULL;
    g_free(name);
    return match;
}
static void ProcessRender(ProcessPanel *panel)
{
    ProcessRowsClear(panel);
    char *query = g_ascii_strdown(gtk_editable_get_text(GTK_EDITABLE(panel->search)), -1);
    size_t matches = 0U, shown = 0U, count = UmiDesktopProcessCatalogCount(panel->catalog);
    for (size_t i = 0U; i < count; ++i)
    {
        UmiDesktopSystemProcess process;
        if (UmiDesktopProcessCatalogAt(panel->catalog, i, &process) != UMI_STATUS_OK ||
            !ProcessMatches(&process, query))
            continue;
        size_t match = matches++;
        if (match < panel->first || shown >= PROCESS_PAGE_ROWS)
            continue;
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        char *text = g_strdup_printf("%llu  %s  (parent %llu)%s", (unsigned long long)process.pid,
                                     process.name, (unsigned long long)process.parentPid,
                                     process.startKnown ? "" : " — creation time unavailable");
        GtkWidget *label = gtk_label_new(text);
        g_free(text);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        gtk_label_set_wrap(GTK_LABEL(label), TRUE);
        gtk_widget_set_hexpand(label, TRUE);
        GtkWidget *choose = gtk_button_new_with_label("Use process");
        gboolean selectable = !panel->report.fixture && process.startKnown && process.pid > 0U &&
                              process.pid <= INT32_MAX && process.state != 'Z' &&
                              process.state != 'X';
        gtk_widget_set_sensitive(choose, selectable);
        ProcessChoice *choice = g_new0(ProcessChoice, 1);
        choice->process = process;
        g_weak_ref_init(&choice->root, panel->root);
        g_object_set_data_full(G_OBJECT(choose), "umicom-process-choice", choice,
                               ProcessChoiceFree);
        g_signal_connect(choose, "clicked", G_CALLBACK(ProcessChoose), choice);
        char tag[80];
        g_snprintf(tag, sizeof tag, "process.picker.choose.%zu", shown++);
        (void)umi_gtk4_automation_tag_widget(choose, tag);
        gtk_box_append(GTK_BOX(row), label);
        gtk_box_append(GTK_BOX(row), choose);
        gtk_box_append(GTK_BOX(panel->rows), row);
    }
    g_free(query);
    gtk_widget_set_sensitive(panel->previous, panel->first != 0U);
    gtk_widget_set_sensitive(panel->next, panel->first + shown < matches);
    if (panel->catalog != NULL)
    {
        char *summary = g_strdup_printf(
            "%zu–%zu of %zu matching captured processes; %zu observed, %zu unreadable "
            "or without creation identity.%s%s Refresh to capture changes.",
            shown ? panel->first + 1U : 0U, panel->first + shown, matches, panel->report.seen,
            panel->report.unreadable,
            panel->report.limited ? " Scan limit reached; this list is incomplete." : "",
            panel->report.fixture ? " Fixture observations; selection is disabled." : "");
        gtk_label_set_text(GTK_LABEL(panel->status), summary);
        g_free(summary);
    }
}
static void ProcessCaptureComplete(GObject *object, GAsyncResult *result, gpointer data)
{
    (void)object;
    (void)data;
    GTask *task = G_TASK(result);
    ProcessCaptureJob *job = g_task_get_task_data(task);
    GError *error = NULL;
    (void)g_task_propagate_boolean(task, &error);
    g_clear_error(&error);
    GtkWidget *root = g_weak_ref_get(&job->root);
    if (root == NULL)
        return;
    ProcessPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-process-picker");
    if (panel != NULL && panel->cancel == job->cancel)
    {
        gboolean cancelled = g_cancellable_is_cancelled(job->cancel);
        g_clear_object(&panel->cancel);
        gtk_widget_set_sensitive(panel->refresh, TRUE);
        gtk_widget_set_sensitive(panel->stop, FALSE);
        if (!cancelled && gtk_widget_get_mapped(root) && job->status == UMI_STATUS_OK)
        {
            panel->catalog = job->catalog;
            job->catalog = NULL;
            (void)UmiDesktopProcessCatalogReport(panel->catalog, &panel->report);
            panel->first = 0U;
            ProcessRender(panel);
        }
        else
            gtk_label_set_text(GTK_LABEL(panel->status), cancelled ? "Process discovery stopped."
                                                                   : umi_status_text(job->status));
    }
    g_object_unref(root);
}
static void ProcessRefresh(GtkButton *button, gpointer data)
{
    ProcessPanel *panel = g_object_get_data(G_OBJECT(data), "umicom-process-picker");
    if (panel->cancel != NULL || !gtk_widget_get_mapped(GTK_WIDGET(button)))
        return;
    UmiDesktopProcessCatalogDestroy(panel->catalog);
    panel->catalog = NULL;
    ProcessRowsClear(panel);
    gtk_widget_set_sensitive(panel->previous, FALSE);
    gtk_widget_set_sensitive(panel->next, FALSE);
    panel->cancel = g_cancellable_new();
    ProcessCaptureJob *job = g_new0(ProcessCaptureJob, 1);
    g_weak_ref_init(&job->root, panel->root);
    job->cancel = g_object_ref(panel->cancel);
    job->provider = panel->provider;
    g_atomic_int_inc(&job->provider->references);
    gtk_widget_set_sensitive(panel->refresh, FALSE);
    gtk_widget_set_sensitive(panel->stop, TRUE);
    gtk_label_set_text(GTK_LABEL(panel->status), "Reading process names and creation identities…");
    /* The task holds no widget or workbench owner. Closing the panel cancels
     * discovery without keeping an otherwise closed application window alive. */
    GTask *task = g_task_new(NULL, job->cancel, ProcessCaptureComplete, NULL);
    g_task_set_task_data(task, job, ProcessCaptureFree);
    g_task_run_in_thread(task, ProcessCaptureRun);
    g_object_unref(task);
}
static void ProcessStop(GtkButton *button, gpointer data)
{
    ProcessPanel *panel = g_object_get_data(G_OBJECT(data), "umicom-process-picker");
    if (gtk_widget_get_mapped(GTK_WIDGET(button)) && panel->cancel != NULL)
    {
        g_cancellable_cancel(panel->cancel);
        gtk_label_set_text(GTK_LABEL(panel->status), "Stopping process discovery…");
    }
}
static void ProcessUnmap(GtkWidget *widget, gpointer data)
{
    (void)data;
    ProcessPanel *panel = g_object_get_data(G_OBJECT(widget), "umicom-process-picker");
    if (panel->cancel != NULL)
        g_cancellable_cancel(panel->cancel);
}
static void ProcessSearch(GtkEditable *editable, gpointer data)
{
    (void)editable;
    ProcessPanel *panel = g_object_get_data(G_OBJECT(data), "umicom-process-picker");
    panel->first = 0U;
    ProcessRender(panel);
}
static void ProcessPage(GtkButton *button, gpointer data)
{
    ProcessPanel *panel = g_object_get_data(G_OBJECT(data), "umicom-process-picker");
    if (!gtk_widget_get_mapped(GTK_WIDGET(button)) || !gtk_widget_get_sensitive(GTK_WIDGET(button)))
        return;
    if (GTK_WIDGET(button) == panel->previous)
        panel->first = panel->first >= PROCESS_PAGE_ROWS ? panel->first - PROCESS_PAGE_ROWS : 0U;
    else
        panel->first += PROCESS_PAGE_ROWS;
    ProcessRender(panel);
}
GtkWidget *UmiGtk4ProcessPickerCreate(const UmiGtk4ProcessProvider *provider,
                                      UmiGtk4ProcessChoose choose, void *context,
                                      GDestroyNotify destroy)
{
    if (choose == NULL || (provider != NULL && provider->capture == NULL))
        return NULL;
    ProcessPanel *panel = g_new0(ProcessPanel, 1);
    panel->choose = choose;
    panel->context = context;
    panel->destroy = destroy;
    panel->provider = g_new0(ProcessProvider, 1);
    panel->provider->references = 1;
    if (provider != NULL)
        panel->provider->callbacks = *provider;
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    panel->rows = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    panel->status = gtk_label_new("Choose Refresh processes to read the local process list.");
    gtk_label_set_xalign(GTK_LABEL(panel->status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(panel->status), TRUE);
    panel->search = gtk_search_entry_new();
    gtk_widget_set_tooltip_text(panel->search,
                                "Filter captured names or PIDs; use pid:123 for an exact PID");
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    panel->refresh = gtk_button_new_with_label("Refresh processes");
    panel->stop = gtk_button_new_with_label("Stop discovery");
    panel->previous = gtk_button_new_with_label("Previous processes");
    panel->next = gtk_button_new_with_label("Next processes");
    GtkWidget *controls[] = {panel->refresh, panel->stop, panel->previous, panel->next};
    const char *tags[] = {"process.picker.refresh", "process.picker.stop",
                          "process.picker.previous", "process.picker.next"};
    for (size_t i = 0U; i < 4U; ++i)
    {
        gtk_box_append(GTK_BOX(actions), controls[i]);
        (void)umi_gtk4_automation_tag_widget(controls[i], tags[i]);
        gtk_widget_set_sensitive(controls[i], i == 0U);
    }
    (void)umi_gtk4_automation_tag_widget(panel->root, "process.picker");
    (void)umi_gtk4_automation_tag_widget(panel->search, "process.picker.search");
    (void)umi_gtk4_automation_tag_widget(panel->status, "process.picker.status");
    gtk_box_append(GTK_BOX(panel->root), actions);
    gtk_box_append(GTK_BOX(panel->root), panel->search);
    gtk_box_append(GTK_BOX(panel->root), panel->status);
    gtk_box_append(GTK_BOX(panel->root), panel->rows);
    g_object_set_data_full(G_OBJECT(panel->root), "umicom-process-picker", panel, ProcessPanelFree);
    g_signal_connect_object(panel->refresh, "clicked", G_CALLBACK(ProcessRefresh), panel->root, 0);
    g_signal_connect_object(panel->stop, "clicked", G_CALLBACK(ProcessStop), panel->root, 0);
    g_signal_connect_object(panel->previous, "clicked", G_CALLBACK(ProcessPage), panel->root, 0);
    g_signal_connect_object(panel->next, "clicked", G_CALLBACK(ProcessPage), panel->root, 0);
    g_signal_connect_object(panel->search, "changed", G_CALLBACK(ProcessSearch), panel->root, 0);
    g_signal_connect(panel->root, "unmap", G_CALLBACK(ProcessUnmap), NULL);
    return panel->root;
}
