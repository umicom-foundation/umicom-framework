/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/song_plan_gtk4.c
 * PURPOSE: Edit local lyric timing and save portable music-video plans.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/song_plan_gtk4.h"
#include "umicom/security/secrets.h"
#include <string.h>
#define PANEL_KEY "umicom-song-plan-panel"
typedef struct SongPanel
{
    GtkWidget *root, *form, *title, *style, *lyrics, *duration, *tempo, *beats, *offset, *review, *path;
    GtkWidget *approval, *save, *captions, *stop, *message;
    UmiSongPlan *plan;
    UmiCancellationToken *pending;
    bool busy, painting;
} SongPanel;
typedef struct SongJob
{
    GWeakRef root;
    char *path;
    UmiSongPlan *plan;
    UmiCreativeAsset *asset;
    UmiCreativeAssetWriteResult written;
    UmiCancellationToken *cancel;
    UmiStatus status;
    bool opening;
} SongJob;
static SongPanel *Panel(gpointer root) { return g_object_get_data(G_OBJECT(root), PANEL_KEY); }
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
static void Message(SongPanel *panel, const char *text)
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

/* An edited form no longer represents the prepared plan. Disable exports until its new timing has been validated. */
static void Invalidate(SongPanel *panel)
{
    UmiSongPlanDestroy(panel->plan);
    panel->plan = NULL;
    gtk_widget_set_sensitive(panel->save, FALSE);
    gtk_widget_set_sensitive(panel->captions, FALSE);
    Display(panel->review, "");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
}
static void Changed(GObject *unused, gpointer root)
{
    (void)unused;
    SongPanel *panel = Panel(root);
    if (panel && !panel->painting)
        Invalidate(panel);
}
/* Replacement approval belongs to the inspected path as well as the draft. */
static void PathChanged(GObject *unused, gpointer root)
{
    (void)unused;
    SongPanel *panel = Panel(root);
    if (panel)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
}
/* Retire the UI owner and request cancellation without freeing a token still used by its file worker. */
static void Release(gpointer data)
{
    SongPanel *panel = data;
    if (panel->pending)
        umi_cancellation_token_request(panel->pending);
    UmiSongPlanDestroy(panel->plan);
    g_free(panel);
}
/* Release independently owned file bytes and any imported plan after the worker and its completion callback finish. */
static void FreeJob(gpointer data)
{
    SongJob *job = data;
    g_weak_ref_clear(&job->root);
    g_free(job->path);
    UmiCreativeAssetDestroy(job->asset);
    UmiSongPlanDestroy(job->plan);
    umi_cancellation_token_destroy(job->cancel);
    g_free(job);
}
/* Import reads only an explicit bounded local file. Export uses the shared
 * create-new writer, so another contributor need not reimplement path safety. */
static void Worker(GTask *task, gpointer source, gpointer data, GCancellable *unused)
{
    (void)source;
    (void)unused;
    SongJob *job = data;
    if (job->opening)
    {
        job->status = UmiCreativeAssetLoadFile(job->path, "Song plan", UMI_CREATIVE_ASSET_DOCUMENT,
                                               UMI_SONG_DOCUMENT_LIMIT, job->cancel, &job->asset);
        const void *bytes = NULL;
        size_t length = 0;
        if (job->status == UMI_STATUS_OK)
            job->status = UmiCreativeAssetBytes(job->asset, &bytes, &length);
        if (job->status == UMI_STATUS_OK)
            job->status = UmiSongPlanImport(bytes, length, job->cancel, &job->plan);
    }
    else
        job->status = UmiCreativeAssetWriteNew(job->asset, job->path, job->cancel, &job->written);
    g_task_return_boolean(task, TRUE);
}
/* Display a complete portable plan and enable exports only when that representation fits the shared document limit. */
static void Show(SongPanel *panel)
{
    UmiCreativeAsset *asset = NULL;
    UmiStatus status = UmiSongPlanExport(panel->plan, UMI_SONG_EXPORT_PLAN, NULL, &asset);
    const void *bytes = NULL;
    size_t length = 0;
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetBytes(asset, &bytes, &length);
    if (status == UMI_STATUS_OK)
    {
        char *text = g_strndup(bytes, length);
        Display(panel->review, text);
        g_free(text);
        size_t cues = 0, shots = 0;
        status = UmiSongPlanCounts(panel->plan, &cues, &shots);
        if (status == UMI_STATUS_OK)
        {
            char *message = g_strdup_printf("%zu lyric cues and %zu shots prepared locally. Save a plan or "
                                            "SRT captions; no audio/video was generated.",
                                            cues, shots);
            Message(panel, message);
            g_free(message);
        }
    }
    UmiCreativeAssetDestroy(asset);
    gtk_widget_set_sensitive(panel->save, status == UMI_STATUS_OK);
    gtk_widget_set_sensitive(panel->captions, status == UMI_STATUS_OK);
    if (status != UMI_STATUS_OK)
        Message(panel, umi_status_text(status));
}
/* Populate all fields under one update guard so importing a plan does not invalidate it halfway through filling the form. */
static void Fill(SongPanel *panel)
{
    UmiSongDraft *draft = g_new0(UmiSongDraft, 1);
    UmiStatus status = UmiSongPlanDraft(panel->plan, draft);
    if (status == UMI_STATUS_OK)
    {
        panel->painting = true;
        gtk_editable_set_text(GTK_EDITABLE(panel->title), draft->title);
        gtk_editable_set_text(GTK_EDITABLE(panel->style), draft->style);
        Display(panel->lyrics, draft->timed_lyrics);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->duration), draft->duration_ms);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->tempo), draft->beats_per_minute);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->beats), draft->beats_per_shot);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->offset), draft->first_beat_ms);
        panel->painting = false;
    }
    umi_secret_clear(draft, sizeof(*draft));
    g_free(draft);
    if (status == UMI_STATUS_OK)
        Show(panel);
    else
        Message(panel, umi_status_text(status));
}
/* A failed import leaves the existing form and plan intact. Only a complete
 * validated plan can replace them after the user's explicit replace approval. */
static void Finished(GObject *source, GAsyncResult *result, gpointer unused)
{
    (void)source;
    (void)unused;
    SongJob *job = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GObject *root = g_weak_ref_get(&job->root);
    if (!root)
        return;
    SongPanel *panel = Panel(root);
    if (panel)
    {
        panel->busy = false;
        panel->pending = NULL;
        gtk_widget_set_sensitive(panel->form, TRUE);
        gtk_widget_set_sensitive(panel->stop, FALSE);
        if (job->opening && job->status == UMI_STATUS_OK)
        {
            UmiSongPlanDestroy(panel->plan);
            panel->plan = job->plan;
            job->plan = NULL;
            Fill(panel);
        }
        else
        {
            char *text = g_strdup_printf(
                "%s: %s. %s%s", job->opening ? "Open" : "Save", umi_status_text(job->status),
                job->written.created ? "New file retained at " : "Existing files and drafts were preserved. ",
                job->written.created ? job->path : "");
            Message(panel, text);
            g_free(text);
        }
    }
    g_object_unref(root);
}
/* Capture export bytes before scheduling disk work. Disable the form while opening so a late imported file cannot overwrite a newly edited draft. */
static void Start(SongPanel *panel, bool opening, bool subtitles)
{
    SongJob *job = g_new0(SongJob, 1);
    g_weak_ref_init(&job->root, G_OBJECT(panel->root));
    job->path = g_strdup(gtk_editable_get_text(GTK_EDITABLE(panel->path)));
    job->opening = opening;
    UmiStatus status = umi_cancellation_token_create(&job->cancel);
    if (status == UMI_STATUS_OK && !opening)
        status = UmiSongPlanExport(panel->plan, subtitles ? UMI_SONG_EXPORT_SUBTITLES : UMI_SONG_EXPORT_PLAN,
                                   job->cancel, &job->asset);
    if (status != UMI_STATUS_OK)
    {
        FreeJob(job);
        Message(panel, umi_status_text(status));
        return;
    }
    panel->busy = true;
    panel->pending = job->cancel;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
    gtk_widget_set_sensitive(panel->form, FALSE);
    gtk_widget_set_sensitive(panel->stop, TRUE);
    Message(panel, opening ? "Opening local song plan…" : "Saving a new local file…");
    GTask *task = g_task_new(NULL, NULL, Finished, NULL);
    g_task_set_task_data(task, job, FreeJob);
    g_task_run_in_thread(task, Worker);
    g_object_unref(task);
}
/* Keep preparation local and file access explicit. Import requires acknowledgement because it replaces the current unsaved form. */
static void Action(GtkButton *button, gpointer root)
{
    SongPanel *panel = Panel(root);
    if (!panel)
        return;
    const char *action = g_object_get_data(G_OBJECT(button), "song-plan-action");
    if (!strcmp(action, "stop"))
    {
        if (panel->pending)
            umi_cancellation_token_request(panel->pending);
        return;
    }
    if (panel->busy)
        return;
    if (!strcmp(action, "open"))
    {
        if (!gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approval)))
        {
            Message(panel, "Save any current draft, then approve replacing it from the selected file.");
            return;
        }
        Start(panel, true, false);
        return;
    }
    if (!strcmp(action, "save") || !strcmp(action, "captions"))
    {
        if (!panel->plan)
        {
            Message(panel, "Prepare the current song plan first.");
            return;
        }
        Start(panel, false, !strcmp(action, "captions"));
        return;
    }
    Invalidate(panel);
    UmiSongDraft *draft = g_new0(UmiSongDraft, 1);
    UmiStatus status =
        Copy(draft->title, sizeof(draft->title), gtk_editable_get_text(GTK_EDITABLE(panel->title)));
    if (status == UMI_STATUS_OK)
        status = Copy(draft->style, sizeof(draft->style), gtk_editable_get_text(GTK_EDITABLE(panel->style)));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->lyrics));
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
    if (status == UMI_STATUS_OK)
        status = Copy(draft->timed_lyrics, sizeof(draft->timed_lyrics), text);
    umi_secret_clear(text, strlen(text));
    g_free(text);
    draft->duration_ms = (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->duration));
    draft->beats_per_minute = (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->tempo));
    draft->beats_per_shot = (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->beats));
    draft->first_beat_ms = (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->offset));
    if (status == UMI_STATUS_OK)
        status = UmiSongPlanCreate(draft, &panel->plan);
    umi_secret_clear(draft, sizeof(*draft));
    g_free(draft);
    if (status == UMI_STATUS_OK)
        Show(panel);
    else
        Message(panel, "Check the title, timing and limits. Lyrics must use [mm:ss.mmm]text, in increasing "
                       "time order, before song end.");
}
static GtkWidget *Button(SongPanel *panel, GtkWidget *box, const char *label, const char *action)
{
    GtkWidget *widget = gtk_button_new_with_label(label);
    char *id = g_strdup_printf("song-plan.%s", action);
    Tag(widget, id);
    g_free(id);
    g_object_set_data_full(G_OBJECT(widget), "song-plan-action", g_strdup(action), g_free);
    g_signal_connect_object(widget, "clicked", G_CALLBACK(Action), G_OBJECT(panel->root), 0);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}
static GtkWidget *Spin(SongPanel *panel, const char *label, const char *id, double low, double high,
                       double initial)
{
    Label(panel->form, label);
    GtkWidget *widget = gtk_spin_button_new_with_range(low, high, 1);
    Tag(widget, id);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), initial);
    gtk_box_append(GTK_BOX(panel->form), widget);
    return widget;
}
/* Compose a reusable native form; Media and Music receive identical timing and persistence behaviour from Framework. */
UmiStatus UmiSongPlanGtkCreate(GtkWidget **out)
{
    if (!out || *out)
        return UMI_STATUS_INVALID_ARGUMENT;
    SongPanel *panel = g_new0(SongPanel, 1);
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    Tag(panel->root, "song-plan.panel");
    g_object_set_data_full(G_OBJECT(panel->root), PANEL_KEY, panel, Release);
    Label(panel->root, "Song and music-video plan · write timed lyrics and plan cuts from a tempo you enter. "
                       "This local tool does not generate songs or detect beats.");
    panel->form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_append(GTK_BOX(panel->root), panel->form);
    panel->title = Entry(panel->form, "Song title", "song-plan.title");
    panel->style = Entry(panel->form, "Visual style and mood", "song-plan.style");
    panel->duration = Spin(panel, "Song duration in milliseconds", "song-plan.duration", 1, 600000, 30000);
    panel->tempo = Spin(panel, "Tempo in beats per minute", "song-plan.tempo", 30, 300, 120);
    panel->beats = Spin(panel, "Beats per shot", "song-plan.beats", 1, 32, 4);
    panel->offset = Spin(panel, "First beat offset in milliseconds", "song-plan.offset", 0, 599999, 0);
    Label(panel->form, "One lyric per line, for example [00:02.500]A new day begins. Leave empty for "
                       "instrumental. Each line lasts until the next line or song end.");
    panel->lyrics = View(panel->form, "song-plan.lyrics", true);
    Button(panel, panel->form, "Prepare song plan", "prepare");
    panel->review = View(panel->form, "song-plan.review", false);
    panel->path =
        Entry(panel->form, "Full local path: .json for plans or .srt for captions", "song-plan.path");
    panel->save = Button(panel, panel->form, "Save new plan file", "save");
    panel->captions = Button(panel, panel->form, "Save new SRT captions", "captions");
    gtk_widget_set_sensitive(panel->save, FALSE);
    gtk_widget_set_sensitive(panel->captions, FALSE);
    panel->approval = Check(panel->form, "I saved my draft and approve replacing it with the opened plan",
                            "song-plan.approval");
    Button(panel, panel->form, "Open plan from path", "open");
    panel->stop = Button(panel, panel->root, "Stop local file operation", "stop");
    gtk_widget_set_sensitive(panel->stop, FALSE);
    panel->message =
        Label(panel->root, "Plans remain in memory until saved. Existing files are never replaced.");
    Tag(panel->message, "song-plan.message");
    GtkWidget *entries[] = {panel->title, panel->style};
    for (size_t index = 0; index < G_N_ELEMENTS(entries); ++index)
        g_signal_connect_object(entries[index], "changed", G_CALLBACK(Changed), G_OBJECT(panel->root), 0);
    GtkWidget *spins[] = {panel->duration, panel->tempo, panel->beats, panel->offset};
    for (size_t index = 0; index < G_N_ELEMENTS(spins); ++index)
        g_signal_connect_object(spins[index], "value-changed", G_CALLBACK(Changed), G_OBJECT(panel->root), 0);
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->lyrics)), "changed",
                            G_CALLBACK(Changed), G_OBJECT(panel->root), 0);
    g_signal_connect_object(panel->path, "changed", G_CALLBACK(PathChanged), G_OBJECT(panel->root), 0);
    *out = panel->root;
    return UMI_STATUS_OK;
}
