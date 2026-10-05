/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/security/profile_keys_worker_gtk4.c
 * PURPOSE: Keep credential bytes in short-lived worker-owned memory.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "profile_keys_internal.h"
#include <string.h>

static void JobFree(gpointer data)
{
    UmiProfileKeyJob *job = data;
    umi_secret_clear(job, sizeof(*job));
    g_free(job);
}
static void KeyWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    UmiProfileKeyJob *job = data;
    UmiProfileSecrets *secrets = NULL;
    job->status = job->open_service(job->application, job->profile, &secrets);
    if (job->status == UMI_STATUS_OK && secrets == NULL) job->status = UMI_STATUS_INVALID_STATE;
    if (job->status == UMI_STATUS_OK) {
        switch (job->operation) {
        case UMI_PROFILE_KEY_REGISTER:
            job->status = UmiProfileSecretsRegister(secrets, job->password); break;
        case UMI_PROFILE_KEY_SAVE:
            job->status = UmiProfileSecretsSet(secrets, job->password, job->alias, job->value); break;
        case UMI_PROFILE_KEY_CHECK:
            /* Reuse the private buffer only for this short availability check.
             * Neither the key nor a provider's arbitrary error text reaches GTK. */
            job->status = UmiProfileSecretsGet(secrets, job->password, job->alias, job->value, sizeof(job->value)); break;
        case UMI_PROFILE_KEY_REMOVE:
            job->status = UmiProfileSecretsRemove(secrets, job->password, job->alias); break;
        default: job->status = UMI_STATUS_INVALID_ARGUMENT; break;
        }
    }
    UmiProfileSecretsDestroy(secrets);
    /* The callback receives only the operation and status as useful results.
     * Wipe sensitive copies before scheduling any completion on the GTK thread. */
    umi_secret_clear(job->password, sizeof(job->password));
    umi_secret_clear(job->value, sizeof(job->value));
    g_task_return_boolean(task, TRUE);
}
static void KeyFinished(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    UmiProfileKeysGtk *panel = data;
    UmiProfileKeyJob *job = g_task_get_task_data(G_TASK(result));
    GError *error = NULL;
    (void)g_task_propagate_boolean(G_TASK(result), &error);
    if (error != NULL) { job->status = UMI_STATUS_INTERNAL_ERROR; g_clear_error(&error); }
    if (!panel->closed) UmiProfileKeysCompleted(panel, job);
    UmiProfileKeysRelease(panel);
}
void UmiProfileKeysStart(UmiProfileKeysGtk *panel, UmiProfileKeyJob *job)
{
    memcpy(job->application, panel->application, sizeof(job->application));
    memcpy(job->profile, panel->profile, sizeof(job->profile));
    job->open_service = panel->open_service;
    panel->busy = true;
    gtk_widget_set_sensitive(panel->form, FALSE);
    gtk_label_set_text(GTK_LABEL(panel->message), "Working with the local profile and credential store…");
    ++panel->references;
    /* Cancellation cannot undo a native credential write. Normal close waits;
     * forced retirement suppresses presentation while the owned job finishes. */
    GTask *task = g_task_new(NULL, NULL, KeyFinished, panel);
    g_task_set_task_data(task, job, JobFree);
    g_task_run_in_thread(task, KeyWorker);
    g_object_unref(task);
}
