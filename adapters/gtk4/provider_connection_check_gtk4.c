/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/provider_connection_check_gtk4.c
 * PURPOSE: Present a reviewed model check without blocking GTK or changing AI request routing.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "provider_connections_internal.h"
#include "umicom/provider_connections/chat_gtk4.h"
#include "umicom/security/local_profile.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <inttypes.h>
#include <string.h>

typedef struct ConnectionCheckJob {
    UmiProviderConnectionCheckPlan plan;
    char path[UMI_PATH_CAPACITY], password[UMI_LOCAL_PROFILE_PASSWORD_CAPACITY];
    UmiCancellationToken *token;
    UmiStatus status;
    UmiProviderConnectionCheckResult result;
    UmiStatus (*run)(const UmiProviderConnectionCheckPlan *, UmiProviderConnections *, bool,
        const char *, const UmiCancellationToken *, UmiProviderConnectionCheckResult *);
} ConnectionCheckJob;
static UmiProviderConnectionsGtk *Panel(gpointer root)
{
    return g_object_get_data(G_OBJECT(root), "umicom-provider-editor");
}
/* Capture only saved settings: the worker will compare this review with the
 * database again, so a draft can never silently choose another destination. */
static UmiStatus CapturedPlan(UmiProviderConnectionsGtk *panel, UmiProviderConnectionCheckPlan *out)
{
    if (!panel->loaded || panel->dirty || !panel->editing_existing) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < panel->snapshot.count; ++i) if (strcmp(panel->snapshot.items[i].id, panel->selected_id) == 0) {
        UmiProviderConnectionCheckPlan plan = {0};
        plan.connection = panel->snapshot.items[i]; plan.revision = panel->snapshot.revision;
        strcpy(plan.application, panel->application); strcpy(plan.profile, panel->profile);
        UmiStatus status = UmiProviderConnectionCheckDescribe(&plan.connection, plan.check_endpoint,
            sizeof(plan.check_endpoint), &plan.requires_credential);
        if (status == UMI_STATUS_OK) *out = plan;
        return status;
    }
    return UMI_STATUS_NOT_FOUND;
}
/* Every metadata change invalidates approval and drops the typed password.
 * Keep check controls outside the disabled form so cancellation stays usable. */
void UmiProviderCheckRefresh(UmiProviderConnectionsGtk *panel)
{
    if (panel->check_detail == NULL || panel->closed) return;
    UmiProviderConnectionCheckPlan plan = {0}; UmiStatus status = CapturedPlan(panel, &plan);
    bool available = UmiProviderConnectionCheckAvailable() || panel->check_run != UmiProviderConnectionCheckRun;
    bool ready = status == UMI_STATUS_OK && available && !panel->busy;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->check_confirm), FALSE);
    gtk_editable_set_text(GTK_EDITABLE(panel->check_password), "");
    gtk_widget_set_sensitive(panel->check_password, ready && plan.requires_credential);
    gtk_widget_set_sensitive(panel->check_confirm, ready);
    gtk_widget_set_sensitive(panel->check_start, ready);
    gtk_widget_set_sensitive(panel->check_cancel, panel->check_token != NULL);
    char *text;
    if (status == UMI_STATUS_OK) text = g_strdup_printf(
        "Model catalogue check · saved revision %" PRIu64 "\nGET %s\nConfigured model: %s\n%s\nNo prompt, source file or tool request is sent.",
        plan.revision, plan.check_endpoint, plan.connection.model,
        plan.requires_credential ? "The verified local key is sent only to this HTTPS destination." : "This loopback request sends no credential.");
    else text = g_strdup("Save and select an enabled connection with a model. Supported adapters: openai at its official Responses/chat endpoint, or local-chat at the exact loopback chat endpoint.");
    gtk_label_set_text(GTK_LABEL(panel->check_detail), text); g_free(text);
    if (!available) gtk_label_set_text(GTK_LABEL(panel->check_result), "Model-check HTTP support is unavailable in this build.");
}
static void PasswordChanged(GtkEditable *entry, gpointer root)
{
    (void)entry; UmiProviderConnectionsGtk *panel = Panel(root);
    if (panel != NULL && !panel->closed && panel->check_confirm != NULL) gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->check_confirm), FALSE);
}
static void JobFree(gpointer data)
{
    ConnectionCheckJob *job = data;
    umi_cancellation_token_destroy(job->token);
    umi_secret_clear(job, sizeof(*job)); g_free(job);
}
static void Worker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel; ConnectionCheckJob *job = data;
    UmiDataServer *server = NULL; UmiProviderConnections *store = NULL;
    /* Each worker owns its SQLite connection. The reviewed model check only
     * reads settings; a missing directory is not silently recreated here. */
    job->status = umi_cancellation_token_is_requested(job->token) ? UMI_STATUS_CANCELLED :
        umi_data_server_create_sqlite(job->path, &server);
    if (job->status == UMI_STATUS_OK) job->status = UmiProviderConnectionsOpen(server,
        job->plan.application, job->plan.profile, &store);
    if (job->status == UMI_STATUS_OK) job->status = job->run(&job->plan, store, true,
        job->password, job->token, &job->result);
    umi_secret_clear(job->password, sizeof(job->password));
    UmiProviderConnectionsDestroy(store); umi_data_server_destroy(server);
    g_task_return_boolean(task, TRUE);
}
static const char *ResultText(UmiStatus status)
{
    switch (status) {
    case UMI_STATUS_OK: return "The catalogue lists the configured model. Chat, inference permission, balance and coding actions were not tested.";
    case UMI_STATUS_BUSY: return "Saved settings changed, storage is busy, or the provider asked to wait. Reload and review before another check.";
    case UMI_STATUS_PERMISSION_DENIED: return "Approval, the local password, destination policy or remote access was not accepted.";
    case UMI_STATUS_NOT_FOUND: return "The connection or model was not found. Check the HTTP status and the saved model identifier.";
    case UMI_STATUS_CANCELLED: return "Check cancelled. A request already sent may still have reached the server.";
    case UMI_STATUS_TIMEOUT: return "The connection check timed out. No automatic retry was made.";
    case UMI_STATUS_PARSE_ERROR: return "The server reply was not a valid model catalogue. No service response text is shown.";
    case UMI_STATUS_CAPACITY_EXCEEDED: return "The reply or a value exceeded the bounded check capacity. The result is inconclusive.";
    case UMI_STATUS_UNAVAILABLE: return "The HTTP transport, local credential store or settings storage is unavailable.";
    default: return "The check could not be completed. Review the endpoint, saved settings and local network availability.";
    }
}
/* The job holds one controller reference through completion. A retired panel
 * has no live widget binding, but the worker still releases its resources. */
static void Finished(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; UmiProviderConnectionsGtk *panel = data;
    ConnectionCheckJob *job = g_task_get_task_data(G_TASK(result)); GError *error = NULL;
    (void)g_task_propagate_boolean(G_TASK(result), &error);
    if (error != NULL) { job->status = UMI_STATUS_INTERNAL_ERROR; g_clear_error(&error); }
    panel->check_token = NULL;
    if (!panel->closed) {
        panel->busy = false;
        if (job->status == UMI_STATUS_PERMISSION_DENIED) {
            if (panel->check_failures < 5U) ++panel->check_failures;
            panel->check_retry_after = g_get_monotonic_time() + (panel->check_failures >= 5U ? 30000000 : 1000000);
        } else if (job->status == UMI_STATUS_OK) { panel->check_failures = 0U; panel->check_retry_after = 0; }
        gtk_widget_set_sensitive(panel->form, TRUE); UmiProviderCheckRefresh(panel);
        char *message = g_strdup_printf("%s\nHTTP status: %u · catalogue entries: %zu\nResult for %s at saved revision %" PRIu64,
            ResultText(job->status), job->result.http_status, job->result.listed_models,
            job->plan.connection.id, job->plan.revision);
        gtk_label_set_text(GTK_LABEL(panel->check_result), message); g_free(message);
    }
    UmiProviderEditorRelease(panel);
}
/* Copy approved inputs into a short-lived worker owner, then clear the GTK
 * entry immediately. Do not retain a verified or unlocked session in the UI. */
static void Start(GtkButton *button, gpointer root)
{
    (void)button; UmiProviderConnectionsGtk *panel = Panel(root);
    if (panel == NULL || panel->closed || panel->busy) return;
    UmiProviderConnectionCheckPlan plan = {0};
    bool confirmed = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->check_confirm));
    UmiStatus status = CapturedPlan(panel, &plan);
    ConnectionCheckJob *job = g_try_new0(ConnectionCheckJob, 1);
    if (job == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK && (!confirmed || g_get_monotonic_time() < panel->check_retry_after)) status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK && plan.requires_credential) {
        const char *password = gtk_editable_get_text(GTK_EDITABLE(panel->check_password));
        size_t length = strlen(password);
        if (length < 12U || length >= sizeof(job->password)) status = UMI_STATUS_INVALID_ARGUMENT;
        else memcpy(job->password, password, length + 1U);
    }
    gtk_editable_set_text(GTK_EDITABLE(panel->check_password), "");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->check_confirm), FALSE);
    if (status == UMI_STATUS_OK) status = umi_cancellation_token_create(&job->token);
    if (status != UMI_STATUS_OK) {
        if (job != NULL) JobFree(job);
        gtk_label_set_text(GTK_LABEL(panel->check_result), "No check started. Save a supported connection, enter its local profile password if needed, and approve the displayed request. Wait before retrying a denied password."); return;
    }
    job->plan = plan; strcpy(job->path, panel->path); job->run = panel->check_run;
    panel->busy = true; panel->check_token = job->token; ++panel->references;
    gtk_widget_set_sensitive(panel->form, FALSE); UmiProviderCheckRefresh(panel);
    gtk_label_set_text(GTK_LABEL(panel->check_result), "Checking the reviewed model catalogue…");
    GTask *task = g_task_new(NULL, NULL, Finished, panel);
    g_task_set_task_data(task, job, JobFree); g_task_run_in_thread(task, Worker); g_object_unref(task);
}
static void Cancel(GtkButton *button, gpointer root)
{
    (void)button; UmiProviderConnectionsGtk *panel = Panel(root);
    if (panel == NULL || panel->closed || panel->check_token == NULL) return;
    umi_cancellation_token_request(panel->check_token);
    gtk_label_set_text(GTK_LABEL(panel->check_result), "Cancelling the check. Waiting for the worker to release its credential and transport.");
}
/* Open chat for the selected saved record only. The new window copies its
 * scope and revision; editing another connection cannot retarget that window. */
static void ChatSelected(GtkButton *button, gpointer root)
{
    (void)button; UmiProviderConnectionsGtk *panel = Panel(root);
    if (panel == NULL || panel->closed || panel->busy) return;
    UmiProviderConnectionCheckPlan plan = {0}; UmiStatus status = CapturedPlan(panel, &plan);
    GtkRoot *parent = gtk_widget_get_root(panel->root);
    if (status == UMI_STATUS_OK && parent != NULL && GTK_IS_WINDOW(parent)) {
        UmiProviderChatGtkConfig config = {{panel->application, panel->profile, panel->path}, plan.connection.id, plan.revision};
        /* Direct opening could not carry an explicitly captured selection.
         * The shared handoff copies its draft and clears it after success;
         * the earlier call remains below for engineering review. */
#if 0
        status = UmiProviderChatGtkPresent(GTK_WINDOW(parent), &config);
#endif
        status = UmiProviderChatContextOpen(panel, GTK_WINDOW(parent), &config);
    } else if (status == UMI_STATUS_OK) status = UMI_STATUS_INVALID_STATE;
    gtk_label_set_text(GTK_LABEL(panel->check_result), status == UMI_STATUS_OK ? "Chat opened for the selected saved connection. Nothing has been sent."
        : "Chat could not open. Select a supported saved connection without an unsaved draft in an application window.");
}
static GtkWidget *Label(GtkWidget *box, const char *text, const char *id)
{
    GtkWidget *label = gtk_label_new(text); gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F); (void)umi_gtk4_automation_tag_widget(label, id);
    gtk_box_append(GTK_BOX(box), label); return label;
}
static GtkWidget *Button(UmiProviderConnectionsGtk *panel, GtkWidget *box,
    const char *text, const char *id, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(text); (void)umi_gtk4_automation_tag_widget(button, id);
    g_signal_connect_object(button, "clicked", callback, panel->root, 0);
    gtk_box_append(GTK_BOX(box), button); return button;
}
void UmiProviderCheckControls(UmiProviderConnectionsGtk *panel, GtkWidget *box)
{
    panel->check_run = UmiProviderConnectionCheckRun;
    panel->check_detail = Label(box, "Select saved settings to review a model check.", "connections.check-detail");
    Label(box, "Local profile password for remote checks; no password for loopback", "connections.check-password-label");
    panel->check_password = gtk_password_entry_new();
    gtk_password_entry_set_show_peek_icon(GTK_PASSWORD_ENTRY(panel->check_password), TRUE);
    gtk_accessible_update_property(GTK_ACCESSIBLE(panel->check_password), GTK_ACCESSIBLE_PROPERTY_LABEL, "Local profile password for model check", -1);
    (void)UmiGtk4RecordingSetPrivate(panel->check_password, 1);
    (void)umi_gtk4_automation_tag_widget(panel->check_password, "connections.check-password");
    g_signal_connect_object(panel->check_password, "changed", G_CALLBACK(PasswordChanged), panel->root, 0);
    gtk_box_append(GTK_BOX(box), panel->check_password);
    panel->check_confirm = gtk_check_button_new_with_label("I approve this model-catalogue request to the displayed destination");
    (void)umi_gtk4_automation_tag_widget(panel->check_confirm, "connections.check-confirm");
    gtk_box_append(GTK_BOX(box), panel->check_confirm);
    panel->check_start = Button(panel, box, "Check saved connection", "connections.check", G_CALLBACK(Start));
    panel->check_cancel = Button(panel, box, "Cancel connection check", "connections.check-cancel", G_CALLBACK(Cancel));
    panel->check_result = Label(box, "No connection check has run.", "connections.check-result");
    Button(panel, box, "Chat with selected saved connection…", "connections.chat", G_CALLBACK(ChatSelected));
}
void UmiProviderCheckRetire(UmiProviderConnectionsGtk *panel)
{
    if (panel->check_token != NULL) umi_cancellation_token_request(panel->check_token);
    if (panel->check_password != NULL) gtk_editable_set_text(GTK_EDITABLE(panel->check_password), "");
}
