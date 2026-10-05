/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/provider_chat_panel_gtk4.c
 * PURPOSE: Present exact chat text and require fresh approval after every edit or attempted send.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "provider_chat_internal.h"
#include "provider_chat_context_internal.h"
#include "umicom/security/profile_secrets.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <string.h>
static UmiProviderChatGtk *Panel(gpointer root)
{
    return g_object_get_data(G_OBJECT(root), "umicom-provider-chat");
}
void UmiProviderChatGtkRelease(UmiProviderChatGtk *panel)
{
    if (--panel->references == 0U)
    {
        umi_secret_clear(panel, sizeof(*panel));
        g_free(panel);
    }
}
void UmiProviderChatGtkText(GtkWidget *view, const char *text)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)), text, -1);
}
static void Forget(UmiProviderChatGtk *panel)
{
    UmiProviderChatDestroy(panel->plan);
    panel->plan = NULL;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approve), FALSE);
    gtk_editable_set_text(GTK_EDITABLE(panel->password), "");
    gtk_label_set_text(GTK_LABEL(panel->details), "Review the current text before sending.");
    UmiProviderChatGtkText(panel->preview, "");
}
void UmiProviderChatGtkSensitivity(UmiProviderChatGtk *panel)
{
    gtk_widget_set_sensitive(panel->fields, !panel->busy);
    if (panel->history_box != NULL) gtk_widget_set_sensitive(panel->history_box, !panel->busy);
    gtk_widget_set_sensitive(panel->review, !panel->busy);
    gtk_widget_set_sensitive(panel->clear, !panel->busy);
    gtk_widget_set_sensitive(panel->cancel, panel->busy && panel->token != NULL);
    bool ready = !panel->busy && panel->plan != NULL;
    gtk_widget_set_sensitive(panel->approve, ready);
    gtk_widget_set_sensitive(panel->password,
                             ready && UmiProviderChatConnection(panel->plan)->requires_credential);
    gtk_widget_set_sensitive(
        panel->send, ready && (UmiProviderConnectionCheckAvailable() || panel->run != UmiProviderChatRun));
}
/* Input edits invalidate the immutable review; password edits only remove
 * approval. Neither action can mutate bytes already owned by a worker. */
static void Changed(gpointer object, gpointer root)
{
    (void)object;
    UmiProviderChatGtk *panel = Panel(root);
    if (panel == NULL || panel->closed || panel->busy || panel->painting)
        return;
    panel->dirty = true;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->discard), FALSE);
    Forget(panel);
    UmiProviderChatGtkSensitivity(panel);
}
static void PasswordChanged(GtkEditable *entry, gpointer root)
{
    (void)entry;
    UmiProviderChatGtk *panel = Panel(root);
    if (panel != NULL && !panel->closed && panel->approve != NULL)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approve), FALSE);
}
static UmiStatus CopyText(GtkWidget *view, char *out, size_t capacity)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter start, end;
    /* Count characters before allocating a GTK copy; accepted UTF-8 bytes may
     * be longer, so enforce the byte capacity again after the bounded copy. */
    if ((size_t)gtk_text_buffer_get_char_count(buffer) >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    size_t length = strlen(text);
    UmiStatus status = length < capacity ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        memcpy(out, text, length + 1U);
    umi_secret_clear(text, length);
    g_free(text);
    return status;
}
static void Start(GtkButton *button, gpointer root)
{
    UmiProviderChatGtk *panel = Panel(root);
    if (panel == NULL || panel->closed || panel->busy)
        return;
    bool send = GTK_WIDGET(button) == panel->send;
    UmiProviderChatGtkJob *job = g_try_new0(UmiProviderChatGtkJob, 1);
    UmiStatus status = job == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK && send)
    {
        if (panel->plan == NULL || !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approve)) ||
            g_get_monotonic_time() < panel->retry_after)
            status = UMI_STATUS_PERMISSION_DENIED;
        else if (UmiProviderChatConnection(panel->plan)->requires_credential)
        {
            const char *password = gtk_editable_get_text(GTK_EDITABLE(panel->password));
            size_t size = strlen(password);
            if (size < 12U || size >= sizeof(job->password))
                status = UMI_STATUS_INVALID_ARGUMENT;
            else
                memcpy(job->password, password, size + 1U);
        }
    }
    if (status == UMI_STATUS_OK && !send)
    {
        status = CopyText(panel->prompt, job->prompt, sizeof(job->prompt));
        if (status == UMI_STATUS_OK)
            status = CopyText(panel->context, job->context, sizeof(job->context));
        job->limit = (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->limit));
    }
    gtk_editable_set_text(GTK_EDITABLE(panel->password), "");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approve), FALSE);
    if (status == UMI_STATUS_OK)
        status = umi_cancellation_token_create(&job->token);
    if (status != UMI_STATUS_OK)
    {
        if (job != NULL)
            UmiProviderChatGtkJobFree(job);
        gtk_label_set_text(GTK_LABEL(panel->message),
                           "Nothing sent. Review bounded text, enter the local password if required, and "
                           "approve this attempt. Wait before retrying a denied password.");
        return;
    }
    job->send = send;
    job->run = panel->run;
    if (send)
    {
        job->plan = panel->plan;
        panel->plan = NULL;
    }
    else
        Forget(panel);
    strcpy(job->path, panel->path);
    strcpy(job->application, panel->application);
    strcpy(job->profile, panel->profile);
    strcpy(job->connection, panel->connection);
    job->revision = panel->revision;
    UmiProviderChatGtkLaunch(panel, job);
}
static void Cancel(GtkButton *button, gpointer root)
{
    (void)button;
    UmiProviderChatGtk *panel = Panel(root);
    if (panel == NULL || panel->closed || panel->token == NULL)
        return;
    umi_cancellation_token_request(panel->token);
    gtk_label_set_text(
        GTK_LABEL(panel->message),
        "Cancelling. A transmitted request may still complete remotely; no retry will be made.");
}
static void Clear(GtkButton *button, gpointer root)
{
    (void)button;
    UmiProviderChatGtk *panel = Panel(root);
    if (panel == NULL || panel->closed || panel->busy)
        return;
    /* Clear local text includes this window's history. Retained copies in a
     * different window or an already transmitted request remain independent. */
    if (UmiProviderChatGtkHistoryClear(panel) != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(panel->message), "Local history could not clear; nothing else was discarded.");
        return;
    }
    panel->painting = true;
    Forget(panel);
    UmiProviderChatGtkText(panel->prompt, "");
    UmiProviderChatGtkText(panel->context, "");
    UmiProviderChatGtkText(panel->result, "");
    panel->painting = false;
    panel->dirty = false;
    UmiProviderChatGtkSensitivity(panel);
    gtk_label_set_text(GTK_LABEL(panel->message),
                       "Local text cleared. This cannot erase content already received by a provider.");
}
static GtkWidget *Label(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label);
    return label;
}
static GtkWidget *View(UmiProviderChatGtk *panel, GtkWidget *box, const char *caption, const char *id,
                       bool editable)
{
    Label(box, caption);
    GtkWidget *view = gtk_text_view_new(), *scroll = gtk_scrolled_window_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), editable);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_size_request(scroll, -1, 130);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    (void)UmiGtk4RecordingSetPrivate(view, 1);
    (void)umi_gtk4_automation_tag_widget(view, id);
    gtk_accessible_update_property(GTK_ACCESSIBLE(view), GTK_ACCESSIBLE_PROPERTY_LABEL, caption, -1);
    if (editable)
        g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)), "changed", G_CALLBACK(Changed),
                                panel->root, 0);
    gtk_box_append(GTK_BOX(box), scroll);
    return view;
}
static GtkWidget *Button(UmiProviderChatGtk *panel, GtkWidget *box, const char *caption, const char *id,
                         GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(caption);
    (void)umi_gtk4_automation_tag_widget(button, id);
    g_signal_connect_object(button, "clicked", callback, panel->root, 0);
    gtk_box_append(GTK_BOX(box), button);
    return button;
}
UmiStatus UmiProviderChatGtkCreate(const UmiProviderChatGtkConfig *config, UmiProviderChatGtk **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (config == NULL || config->settings.database_path == NULL ||
        !g_path_is_absolute(config->settings.database_path) ||
        strlen(config->settings.database_path) >= UMI_PATH_CAPACITY ||
        UmiProfileSecretsScopeValidate(config->settings.application_id, config->settings.profile_id) !=
            UMI_STATUS_OK ||
        strlen(config->settings.application_id) >= 49U || strlen(config->settings.profile_id) >= 49U ||
        config->connection_id == NULL || strlen(config->connection_id) >= 49U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderConnection probe = {0};
    strcpy(probe.id, config->connection_id);
    strcpy(probe.provider_id, "local-chat");
    strcpy(probe.label, "Chat");
    strcpy(probe.endpoint, "http://127.0.0.1:8080/v1/chat/completions");
    probe.route = UMI_PROVIDER_CONNECTION_LOOPBACK;
    probe.timeout_ms = 30000U;
    if (UmiProviderConnectionValidate(&probe) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderChatGtk *panel = g_try_new0(UmiProviderChatGtk, 1);
    if (panel == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus history_status = UmiProviderChatHistoryCreate(&panel->history);
    if (history_status != UMI_STATUS_OK) { g_free(panel); return history_status; }
    panel->references = 1U;
    panel->revision = config->revision;
    panel->run = UmiProviderChatRun;
    strcpy(panel->path, config->settings.database_path);
    strcpy(panel->application, config->settings.application_id);
    strcpy(panel->profile, config->settings.profile_id);
    strcpy(panel->connection, config->connection_id);
    panel->root = gtk_scrolled_window_new();
    g_object_ref_sink(panel->root);
    g_object_set_data(G_OBJECT(panel->root), "umicom-provider-chat", panel);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(panel->root), box);
    Label(box, "Chat with the selected saved connection. Review all text before sending. No files, history "
               "or tools are added automatically. Remote requests may incur provider charges.");
    panel->fields = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_append(GTK_BOX(box), panel->fields);
    panel->prompt =
        View(panel, panel->fields, "Your prompt (up to 4096 UTF-8 bytes)", "provider-chat.prompt", true);
    panel->context =
        View(panel, panel->fields, "Optional context you choose to send (up to 8192 UTF-8 bytes)",
             "provider-chat.context", true);
    Label(panel->fields, "Maximum output tokens, including reasoning where the provider counts it");
    panel->limit = gtk_spin_button_new_with_range(16.0, 4096.0, 16.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->limit), 1024.0);
    gtk_accessible_update_property(GTK_ACCESSIBLE(panel->limit), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                   "Maximum output tokens", -1);
    gtk_box_append(GTK_BOX(panel->fields), panel->limit);
    g_signal_connect_object(panel->limit, "value-changed", G_CALLBACK(Changed), panel->root, 0);
    panel->review = Button(panel, box, "Review request", "provider-chat.review", G_CALLBACK(Start));
    panel->details = Label(box, "No reviewed request.");
    panel->preview = View(panel, box, "Exact user message to be sent", "provider-chat.preview", false);
    Label(box, "Local profile password, required only for remote requests");
    panel->password = gtk_password_entry_new();
    (void)UmiGtk4RecordingSetPrivate(panel->password, 1);
    (void)umi_gtk4_automation_tag_widget(panel->password, "provider-chat.password");
    gtk_accessible_update_property(GTK_ACCESSIBLE(panel->password), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                   "Local profile password", -1);
    gtk_box_append(GTK_BOX(box), panel->password);
    g_signal_connect_object(panel->password, "changed", G_CALLBACK(PasswordChanged), panel->root, 0);
    panel->approve =
        gtk_check_button_new_with_label("I approve sending this reviewed text to the displayed destination");
    (void)umi_gtk4_automation_tag_widget(panel->approve, "provider-chat.approve");
    gtk_box_append(GTK_BOX(box), panel->approve);
    panel->send = Button(panel, box, "Send reviewed request once", "provider-chat.send", G_CALLBACK(Start));
    panel->cancel = Button(panel, box, "Cancel request", "provider-chat.cancel", G_CALLBACK(Cancel));
    panel->clear = Button(panel, box, "Clear local text", "provider-chat.clear", G_CALLBACK(Clear));
    panel->result =
        View(panel, box, "Provider reply — review as untrusted text", "provider-chat.result", false);
    UmiProviderChatGtkHistoryControls(panel, box);
    panel->discard = gtk_check_button_new_with_label("Discard this window's unsaved text when closing");
    gtk_box_append(GTK_BOX(box), panel->discard);
    panel->message = Label(
        box, "No request sent. Closing clears this window's text; no transcript is saved by this workflow.");
    UmiProviderChatGtkSensitivity(panel);
    *out = panel;
    return UMI_STATUS_OK;
}
GtkWidget *UmiProviderChatGtkWidget(UmiProviderChatGtk *panel)
{
    return panel != NULL && !panel->closed ? panel->root : NULL;
}
bool UmiProviderChatGtkBusy(const UmiProviderChatGtk *panel)
{
    return panel != NULL && !panel->closed && panel->busy;
}
bool UmiProviderChatGtkCanClose(UmiProviderChatGtk *panel)
{
    if (panel == NULL || panel->closed)
        return true;
    bool allowed =
        !panel->busy && (!panel->dirty || gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->discard)));
    if (!allowed)
        gtk_label_set_text(
            GTK_LABEL(panel->message),
            panel->busy
                ? "Cancel or wait for the current operation before closing."
                : "Clear local text or confirm discarding this window's unsaved text before closing.");
    return allowed;
}
void UmiProviderChatGtkDestroy(UmiProviderChatGtk *panel)
{
    if (panel == NULL || panel->closed)
        return;
    panel->closed = true;
    UmiProviderChatGtkHistoryRetire(panel);
    umi_cancellation_token_request(panel->token);
    Forget(panel);
    UmiProviderChatGtkText(panel->prompt, "");
    UmiProviderChatGtkText(panel->context, "");
    UmiProviderChatGtkText(panel->result, "");
    gtk_widget_set_sensitive(panel->root, FALSE);
    g_object_set_data(G_OBJECT(panel->root), "umicom-provider-chat", NULL);
    g_clear_object(&panel->root);
    UmiProviderChatGtkRelease(panel);
}

/* GTK must receive complete UTF-8. Validate before mutating a draft so rejected
 * input cannot discard an earlier review. Only ordinary multiline controls
 * are accepted, matching the shared request encoder. */
UmiStatus UmiProviderChatGtkContextValidate(const char *text)
{
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_PROVIDER_CHAT_CONTEXT_CAPACITY && text[length] != '\0') {
        unsigned char c = (unsigned char)text[length];
        if ((c < 0x20U && c != '\t' && c != '\r' && c != '\n') || c == 0x7fU)
            return UMI_STATUS_INVALID_ARGUMENT;
        ++length;
    }
    if (length == UMI_PROVIDER_CHAT_CONTEXT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    return g_utf8_validate(text, (gssize)length, NULL) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiProviderChatGtkSetContext(UmiProviderChatGtk *panel, const char *text)
{
    if (panel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (panel->closed) return UMI_STATUS_INVALID_STATE;
    if (panel->busy) return UMI_STATUS_BUSY;
    UmiStatus status = UmiProviderChatGtkContextValidate(text);
    if (status != UMI_STATUS_OK) return status;
    /* The existing change callback clears review credentials, but explicitly
     * do so as well for an unchanged or empty replacement. */
    Forget(panel);
    UmiProviderChatGtkText(panel->context, text);
    UmiProviderChatGtkText(panel->result, "");
    gtk_label_set_text(GTK_LABEL(panel->message), "Context copied. Enter a prompt, then review the complete request before sending.");
    panel->dirty = true;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->discard), FALSE);
    UmiProviderChatGtkSensitivity(panel);
    return UMI_STATUS_OK;
}
