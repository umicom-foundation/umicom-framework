/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/pixverse_panel_gtk4.c
 * PURPOSE: Run reviewed video requests with local keys and explicit progress checks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media_generation/pixverse_gtk4.h"
#include "umicom/security/gtk4/profile_keys.h"
#include "umicom/security/secrets.h"
#include <inttypes.h>
#include <string.h>
#define PANEL_KEY "umicom-pixverse-panel"
typedef struct VideoPanel
{
    char application[96], profile[97];
    GtkWidget *root, *form, *alias, *password, *prompt, *ratio, *seconds, *quality, *audio, *shots;
    GtkWidget *identity, *review, *receipt, *approval, *acknowledge, *send, *stop, *message;
    UmiPixVersePlan *plan;
    UmiCancellationToken *pending;
    bool busy, painting, uncertain;
} VideoPanel;
typedef struct VideoJob
{
    GWeakRef root;
    char application[96], profile[97], alias[97], password[257];
    UmiPixVersePlan *plan;
    UmiPixVerseResult result;
    UmiCancellationToken *cancel;
    UmiStatus status;
    bool creating;
} VideoJob;
static VideoPanel *Panel(gpointer root) { return g_object_get_data(G_OBJECT(root), PANEL_KEY); }
static void Tag(GtkWidget *widget, const char *id)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free);
}
static GtkWidget *Label(GtkWidget *box, const char *text)
{
    GtkWidget *widget = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(widget), TRUE);
    gtk_label_set_xalign(GTK_LABEL(widget), 0);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}
static void Message(VideoPanel *panel, const char *text)
{
    gtk_label_set_text(GTK_LABEL(panel->message), text);
}
static GtkWidget *Entry(GtkWidget *box, const char *label, const char *id)
{
    Label(box, label);
    GtkWidget *widget = gtk_entry_new();
    Tag(widget, id);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}
static GtkWidget *Check(GtkWidget *box, const char *label, const char *id)
{
    GtkWidget *widget = gtk_check_button_new_with_label(label);
    Tag(widget, id);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}
static GtkWidget *View(GtkWidget *box, const char *id, bool editable)
{
    GtkWidget *widget = gtk_text_view_new(), *scroll = gtk_scrolled_window_new();
    Tag(widget, id);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(widget), editable);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(widget), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_size_request(scroll, -1, 130);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), widget);
    gtk_box_append(GTK_BOX(box), scroll);
    return widget;
}
static void Display(GtkWidget *widget, const char *value)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget)), value, -1);
}
static UmiStatus Copy(char *out, size_t capacity, const char *value)
{
    if (strlen(value) >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, value, strlen(value) + 1U);
    return UMI_STATUS_OK;
}

/* A field edit revokes the old approval. The receipt remains visible so a user
 * can check an existing job without losing its ID while composing another. */
static void Invalidate(VideoPanel *panel)
{
    UmiPixVerseDestroy(panel->plan);
    panel->plan = NULL;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
    gtk_widget_set_sensitive(panel->send, FALSE);
    Display(panel->review, "");
}
static void Changed(GObject *unused, gpointer root)
{
    (void)unused;
    VideoPanel *panel = Panel(root);
    if (panel && !panel->painting)
        Invalidate(panel);
}
/* The worker owns the request, password and cancellation token independently of the panel. Release them only after completion delivery ends. */
static void FreeJob(gpointer data)
{
    VideoJob *job = data;
    g_weak_ref_clear(&job->root);
    UmiPixVerseDestroy(job->plan);
    umi_cancellation_token_destroy(job->cancel);
    umi_secret_clear(job, sizeof(*job));
    g_free(job);
}
/* Closing the panel requests cooperative cancellation without waiting on the UI thread or assuming the remote job was cancelled. */
static void Release(gpointer data)
{
    VideoPanel *panel = data;
    if (panel->pending)
        umi_cancellation_token_request(panel->pending);
    UmiPixVerseDestroy(panel->plan);
    g_free(panel);
}
/* Open the local profile only inside the worker and for the selected application namespace. No GTK widget or key is retained by the provider owner. */
static void Worker(GTask *task, gpointer source, gpointer data, GCancellable *unused)
{
    (void)source;
    (void)unused;
    VideoJob *job = data;
    UmiProfileSecrets *secrets = NULL;
    job->status = umi_cancellation_token_is_requested(job->cancel)
                      ? UMI_STATUS_CANCELLED
                      : UmiProfileSecretsPlatform(job->application, job->profile, &secrets);
    if (job->status == UMI_STATUS_OK)
        job->status = UmiPixVerseExecuteWithProfile(job->plan, true, secrets, job->password, job->alias,
                                                    job->cancel, &job->result);
    UmiProfileSecretsDestroy(secrets);
    umi_secret_clear(job->password, sizeof(job->password));
    g_task_return_boolean(task, TRUE);
}
/* The main thread only displays the worker's owned receipt. It never follows
 * a signed URL, polls automatically or sends another billable request. */
static void Finished(GObject *source, GAsyncResult *result, gpointer unused)
{
    (void)source;
    (void)unused;
    VideoJob *job = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GObject *root = g_weak_ref_get(&job->root);
    if (!root)
        return;
    VideoPanel *panel = Panel(root);
    if (panel)
    {
        panel->busy = false;
        panel->pending = NULL;
        gtk_widget_set_sensitive(panel->form, TRUE);
        gtk_widget_set_sensitive(panel->stop, FALSE);
        if (job->status == UMI_STATUS_OK)
        {
            char identity[32];
            g_snprintf(identity, sizeof(identity), "%" PRId64, job->result.video_id);
            gtk_editable_set_text(GTK_EDITABLE(panel->identity), identity);
            const char *state = job->creating            ? "Accepted"
                                : job->result.state == 1 ? "Ready"
                                : job->result.state == 5 ? "Generating"
                                : job->result.state == 6 ? "Deleted"
                                : job->result.state == 7 ? "Moderation failed"
                                                         : "Generation failed";
            char *text = g_strdup_printf(
                "Video ID: %s\nState: %s\n%s%s\nCopy the ID before closing this page.", identity, state,
                job->result.video_url[0] ? "Private result URL: " : "", job->result.video_url);
            Display(panel->receipt, text);
            g_free(text);
            Message(panel, job->creating
                               ? "Job accepted. Use Review status check when you want to refresh progress."
                               : "Status received. No automatic polling or download was started.");
        }
        else
        {
            panel->uncertain = panel->uncertain || job->result.creation_may_exist;
            gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->acknowledge), FALSE);
            char *text = g_strdup_printf(
                "%s (HTTP %u). %s", umi_status_text(job->status), job->result.http_status,
                panel->uncertain
                    ? "A job may exist and may be charged. Check PixVerse before creating another."
                    : "Check the saved key, local password, account access and request fields.");
            Message(panel, text);
            g_free(text);
        }
    }
    g_object_unref(root);
}
/* Copy credentials into a single worker job, clear the visible password and transfer request ownership before scheduling work. */
static void Start(VideoPanel *panel)
{
    VideoJob *job = g_new0(VideoJob, 1);
    g_weak_ref_init(&job->root, G_OBJECT(panel->root));
    strcpy(job->application, panel->application);
    strcpy(job->profile, panel->profile);
    UmiStatus status = umi_cancellation_token_create(&job->cancel);
    if (status == UMI_STATUS_OK)
        status = Copy(job->alias, sizeof(job->alias), gtk_editable_get_text(GTK_EDITABLE(panel->alias)));
    if (status == UMI_STATUS_OK)
        status =
            Copy(job->password, sizeof(job->password), gtk_editable_get_text(GTK_EDITABLE(panel->password)));
    const char *url = NULL, *body = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiPixVerseReview(panel->plan, &url, &body, &job->creating);
    panel->painting = true;
    gtk_editable_set_text(GTK_EDITABLE(panel->password), "");
    panel->painting = false;
    if (status != UMI_STATUS_OK)
    {
        FreeJob(job);
        Invalidate(panel);
        Message(panel, umi_status_text(status));
        return;
    }
    job->plan = panel->plan;
    panel->plan = NULL;
    Invalidate(panel);
    panel->busy = true;
    panel->pending = job->cancel;
    gtk_widget_set_sensitive(panel->form, FALSE);
    gtk_widget_set_sensitive(panel->stop, TRUE);
    Message(panel, job->creating ? "Submitting one video request…" : "Checking this video once…");
    GTask *task = g_task_new(NULL, NULL, Finished, NULL);
    g_task_set_task_data(task, job, FreeJob);
    g_task_run_in_thread(task, Worker);
    g_object_unref(task);
}
/* Parse the complete decimal job ID with overflow checks. Reject path fragments and alternate spellings before creating a status request. */
static UmiStatus Identity(const char *text, int64_t *out)
{
    if (!text[0] || text[0] == '0')
        return UMI_STATUS_INVALID_ARGUMENT;
    int64_t value = 0;
    for (size_t index = 0; text[index]; ++index)
    {
        if (text[index] < '0' || text[index] > '9')
            return UMI_STATUS_INVALID_ARGUMENT;
        int64_t digit = text[index] - '0';
        if (value > (INT64_MAX - digit) / 10)
            return UMI_STATUS_INVALID_ARGUMENT;
        value = value * 10 + digit;
    }
    *out = value;
    return UMI_STATUS_OK;
}
/* Separate preparation, approval and submission. Each new field or action must preserve the rule that viewing or editing a draft performs no network I/O. */
static void Action(GtkButton *button, gpointer root)
{
    VideoPanel *panel = Panel(root);
    if (!panel)
        return;
    const char *action = g_object_get_data(G_OBJECT(button), "pixverse-action");
    if (!strcmp(action, "stop"))
    {
        if (panel->pending)
            umi_cancellation_token_request(panel->pending);
        Message(panel, "Stopping local waiting does not cancel a remote job or provider charge.");
        return;
    }
    if (panel->busy)
        return;
    if (!strcmp(action, "keys"))
    {
        Invalidate(panel);
        GtkRoot *owner = gtk_widget_get_root(panel->root);
        UmiStatus status = UmiProfileKeysGtkPresent(GTK_IS_WINDOW(owner) ? GTK_WINDOW(owner) : NULL,
                                                    panel->application, panel->profile);
        if (status != UMI_STATUS_OK)
            Message(panel, umi_status_text(status));
        return;
    }
    if (!strcmp(action, "send"))
    {
        if (!panel->plan || !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approval)))
        {
            Message(panel, "Review and approve this request first.");
            return;
        }
        if (!UmiPixVerseHttpAvailable())
        {
            Message(panel, "This build needs the native HTTP adapter.");
            return;
        }
        Start(panel);
        return;
    }
    Invalidate(panel);
    bool creating = !strcmp(action, "prepare");
    if (creating && panel->uncertain && !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->acknowledge)))
    {
        Message(panel, "Check PixVerse for the uncertain creation and acknowledge the check first.");
        return;
    }
    UmiPixVerseDraft *draft = g_new0(UmiPixVerseDraft, 1);
    char *trace = g_uuid_string_random();
    strcpy(draft->trace_id, trace);
    g_free(trace);
    draft->operation = creating ? UMI_PIXVERSE_CREATE_VIDEO : UMI_PIXVERSE_GET_VIDEO;
    UmiStatus status = UMI_STATUS_OK;
    if (creating)
    {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->prompt));
        GtkTextIter first, last;
        gtk_text_buffer_get_bounds(buffer, &first, &last);
        char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
        status = Copy(draft->prompt, sizeof(draft->prompt), text);
        umi_secret_clear(text, strlen(text));
        g_free(text);
        if (status == UMI_STATUS_OK)
            status = Copy(draft->aspect_ratio, sizeof(draft->aspect_ratio),
                          gtk_editable_get_text(GTK_EDITABLE(panel->ratio)));
        draft->seconds = (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->seconds));
        draft->quality = (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->quality));
        draft->audio = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->audio));
        draft->multiple_shots = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->shots));
    }
    else
        status = Identity(gtk_editable_get_text(GTK_EDITABLE(panel->identity)), &draft->video_id);
    if (status == UMI_STATUS_OK)
        status = UmiPixVersePrepare(draft, &panel->plan);
    umi_secret_clear(draft, sizeof(*draft));
    g_free(draft);
    if (status != UMI_STATUS_OK)
    {
        Message(panel, umi_status_text(status));
        return;
    }
    const char *url = NULL, *body = NULL;
    bool sends = false;
    status = UmiPixVerseReview(panel->plan, &url, &body, &sends);
    if (status != UMI_STATUS_OK)
    {
        Invalidate(panel);
        Message(panel, umi_status_text(status));
        return;
    }
    char *text = g_strdup_printf("%s %s\nLocal profile: %s / %s\nKey alias: %s\n%s\n%s",
                                 sends ? "POST" : "GET", url, panel->application, panel->profile,
                                 gtk_editable_get_text(GTK_EDITABLE(panel->alias)), body,
                                 sends ? "Creating a video spends provider API credits."
                                       : "Read status once; the request uses a fresh trace ID.");
    Display(panel->review, text);
    g_free(text);
    if (creating)
    {
        panel->uncertain = false;
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->acknowledge), FALSE);
    }
    gtk_widget_set_sensitive(panel->send, TRUE);
    Message(panel, "Review ready. Nothing has been sent.");
}
static GtkWidget *Button(VideoPanel *panel, GtkWidget *box, const char *label, const char *action)
{
    GtkWidget *widget = gtk_button_new_with_label(label);
    char *id = g_strdup_printf("pixverse.%s", action);
    Tag(widget, id);
    g_free(id);
    g_object_set_data_full(G_OBJECT(widget), "pixverse-action", g_strdup(action), g_free);
    g_signal_connect_object(widget, "clicked", G_CALLBACK(Action), G_OBJECT(panel->root), 0);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}
static GtkWidget *Spin(VideoPanel *panel, const char *label, const char *id, double low, double high,
                       double initial)
{
    Label(panel->form, label);
    GtkWidget *widget = gtk_spin_button_new_with_range(low, high, 1);
    Tag(widget, id);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), initial);
    gtk_box_append(GTK_BOX(panel->form), widget);
    return widget;
}
/* Compose controls around the portable owner. Applications supply only their local profile namespace; they do not implement provider request logic. */
UmiStatus UmiPixVerseGtkCreate(const char *application, const char *profile, GtkWidget **out)
{
    if (!out || *out)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiProfileSecretsScopeValidate(application, profile);
    if (status != UMI_STATUS_OK)
        return status;
    VideoPanel *panel = g_new0(VideoPanel, 1);
    strcpy(panel->application, application);
    strcpy(panel->profile, profile);
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    Tag(panel->root, "pixverse.panel");
    g_object_set_data_full(G_OBJECT(panel->root), PANEL_KEY, panel, Release);
    Label(panel->root, "PixVerse video · text to video, optional generated audio and multiple shots. API "
                       "credits are separate from web membership.");
    panel->form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_append(GTK_BOX(panel->root), panel->form);
    Button(panel, panel->form, "Manage local profile and API keys", "keys");
    panel->alias = Entry(panel->form, "Saved key alias", "pixverse.alias");
    gtk_editable_set_text(GTK_EDITABLE(panel->alias), "pixverse");
    panel->password = Entry(panel->form, "Local profile password (cleared after send)", "pixverse.password");
    gtk_entry_set_visibility(GTK_ENTRY(panel->password), FALSE);
    Label(panel->form, "Prompt: describe subject, movement, lighting and scene (up to 5000 UTF-8 bytes)");
    panel->prompt = View(panel->form, "pixverse.prompt", true);
    panel->ratio =
        Entry(panel->form, "Aspect ratio: 16:9, 4:3, 1:1, 3:4, 9:16, 2:3, 3:2 or 21:9", "pixverse.ratio");
    gtk_editable_set_text(GTK_EDITABLE(panel->ratio), "16:9");
    panel->seconds = Spin(panel, "Duration in seconds", "pixverse.seconds", 1, 15, 5);
    panel->quality =
        Spin(panel, "Resolution: enter 360, 540, 720 or 1080", "pixverse.quality", 360, 1080, 720);
    panel->audio = Check(panel->form, "Generate audio", "pixverse.audio");
    panel->shots = Check(panel->form, "Generate multiple shots", "pixverse.shots");
    panel->acknowledge =
        Check(panel->form, "I checked PixVerse after the uncertain creation", "pixverse.acknowledge");
    Button(panel, panel->form, "Review new video request", "prepare");
    panel->identity = Entry(panel->form, "Video ID (from your receipt)", "pixverse.identity");
    Button(panel, panel->form, "Review status check", "status");
    panel->review = View(panel->form, "pixverse.review", false);
    panel->approval =
        Check(panel->form, "I approve this request and any provider charge", "pixverse.approval");
    panel->send = Button(panel, panel->form, "Send reviewed request", "send");
    gtk_widget_set_sensitive(panel->send, FALSE);
    panel->receipt = View(panel->form, "pixverse.receipt", false);
    panel->stop = Button(panel, panel->root, "Stop local waiting", "stop");
    gtk_widget_set_sensitive(panel->stop, FALSE);
    panel->message = Label(
        panel->root, "No request sent. Copy receipts before leaving; they are not saved automatically.");
    Tag(panel->message, "pixverse.message");
    GtkWidget *entries[] = {panel->alias, panel->password, panel->ratio, panel->identity};
    for (size_t index = 0; index < G_N_ELEMENTS(entries); ++index)
        g_signal_connect_object(entries[index], "changed", G_CALLBACK(Changed), G_OBJECT(panel->root), 0);
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->prompt)), "changed",
                            G_CALLBACK(Changed), G_OBJECT(panel->root), 0);
    g_signal_connect_object(panel->seconds, "value-changed", G_CALLBACK(Changed), G_OBJECT(panel->root), 0);
    g_signal_connect_object(panel->quality, "value-changed", G_CALLBACK(Changed), G_OBJECT(panel->root), 0);
    g_signal_connect_object(panel->audio, "toggled", G_CALLBACK(Changed), G_OBJECT(panel->root), 0);
    g_signal_connect_object(panel->shots, "toggled", G_CALLBACK(Changed), G_OBJECT(panel->root), 0);
    *out = panel->root;
    return UMI_STATUS_OK;
}
