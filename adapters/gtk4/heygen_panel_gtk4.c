/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/heygen_panel_gtk4.c
 * PURPOSE: Compose reviewed avatar generation without blocking the GTK thread.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media_generation/heygen_gtk4.h"
#include "umicom/security/gtk4/profile_keys.h"
#include "umicom/security/secrets.h"
#include <string.h>

#define PANEL_KEY "umicom-heygen-panel"
typedef struct HeyPanel
{
    char application[96], profile[97];
    GtkWidget *root, *form, *password, *alias, *name, *script, *motion, *avatar, *voice, *video;
    GtkWidget *private_list, *portrait, *subtitles, *approval, *acknowledge, *send, *cancel, *next, *choices;
    GtkWidget *review, *result, *message, *image_asset, *scene_seconds;
    UmiHeyGenOperation prepared_operation;
    UmiHeyGenPlan *plan;
    UmiCancellationToken *pending;
    UmiHeyGenResult catalogue;
    UmiHeyGenOperation catalogue_kind;
    bool busy, uncertain, painting;
} HeyPanel;
typedef struct HeyJob
{
    GWeakRef root;
    UmiHeyGenPlan *plan;
    UmiCancellationToken *cancel;
    UmiHeyGenResult result;
    UmiStatus status;
    UmiHeyGenOperation operation;
    char application[96], profile[97], password[257], alias[97];
} HeyJob;
static HeyPanel *Panel(gpointer root) { return g_object_get_data(G_OBJECT(root), PANEL_KEY); }
static void Message(HeyPanel *p, const char *text) { gtk_label_set_text(GTK_LABEL(p->message), text); }
static void Tag(GtkWidget *widget, const char *id)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free);
}
static GtkWidget *Label(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0);
    gtk_box_append(GTK_BOX(box), label);
    return label;
}
static GtkWidget *Entry(GtkWidget *box, const char *label, const char *id)
{
    Label(box, label);
    GtkWidget *entry = gtk_entry_new();
    Tag(entry, id);
    gtk_box_append(GTK_BOX(box), entry);
    return entry;
}
static GtkWidget *Check(GtkWidget *box, const char *label, const char *id)
{
    GtkWidget *check = gtk_check_button_new_with_label(label);
    Tag(check, id);
    gtk_box_append(GTK_BOX(box), check);
    return check;
}
static GtkWidget *TextView(GtkWidget *box, const char *id, bool editable)
{
    GtkWidget *view = gtk_text_view_new();
    Tag(view, id);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), editable);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scroll, -1, 120);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_box_append(GTK_BOX(box), scroll);
    return view;
}
static void Display(GtkWidget *view, const char *text)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)), text, -1);
}
static UmiStatus Copy(char *out, size_t capacity, const char *text)
{
    if (strlen(text) >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, text, strlen(text) + 1U);
    return UMI_STATUS_OK;
}
/* Editing request fields retires the encoded request and approval together. A later click must never submit a body that differs from the displayed review. */
static void Invalidate(HeyPanel *p)
{
    UmiHeyGenDestroy(p->plan);
    p->plan = NULL;
    gtk_widget_set_sensitive(p->send, FALSE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(p->approval), FALSE);
    Display(p->review, "");
}
static void Changed(GObject *object, gpointer root)
{
    (void)object;
    HeyPanel *p = Panel(root);
    if (p == NULL || p->painting)
        return;
    Invalidate(p);
    /* A pagination cursor belongs to the account alias and catalogue filter
     * that produced it. Never reuse it after either choice changes. */
    if (object == G_OBJECT(p->alias) || object == G_OBJECT(p->private_list))
    {
        memset(&p->catalogue, 0, sizeof(p->catalogue));
        gtk_drop_down_set_model(GTK_DROP_DOWN(p->choices), NULL);
        gtk_widget_set_sensitive(p->next, FALSE);
    }
}
/* The worker owns its captured plan and cancellation token independently of the panel. Its final release wipes the password, results and signed links. */
static void FreeJob(gpointer data)
{
    HeyJob *job = data;
    g_weak_ref_clear(&job->root);
    UmiHeyGenDestroy(job->plan);
    umi_cancellation_token_destroy(job->cancel);
    umi_secret_clear(job, sizeof(*job));
    g_free(job);
}
/* Closing the panel requests local cancellation without destroying resources still borrowed by a worker. Weak completion delivery prevents use of retired widgets. */
static void Release(gpointer data)
{
    HeyPanel *p = data;
    if (p->pending != NULL)
        umi_cancellation_token_request(p->pending);
    UmiHeyGenDestroy(p->plan);
    umi_secret_clear(&p->catalogue, sizeof(p->catalogue));
    g_free(p);
}
/* Open the local credential service on the worker thread, perform this one request, then close it. GTK widgets are accessed only by the completion callback. */
static void Worker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    HeyJob *job = data;
    UmiProfileSecrets *secrets = NULL;
    job->status = umi_cancellation_token_is_requested(job->cancel)
                      ? UMI_STATUS_CANCELLED
                      : UmiProfileSecretsPlatform(job->application, job->profile, &secrets);
    if (job->status == UMI_STATUS_OK)
        job->status = UmiHeyGenExecuteWithProfile(job->plan, true, secrets, job->password, job->alias,
                                                  job->cancel, &job->result);
    UmiProfileSecretsDestroy(secrets);
    umi_secret_clear(job->password, sizeof(job->password));
    g_task_return_boolean(task, TRUE);
}
/* Acquire the panel weak reference only for owner-thread delivery. If the panel has closed, the job still finishes and releases its own resources safely. */
static void Finished(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    (void)data;
    HeyJob *job = g_task_get_task_data(G_TASK(result));
    GError *error = NULL;
    (void)g_task_propagate_boolean(G_TASK(result), &error);
    if (error != NULL)
    {
        job->status = UMI_STATUS_INTERNAL_ERROR;
        g_clear_error(&error);
    }
    GObject *root = g_weak_ref_get(&job->root);
    if (root == NULL)
        return;
    HeyPanel *p = Panel(root);
    if (p != NULL)
    {
        p->busy = false;
        p->pending = NULL;
        gtk_widget_set_sensitive(p->form, TRUE);
        gtk_widget_set_sensitive(p->cancel, FALSE);
        p->uncertain = p->uncertain || (job->result.creation_may_exist && !job->result.creation_confirmed);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(p->acknowledge), FALSE);
        if (job->status == UMI_STATUS_OK)
        {
            if (job->operation == UMI_HEYGEN_LIST_AVATARS || job->operation == UMI_HEYGEN_LIST_VOICES)
            {
                p->catalogue = job->result;
                p->catalogue_kind = job->operation;
                GtkStringList *names = gtk_string_list_new(NULL);
                for (size_t i = 0U; i < p->catalogue.count; ++i)
                {
                    const UmiHeyGenItem *item = &p->catalogue.items[i];
                    char *line = g_strdup_printf("%s · %s%s%s", item->name, item->id,
                                                 item->status[0] ? " · " : "", item->status);
                    gtk_string_list_append(names, line);
                    g_free(line);
                }
                /* set_model borrows; the drop-down retains its own reference. */
                gtk_drop_down_set_model(GTK_DROP_DOWN(p->choices), G_LIST_MODEL(names));
                g_object_unref(names);
                gtk_drop_down_set_selected(GTK_DROP_DOWN(p->choices),
                                           p->catalogue.count ? 0U : GTK_INVALID_LIST_POSITION);
                gtk_widget_set_sensitive(p->next, p->catalogue.has_more);
                Message(p, p->catalogue.count ? "Select a result, then choose Use selected item. More pages "
                                                "are fetched only on request."
                                              : "No matching items in this account catalogue.");
            }
            else
            {
                char *text = g_strdup_printf(
                    "Resource: %s\nState: %s\n%s%s%s%s", job->result.resource_id, job->result.state,
                    job->result.video_url[0] ? "Temporary download URL (keep private):\n" : "",
                    job->result.video_url,
                    job->result.subtitle_url[0] ? "\nTemporary subtitle URL (keep private):\n" : "",
                    job->result.subtitle_url);
                Display(p->result, text);
                g_free(text);
                if (job->operation == UMI_HEYGEN_CREATE_AVATAR)
                    gtk_editable_set_text(GTK_EDITABLE(p->avatar), job->result.resource_id);
                if (job->operation == UMI_HEYGEN_CREATE_VIDEO || job->operation == UMI_HEYGEN_CREATE_SCENE)
                    gtk_editable_set_text(GTK_EDITABLE(p->video), job->result.resource_id);
                Message(p, job->result.creation_confirmed
                               ? "HeyGen accepted the request. Keep the resource ID. Check status "
                                 "explicitly; closing this page does not cancel rendering."
                               : "Status received. Copy a completed video's private URL if you want to "
                                 "download it in your browser.");
            }
        }
        else
        {
            char *text = g_strdup_printf(
                "%s (HTTP %u). %s", umi_status_text(job->status), job->result.http_status,
                p->uncertain ? "A remote creation may exist. Check your HeyGen dashboard before submitting "
                               "another creation."
                             : "Review credentials, permissions and fields before trying again.");
            Message(p, text);
            g_free(text);
        }
    }
    g_object_unref(root);
}
/* Capture credentials and namespace before disabling the form. Later edits cannot change the destination account of a request that is already running. */
static void Start(HeyPanel *p, UmiHeyGenPlan *plan, UmiHeyGenOperation operation)
{
    HeyJob *job = g_new0(HeyJob, 1);
    g_weak_ref_init(&job->root, G_OBJECT(p->root));
    job->plan = plan;
    job->operation = operation;
    strcpy(job->application, p->application);
    strcpy(job->profile, p->profile);
    UmiStatus status =
        Copy(job->password, sizeof(job->password), gtk_editable_get_text(GTK_EDITABLE(p->password)));
    if (status == UMI_STATUS_OK)
        status = Copy(job->alias, sizeof(job->alias), gtk_editable_get_text(GTK_EDITABLE(p->alias)));
    if (status == UMI_STATUS_OK)
        status = umi_cancellation_token_create(&job->cancel);
    p->painting = true;
    gtk_editable_set_text(GTK_EDITABLE(p->password), "");
    p->painting = false;
    if (status != UMI_STATUS_OK)
    {
        FreeJob(job);
        Message(p, umi_status_text(status));
        return;
    }
    p->busy = true;
    p->pending = job->cancel;
    gtk_widget_set_sensitive(p->form, FALSE);
    gtk_widget_set_sensitive(p->cancel, TRUE);
    Message(p, "Contacting HeyGen with this request…");
    GTask *task = g_task_new(NULL, NULL, Finished, NULL);
    g_task_set_task_data(task, job, FreeJob);
    g_task_run_in_thread(task, Worker);
    g_object_unref(task);
}
/* Translate explicit UI actions into shared request operations. Catalogue reads start directly; every resource creation must pass a separate review and approval. */
static void Action(GtkButton *button, gpointer root)
{
    HeyPanel *p = Panel(root);
    if (p == NULL)
        return;
    const char *action = g_object_get_data(G_OBJECT(button), "heygen-action");
    if (strcmp(action, "cancel") == 0)
    {
        if (p->pending != NULL)
            umi_cancellation_token_request(p->pending);
        Message(p, "Stopping the local request. Any remote creation may continue; wait for its outcome.");
        return;
    }
    if (p->busy)
        return;
    if (strcmp(action, "keys") == 0)
    {
        /* The manager can replace a credential under the same alias. Retire
         * account-specific catalogue cursors and any prior approval first. */
        Invalidate(p);
        memset(&p->catalogue, 0, sizeof(p->catalogue));
        gtk_drop_down_set_model(GTK_DROP_DOWN(p->choices), NULL);
        gtk_widget_set_sensitive(p->next, FALSE);
        GtkRoot *owner = gtk_widget_get_root(p->root);
        UmiStatus status = UmiProfileKeysGtkPresent(GTK_IS_WINDOW(owner) ? GTK_WINDOW(owner) : NULL,
                                                    p->application, p->profile);
        if (status != UMI_STATUS_OK)
            Message(p, umi_status_text(status));
        return;
    }
    if (strcmp(action, "use") == 0)
    {
        guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(p->choices));
        if ((size_t)index >= p->catalogue.count)
        {
            Message(p, "Select a catalogue item first.");
            return;
        }
        const UmiHeyGenItem *item = &p->catalogue.items[index];
        if (p->catalogue_kind == UMI_HEYGEN_LIST_AVATARS && strcmp(item->status, "completed") != 0)
        {
            Message(p, "This avatar is not ready. Check its status before using it.");
            return;
        }
        gtk_editable_set_text(
            GTK_EDITABLE(p->catalogue_kind == UMI_HEYGEN_LIST_AVATARS ? p->avatar : p->voice), item->id);
        return;
    }
    if (strcmp(action, "send") == 0)
    {
        if (p->plan == NULL || !gtk_check_button_get_active(GTK_CHECK_BUTTON(p->approval)))
        {
            Message(p, "Review the request and approve that exact creation first.");
            return;
        }
        UmiHeyGenPlan *plan = p->plan;
        p->plan = NULL;
        UmiHeyGenOperation op = p->prepared_operation;
        Invalidate(p);
        Start(p, plan, op);
        return;
    }
    UmiHeyGenDraft *draft = g_new0(UmiHeyGenDraft, 1);
    UmiStatus status = UMI_STATUS_OK;
    bool create = strcmp(action, "prepare-avatar") == 0 || strcmp(action, "prepare-video") == 0 ||
                  strcmp(action, "prepare-scene") == 0;
    if (create)
    {
        if (p->uncertain && !gtk_check_button_get_active(GTK_CHECK_BUTTON(p->acknowledge)))
        {
            g_free(draft);
            Message(p, "First check HeyGen for the earlier uncertain creation, then acknowledge that check.");
            return;
        }
        p->uncertain = false;
        draft->operation = strcmp(action, "prepare-avatar") == 0  ? UMI_HEYGEN_CREATE_AVATAR
                           : strcmp(action, "prepare-scene") == 0 ? UMI_HEYGEN_CREATE_SCENE
                                                                  : UMI_HEYGEN_CREATE_VIDEO;
        char *uuid = g_uuid_string_random();
        (void)Copy(draft->request_key, sizeof(draft->request_key), uuid);
        g_free(uuid);
        if (draft->operation != UMI_HEYGEN_CREATE_SCENE)
            status = Copy(draft->name, sizeof(draft->name), gtk_editable_get_text(GTK_EDITABLE(p->name)));
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->script));
        GtkTextIter first, last;
        gtk_text_buffer_get_bounds(buffer, &first, &last);
        char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
        if (status == UMI_STATUS_OK)
            status = Copy(draft->script, sizeof(draft->script), text);
        umi_secret_clear(text, strlen(text));
        g_free(text);
        draft->portrait = gtk_check_button_get_active(GTK_CHECK_BUTTON(p->portrait));
        if (draft->operation == UMI_HEYGEN_CREATE_SCENE)
        {
            draft->scene_seconds =
                (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(p->scene_seconds));
            if (status == UMI_STATUS_OK)
                status = Copy(draft->image_asset_id, sizeof(draft->image_asset_id),
                              gtk_editable_get_text(GTK_EDITABLE(p->image_asset)));
        }
        if (draft->operation == UMI_HEYGEN_CREATE_VIDEO)
        {
            if (status == UMI_STATUS_OK)
                status = Copy(draft->resource_id, sizeof(draft->resource_id),
                              gtk_editable_get_text(GTK_EDITABLE(p->avatar)));
            if (status == UMI_STATUS_OK)
                status = Copy(draft->voice_id, sizeof(draft->voice_id),
                              gtk_editable_get_text(GTK_EDITABLE(p->voice)));
            if (status == UMI_STATUS_OK)
                status = Copy(draft->motion, sizeof(draft->motion),
                              gtk_editable_get_text(GTK_EDITABLE(p->motion)));
            draft->subtitles = gtk_check_button_get_active(GTK_CHECK_BUTTON(p->subtitles));
        }
    }
    else if (strcmp(action, "avatar-status") == 0 || strcmp(action, "video-status") == 0)
    {
        draft->operation =
            strcmp(action, "avatar-status") == 0 ? UMI_HEYGEN_GET_AVATAR : UMI_HEYGEN_GET_VIDEO;
        status = Copy(draft->resource_id, sizeof(draft->resource_id),
                      gtk_editable_get_text(
                          GTK_EDITABLE(draft->operation == UMI_HEYGEN_GET_AVATAR ? p->avatar : p->video)));
    }
    else
    {
        draft->operation = strcmp(action, "next") == 0      ? p->catalogue_kind
                           : strcmp(action, "avatars") == 0 ? UMI_HEYGEN_LIST_AVATARS
                                                            : UMI_HEYGEN_LIST_VOICES;
        draft->private_catalogue = gtk_check_button_get_active(GTK_CHECK_BUTTON(p->private_list));
        if (strcmp(action, "next") == 0)
        {
            if (!p->catalogue.has_more)
                status = UMI_STATUS_INVALID_STATE;
            else
                strcpy(draft->cursor, p->catalogue.next_cursor);
        }
    }
    UmiHeyGenPlan *plan = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiHeyGenPrepare(draft, &plan);
    UmiHeyGenOperation operation = draft->operation;
    umi_secret_clear(draft, sizeof(*draft));
    g_free(draft);
    if (status != UMI_STATUS_OK)
    {
        Message(p, umi_status_text(status));
        return;
    }
    Invalidate(p);
    if (create)
    {
        p->plan = plan;
        p->prepared_operation = operation;
        const char *url, *body;
        bool creates;
        (void)UmiHeyGenReview(plan, &url, &body, &creates);
        char *text = g_strdup_printf("Local key alias: %s\nProfile: %s / %s\nPOST %s\n\n%s",
                                     gtk_editable_get_text(GTK_EDITABLE(p->alias)), p->application,
                                     p->profile, url, body);
        Display(p->review, text);
        g_free(text);
        gtk_widget_set_sensitive(p->send, TRUE);
        Message(p, "Review the exact request below. HeyGen may charge for creation. Approve, then send; any "
                   "field edit requires another review.");
    }
    else
        Start(p, plan, operation);
}
/* Bind callbacks to the root object so a retained button cannot call a released panel. Stable automation tags also let fixtures address controls by purpose. */
static GtkWidget *Button(HeyPanel *p, GtkWidget *box, const char *label, const char *action)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    char *tag = g_strdup_printf("heygen.%s", action);
    Tag(button, tag);
    g_free(tag);
    g_object_set_data_full(G_OBJECT(button), "heygen-action", g_strdup(action), g_free);
    g_signal_connect_object(button, "clicked", G_CALLBACK(Action), G_OBJECT(p->root), 0);
    gtk_box_append(GTK_BOX(box), button);
    return button;
}
/* Construction is a composition step, with no provider or credential-store access. Applications supply a namespace; Framework owns the request form and lifecycle. */
UmiStatus UmiHeyGenGtkCreate(const char *application, const char *profile, GtkWidget **out)
{
    if (out == NULL || *out != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiProfileSecretsScopeValidate(application, profile);
    if (status != UMI_STATUS_OK)
        return status;
    HeyPanel *p = g_new0(HeyPanel, 1);
    if (Copy(p->application, sizeof(p->application), application) != UMI_STATUS_OK ||
        Copy(p->profile, sizeof(p->profile), profile) != UMI_STATUS_OK)
    {
        g_free(p);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    p->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    Tag(p->root, "heygen.panel");
    g_object_set_data_full(G_OBJECT(p->root), PANEL_KEY, p, Release);
    Label(p->root, "HeyGen avatar videos · requests go to api.heygen.com only when you choose an action. "
                   "Save your API key locally using Manage keys; it is never stored in project files.");
    p->form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_append(GTK_BOX(p->root), p->form);
    Button(p, p->form, "Manage local profile and API keys", "keys");
    p->alias = Entry(p->form, "Saved key alias", "heygen.alias");
    gtk_editable_set_text(GTK_EDITABLE(p->alias), "heygen");
    p->password = Entry(p->form, "Local profile password (cleared after each request)", "heygen.password");
    gtk_entry_set_visibility(GTK_ENTRY(p->password), FALSE);
    p->private_list = Check(p->form, "Browse my private avatars or voices", "heygen.private");
    Button(p, p->form, "Load avatars", "avatars");
    Button(p, p->form, "Load voices", "voices");
    p->choices = gtk_drop_down_new_from_strings((const char *const[]){NULL});
    Tag(p->choices, "heygen.catalogue");
    gtk_box_append(GTK_BOX(p->form), p->choices);
    Button(p, p->form, "Use selected item", "use");
    p->next = Button(p, p->form, "Load next catalogue page", "next");
    gtk_widget_set_sensitive(p->next, FALSE);
    p->avatar = Entry(p->form, "Avatar look ID", "heygen.avatar");
    Button(p, p->form, "Check avatar status", "avatar-status");
    p->voice = Entry(p->form, "Voice ID", "heygen.voice");
    p->name = Entry(p->form, "Video title or new character name", "heygen.name");
    Label(p->form, "Script for a video, or description for a new synthetic character (1000 bytes for a "
                   "character, 8192 for a script)");
    p->script = TextView(p->form, "heygen.script", true);
    p->motion = Entry(p->form, "Motion direction for a video (optional)", "heygen.motion");
    p->portrait = Check(p->form, "Portrait output", "heygen.portrait");
    p->subtitles = Check(p->form, "Request subtitle sidecar", "heygen.subtitles");
    p->acknowledge =
        Check(p->form, "I checked HeyGen for an earlier uncertain creation before preparing another",
              "heygen.acknowledge");
    Button(p, p->form, "Review new character request", "prepare-avatar");
    Button(p, p->form, "Review avatar video request", "prepare-video");
    Label(p->form, "Short scenes: describe a character's action in the script box. Optionally use an image "
                   "asset already uploaded to the same HeyGen workspace. This page does not upload local "
                   "images. Image scenes follow the source shape; turn Portrait off. Scenes use 768p and "
                   "ignore avatar, voice, title, motion and subtitle fields.");
    p->image_asset =
        Entry(p->form, "Existing HeyGen image asset ID (blank for a scene from text)", "heygen.image-asset");
    Label(p->form, "Scene duration in seconds");
    p->scene_seconds = gtk_spin_button_new_with_range(5.0, 15.0, 1.0);
    Tag(p->scene_seconds, "heygen.scene-seconds");
    gtk_box_append(GTK_BOX(p->form), p->scene_seconds);
    Button(p, p->form, "Review short scene request", "prepare-scene");
    p->review = TextView(p->form, "heygen.review", false);
    p->approval =
        Check(p->form, "I approve this displayed request and any HeyGen usage charge", "heygen.approval");
    p->send = Button(p, p->form, "Send reviewed creation", "send");
    gtk_widget_set_sensitive(p->send, FALSE);
    p->video = Entry(p->form, "Video ID (keep this to check later)", "heygen.video");
    Button(p, p->form, "Check video status", "video-status");
    p->cancel = Button(p, p->root, "Stop local request", "cancel");
    gtk_widget_set_sensitive(p->cancel, FALSE);
    p->result = TextView(p->root, "heygen.result", false);
    p->message = Label(p->root, UmiHeyGenHttpAvailable()
                                    ? "No request has been sent. Set up a local profile and key to begin."
                                    : "HeyGen HTTP transport is unavailable in this build.");
    Tag(p->message, "heygen.message");
    GtkWidget *entries[] = {p->alias, p->password, p->avatar, p->voice,
                            p->name,  p->motion,   p->video,  p->image_asset};
    for (size_t i = 0U; i < G_N_ELEMENTS(entries); ++i)
        g_signal_connect_object(entries[i], "changed", G_CALLBACK(Changed), G_OBJECT(p->root), 0);
    GtkWidget *checks[] = {p->private_list, p->portrait, p->subtitles};
    for (size_t i = 0U; i < G_N_ELEMENTS(checks); ++i)
        g_signal_connect_object(checks[i], "toggled", G_CALLBACK(Changed), G_OBJECT(p->root), 0);
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->script)), "changed",
                            G_CALLBACK(Changed), G_OBJECT(p->root), 0);
    g_signal_connect_object(p->scene_seconds, "value-changed", G_CALLBACK(Changed), G_OBJECT(p->root), 0);
    *out = p->root;
    return UMI_STATUS_OK;
}
