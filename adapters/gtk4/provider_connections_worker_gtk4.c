/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/provider_connections_worker_gtk4.c
 * PURPOSE: Run owned SQLite jobs without blocking or borrowing UI state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "provider_connections_internal.h"
#include <glib/gstdio.h>
#include <string.h>

/* Each job owns a separate Data Server connection. SQLite transactions still
 * arbitrate edits from other panels and processes through the shared revision.
 * No worker reads widgets or keeps pointers into the application's runtime. */
static void ConnectionWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    UmiProviderEditorJob *job = data;
    UmiDataServer *server = NULL;
    UmiProviderConnections *store = NULL;
    char *directory = g_path_get_dirname(job->path);
    job->status = g_mkdir_with_parents(directory, 0700) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    g_free(directory);
    if (job->status == UMI_STATUS_OK) job->status = umi_data_server_create_sqlite(job->path, &server);
    if (job->status == UMI_STATUS_OK) job->status = UmiProviderConnectionsOpen(server, job->application, job->profile, &store);
    if (job->status == UMI_STATUS_OK && job->operation == UMI_PROVIDER_EDITOR_SAVE) {
        job->status = UmiProviderConnectionsPut(store, &job->draft, job->replace_existing,
            job->expected_revision, &job->saved_revision);
        job->committed = job->status == UMI_STATUS_OK;
    } else if (job->status == UMI_STATUS_OK && job->operation == UMI_PROVIDER_EDITOR_REMOVE) {
        job->status = UmiProviderConnectionsRemove(store, job->draft.id,
            job->expected_revision, &job->saved_revision);
        job->committed = job->status == UMI_STATUS_OK;
    }
    job->reload_status = job->status;
    if (job->status == UMI_STATUS_OK) job->reload_status = UmiProviderConnectionsRead(store, &job->snapshot);
    UmiProviderConnectionsDestroy(store);
    umi_data_server_destroy(server);
    /* A failed reread after a successful commit is distinct from a failed
     * save. Completion must not invite the user to create the record again. */
    g_task_return_boolean(task, TRUE);
}
static void ConnectionFinished(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    UmiProviderConnectionsGtk *panel = data;
    UmiProviderEditorJob *job = g_task_get_task_data(G_TASK(result));
    GError *error = NULL;
    (void)g_task_propagate_boolean(G_TASK(result), &error);
    if (error != NULL) { job->status = UMI_STATUS_INTERNAL_ERROR; g_clear_error(&error); }
    if (!panel->closed) UmiProviderEditorCompleted(panel, job);
    UmiProviderEditorRelease(panel);
}
void UmiProviderEditorStart(UmiProviderConnectionsGtk *panel, UmiProviderEditorJob *job)
{
    memcpy(job->path, panel->path, sizeof(job->path));
    memcpy(job->application, panel->application, sizeof(job->application));
    memcpy(job->profile, panel->profile, sizeof(job->profile));
    panel->busy = true;
    gtk_widget_set_sensitive(panel->form, FALSE);
    UmiProviderCheckRefresh(panel);
    gtk_label_set_text(GTK_LABEL(panel->message), "Working with saved connection settings…");
    ++panel->references;
    /* Do not attach automatic cancellation: a cancelled UI wait cannot prove
     * that SQLite did not commit. Retirement suppresses presentation only. */
    GTask *task = g_task_new(NULL, NULL, ConnectionFinished, panel);
    g_task_set_task_data(task, job, g_free);
    g_task_run_in_thread(task, ConnectionWorker);
    g_object_unref(task);
}
