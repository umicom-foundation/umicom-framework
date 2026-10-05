/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/provider_chat_worker_gtk4.c
 * PURPOSE: Run preparation and chat outside the GTK thread with owned credentials and cancellation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "provider_chat_internal.h"
#include "umicom/security/secrets.h"
#include <inttypes.h>
#include <string.h>
void UmiProviderChatGtkJobFree(gpointer data)
{
    UmiProviderChatGtkJob *job = data;
    UmiProviderChatDestroy(job->plan);
    umi_cancellation_token_destroy(job->token);
    umi_secret_clear(job, sizeof(*job));
    g_free(job);
}
static void Worker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    UmiProviderChatGtkJob *job = data;
    UmiDataServer *server = NULL;
    UmiProviderConnections *store = NULL;
    job->status = umi_cancellation_token_is_requested(job->token)
                      ? UMI_STATUS_CANCELLED
                      : umi_data_server_create_sqlite(job->path, &server);
    if (job->status == UMI_STATUS_OK)
        job->status = UmiProviderConnectionsOpen(server, job->application, job->profile, &store);
    if (job->status == UMI_STATUS_OK)
    {
        if (job->send)
            job->status = job->run(job->plan, store, true, job->password, job->token, &job->result);
        else
            job->status =
                UmiProviderChatPrepare(store, job->application, job->profile, job->connection, job->revision,
                                       job->prompt, job->context, job->limit, &job->plan);
    }
    if (umi_cancellation_token_is_requested(job->token))
    {
        job->status = UMI_STATUS_CANCELLED;
        umi_secret_clear(&job->result, sizeof(job->result));
    }
    umi_secret_clear(job->password, sizeof(job->password));
    umi_secret_clear(job->prompt, sizeof(job->prompt));
    umi_secret_clear(job->context, sizeof(job->context));
    UmiProviderConnectionsDestroy(store);
    umi_data_server_destroy(server);
    g_task_return_boolean(task, TRUE);
}
/* Each job keeps one controller reference. On retirement we skip widgets but
 * still release the plan, buffers, token and final controller reference. */
static void Finished(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    UmiProviderChatGtk *panel = data;
    UmiProviderChatGtkJob *job = g_task_get_task_data(G_TASK(result));
    GError *error = NULL;
    (void)g_task_propagate_boolean(G_TASK(result), &error);
    if (error != NULL)
    {
        job->status = UMI_STATUS_INTERNAL_ERROR;
        g_clear_error(&error);
    }
    panel->token = NULL;
    if (!panel->closed)
    {
        panel->busy = false;
        if (job->send && job->status == UMI_STATUS_PERMISSION_DENIED)
        {
            if (panel->denials < 5U)
                ++panel->denials;
            panel->retry_after = g_get_monotonic_time() + (panel->denials >= 5U ? 30000000 : 1000000);
        }
        else if (job->send && job->status == UMI_STATUS_OK)
        {
            panel->denials = 0U;
            panel->retry_after = 0;
        }
        if (!job->send && job->status == UMI_STATUS_OK)
        {
            panel->plan = job->plan;
            job->plan = NULL;
            const UmiProviderConnectionCheckPlan *review = UmiProviderChatConnection(panel->plan);
            char *detail = g_strdup_printf(
                "POST %s\nModel: %s · output limit: %" PRIu32 "\nConnection: %s · saved revision: %" PRIu64
                "\n%s",
                review->connection.endpoint, review->connection.model,
                UmiProviderChatOutputLimit(panel->plan), review->connection.id, review->revision,
                review->requires_credential
                    ? "The verified API key and displayed text will leave this computer. Provider charges "
                      "and data policies apply."
                    : "The displayed text goes to this computer's loopback service without a credential.");
            gtk_label_set_text(GTK_LABEL(panel->details), detail);
            g_free(detail);
            UmiProviderChatGtkText(panel->preview, UmiProviderChatInput(panel->plan));
            gtk_label_set_text(
                GTK_LABEL(panel->message),
                "Review prepared. Enter the local password if needed, then approve this specific send.");
        }
        else
        {
            if (job->send)
                UmiProviderChatGtkText(panel->result, job->status == UMI_STATUS_OK ? job->result.text : "");
            const char *meaning =
                job->status == UMI_STATUS_OK
                    ? (job->result.refused     ? "Provider refusal"
                       : job->result.truncated ? "Output limit reached; reply is incomplete"
                                               : "Reply received")
                : job->status == UMI_STATUS_BUSY
                    ? "Saved settings changed or service busy; reload the connection and review again"
                : job->status == UMI_STATUS_CANCELLED
                    ? "Cancelled; a transmitted request may still finish remotely"
                : job->status == UMI_STATUS_UNAVAILABLE ? "Required transport or local storage is unavailable"
                : job->status == UMI_STATUS_PERMISSION_DENIED
                    ? "Local or remote permission denied, or an actionable reply was refused"
                : job->status == UMI_STATUS_TIMEOUT
                    ? "Timed out; the remote outcome is uncertain"
                    : "Request could not complete; review settings and input limits";
            char *message = g_strdup_printf(
                "%s.\nHTTP: %u · reported model: %s\nNo automatic retry. Review again before another send.",
                meaning, job->result.http_status,
                job->result.model[0] != '\0' ? job->result.model : "not reported");
            gtk_label_set_text(GTK_LABEL(panel->message), message);
            g_free(message);
        }
        /* Record only validated successful sends, on the owner thread and
         * before the task retires its plan. A local history failure never
         * changes the remote result and must not cause an automatic resend. */
        UmiProviderChatGtkHistoryRecord(panel, job);
        UmiProviderChatGtkSensitivity(panel);
    }
    UmiProviderChatGtkRelease(panel);
}
void UmiProviderChatGtkLaunch(UmiProviderChatGtk *panel, UmiProviderChatGtkJob *job)
{
    panel->busy = true;
    panel->token = job->token;
    ++panel->references;
    /* A new operation must not leave an earlier reply beside its status. */
    UmiProviderChatGtkText(panel->result, "");
    UmiProviderChatGtkSensitivity(panel);
    gtk_label_set_text(GTK_LABEL(panel->message), job->send ? "Sending this reviewed request once…"
                                                            : "Preparing review from saved settings…");
    GTask *task = g_task_new(NULL, NULL, Finished, panel);
    g_task_set_task_data(task, job, UmiProviderChatGtkJobFree);
    g_task_run_in_thread(task, Worker);
    g_object_unref(task);
}
