/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/audio_arrangement_gtk4.c
 * PURPOSE: Connect asset libraries and reusable PCM mixing to Media and Music through worker-owned jobs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/audio_arrangement.h"
#include "umicom/creative_workspace/asset_library_archive.h"
#include <string.h>
typedef enum Operation
{
    ARRANGEMENT_LOAD_LIBRARY,
    ARRANGEMENT_LOAD_PLAN,
    ARRANGEMENT_SAVE_PLAN,
    ARRANGEMENT_RENDER,
    ARRANGEMENT_SAVE_WAVE
} Operation;
typedef struct Panel
{
    UmiCreativeAudioArrangement plan;
    UmiCreativeAssetLibrary *library;
    UmiCreativeAsset *rendered;
    UmiCancellationToken *cancel;
    bool busy, dirty;
    GtkWidget *form, *title, *rate, *library_path, *library_info, *plan_path, *approval, *output_path;
    GtkWidget *id, *asset_id, *start, *begin, *end, *gain, *fade_in, *fade_out;
    GtkWidget *summary, *render, *save_wave, *open_plan, *status, *cancel_button;
} Panel;
typedef struct Job
{
    GWeakRef root;
    Operation operation;
    char *path;
    UmiCreativeAudioArrangement plan, loaded_plan;
    UmiCreativeAssetLibrary *library, *loaded_library;
    UmiCreativeAsset *rendered;
    UmiCreativeAudioArrangementReport report;
    UmiCreativeAssetWriteResult written;
    UmiCancellationToken *cancel;
    UmiStatus status;
} Job;
static Panel *State(GtkWidget *root) { return g_object_get_data(G_OBJECT(root), "umicom-audio-arrangement"); }
static void Tag(GtkWidget *widget, const char *id)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free);
}
static void Message(Panel *panel, const char *text) { gtk_label_set_text(GTK_LABEL(panel->status), text); }
static void Dispose(gpointer data)
{
    Panel *panel = data;
    if (panel->cancel != NULL)
        umi_cancellation_token_request(panel->cancel);
    UmiCreativeAssetDestroy(panel->rendered);
    UmiCreativeAssetLibraryDestroy(panel->library);
    g_free(panel);
}
static void Sensitivity(Panel *panel)
{
    gtk_widget_set_sensitive(panel->form, !panel->busy);
    gtk_widget_set_sensitive(panel->render,
                             !panel->busy && panel->library != NULL && panel->plan.clip_count != 0U);
    gtk_widget_set_sensitive(panel->save_wave, !panel->busy && panel->rendered != NULL);
    gtk_widget_set_sensitive(
        panel->open_plan,
        !panel->busy && (!panel->dirty || gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approval))));
    gtk_widget_set_sensitive(panel->cancel_button, panel->busy);
}
static void Summary(Panel *panel)
{
    GString *text = g_string_new(NULL);
    g_string_append_printf(text, "%s — %u Hz, %zu / %u placements. %s\n", panel->plan.title,
                           panel->plan.sample_rate, panel->plan.clip_count,
                           UMI_CREATIVE_AUDIO_ARRANGEMENT_MAX_CLIPS,
                           panel->dirty ? "Unsaved arrangement edits." : "No unsaved arrangement edits.");
    for (size_t i = 0U; i < panel->plan.clip_count; ++i)
    {
        const UmiCreativeAudioPlacement *clip = &panel->plan.clips[i];
        g_string_append_printf(
            text, "%s: asset %s; timeline %u ms; source [%u, %u) ms; gain %u/1000; fades %u / %u ms\n",
            clip->id, clip->asset_id, clip->start_ms, clip->source_begin_ms, clip->source_end_ms,
            clip->gain_permille, clip->fade_in_ms, clip->fade_out_ms);
    }
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->summary)), text->str, -1);
    g_string_free(text, TRUE);
    Sensitivity(panel);
}
static void Invalidate(Panel *panel)
{
    UmiCreativeAssetDestroy(panel->rendered);
    panel->rendered = NULL;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
}
static void Changed(Panel *panel)
{
    panel->dirty = true;
    Invalidate(panel);
    Summary(panel);
}
static UmiStatus Copy(char *out, size_t capacity, const char *text)
{
    size_t length = strlen(text);
    if (length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}
static uint32_t Value(GtkWidget *field)
{
    return (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(field));
}
static void ApplySettings(GtkButton *button, gpointer root)
{
    (void)button;
    Panel *panel = State(GTK_WIDGET(root));
    if (panel->busy)
        return;
    UmiCreativeAudioArrangement plan = panel->plan;
    UmiStatus status =
        Copy(plan.title, sizeof(plan.title), gtk_editable_get_text(GTK_EDITABLE(panel->title)));
    plan.sample_rate = Value(panel->rate);
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAudioArrangementValidate(&plan);
    if (status == UMI_STATUS_OK)
    {
        panel->plan = plan;
        Changed(panel);
        Message(panel, "Arrangement settings applied. Render again after editing.");
    }
    else
        Message(panel, "Settings were not applied. Use a valid title and a supported sample rate.");
}
static void Put(GtkWidget *root, bool replace)
{
    Panel *panel = State(root);
    if (panel->busy)
        return;
    UmiCreativeAudioPlacement clip = {0};
    UmiStatus status = Copy(clip.id, sizeof(clip.id), gtk_editable_get_text(GTK_EDITABLE(panel->id)));
    if (status == UMI_STATUS_OK)
        status =
            Copy(clip.asset_id, sizeof(clip.asset_id), gtk_editable_get_text(GTK_EDITABLE(panel->asset_id)));
    clip.start_ms = Value(panel->start);
    clip.source_begin_ms = Value(panel->begin);
    clip.source_end_ms = Value(panel->end);
    clip.gain_permille = (unsigned)Value(panel->gain);
    clip.fade_in_ms = Value(panel->fade_in);
    clip.fade_out_ms = Value(panel->fade_out);
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAudioArrangementPut(&panel->plan, &clip, replace);
    if (status == UMI_STATUS_OK)
    {
        Changed(panel);
        Message(panel, "Placement applied. Source ranges and sample rates are checked against the loaded "
                       "library when rendering.");
    }
    else
    {
        char *message = g_strdup_printf("Placement not applied: %s. Add requires a new ID; Replace requires "
                                        "an existing ID. Check range, gain and fade lengths.",
                                        umi_status_text(status));
        Message(panel, message);
        g_free(message);
    }
}
static void Add(GtkButton *button, gpointer root)
{
    (void)button;
    Put(GTK_WIDGET(root), false);
}
static void Replace(GtkButton *button, gpointer root)
{
    (void)button;
    Put(GTK_WIDGET(root), true);
}
static void Remove(GtkButton *button, gpointer root)
{
    (void)button;
    Panel *panel = State(GTK_WIDGET(root));
    if (panel->busy)
        return;
    UmiStatus status =
        UmiCreativeAudioArrangementRemove(&panel->plan, gtk_editable_get_text(GTK_EDITABLE(panel->id)));
    if (status == UMI_STATUS_OK)
    {
        Changed(panel);
        Message(panel, "Placement removed. Source captures and saved files remain unchanged.");
    }
    else
        Message(panel, "No placement was removed. Enter its exact placement ID.");
}
static void Select(GtkButton *button, gpointer root)
{
    (void)button;
    Panel *panel = State(GTK_WIDGET(root));
    if (panel->busy)
        return;
    const char *id = gtk_editable_get_text(GTK_EDITABLE(panel->id));
    for (size_t i = 0U; i < panel->plan.clip_count; ++i)
        if (strcmp(id, panel->plan.clips[i].id) == 0)
        {
            const UmiCreativeAudioPlacement *clip = &panel->plan.clips[i];
            gtk_editable_set_text(GTK_EDITABLE(panel->asset_id), clip->asset_id);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->start), (double)clip->start_ms);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->begin), (double)clip->source_begin_ms);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->end), (double)clip->source_end_ms);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->gain), (double)clip->gain_permille);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->fade_in), (double)clip->fade_in_ms);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->fade_out), (double)clip->fade_out_ms);
            Message(panel, "Placement loaded into the form. Choose Replace placement to apply edits.");
            return;
        }
    Message(panel, "Enter an existing placement ID to load its fields.");
}
static void JobFree(gpointer data)
{
    Job *job = data;
    g_weak_ref_clear(&job->root);
    g_free(job->path);
    UmiCreativeAssetLibraryDestroy(job->library);
    UmiCreativeAssetLibraryDestroy(job->loaded_library);
    UmiCreativeAssetDestroy(job->rendered);
    umi_cancellation_token_destroy(job->cancel);
    g_free(job);
}
/* The worker borrows no widgets. It receives a complete plan value, exclusive
 * library ownership and copied paths, then returns owned results through GTask.
 * Contributors can add another renderer without moving I/O into callbacks. */
static void Work(GTask *task, gpointer source, gpointer data, GCancellable *unused)
{
    (void)source;
    (void)unused;
    Job *job = data;
    if (job->operation == ARRANGEMENT_LOAD_LIBRARY)
        job->status = UmiCreativeAssetLibraryLoad(job->path, UMI_CREATIVE_LIBRARY_MAX_BYTES, job->cancel,
                                                  &job->loaded_library);
    else if (job->operation == ARRANGEMENT_LOAD_PLAN)
    {
        UmiCreativeAsset *document = NULL;
        const void *bytes = NULL;
        size_t size = 0U;
        job->status =
            UmiCreativeAssetLoadFile(job->path, "Arrangement", UMI_CREATIVE_ASSET_DOCUMENT,
                                     UMI_CREATIVE_AUDIO_ARRANGEMENT_DOCUMENT_BYTES, job->cancel, &document);
        if (job->status == UMI_STATUS_OK)
            job->status = UmiCreativeAssetBytes(document, &bytes, &size);
        if (job->status == UMI_STATUS_OK)
            job->status = UmiCreativeAudioArrangementImport(bytes, size, job->cancel, &job->loaded_plan);
        UmiCreativeAssetDestroy(document);
    }
    else if (job->operation == ARRANGEMENT_SAVE_PLAN)
    {
        UmiCreativeAsset *document = NULL;
        job->status = UmiCreativeAudioArrangementExport(&job->plan, job->cancel, &document);
        if (job->status == UMI_STATUS_OK)
            job->status = UmiCreativeAssetWriteNew(document, job->path, job->cancel, &job->written);
        UmiCreativeAssetDestroy(document);
    }
    else if (job->operation == ARRANGEMENT_RENDER)
        job->status = UmiCreativeAudioArrangementRender(&job->plan, job->library, job->cancel, &job->rendered,
                                                        &job->report);
    else
        job->status = UmiCreativeAssetWriteNew(job->rendered, job->path, job->cancel, &job->written);
    g_task_return_boolean(task, TRUE);
}
/* Reacquire the view only for publication. Late results from a closed window
 * are freed with the job, and cancellation can still refuse a completed read
 * before it replaces the live plan. A started file write has its own receipt. */
static void Done(GObject *source, GAsyncResult *result, gpointer unused)
{
    (void)source;
    (void)unused;
    Job *job = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GtkWidget *root = g_weak_ref_get(&job->root);
    if (root == NULL)
        return;
    Panel *panel = State(root);
    panel->busy = false;
    panel->cancel = NULL;
    panel->library = job->library;
    job->library = NULL;
    if (job->operation != ARRANGEMENT_SAVE_PLAN && job->operation != ARRANGEMENT_SAVE_WAVE &&
        umi_cancellation_token_is_requested(job->cancel))
        job->status = UMI_STATUS_CANCELLED;
    if (job->operation == ARRANGEMENT_SAVE_WAVE ||
        (job->operation == ARRANGEMENT_RENDER && job->status == UMI_STATUS_OK))
    {
        UmiCreativeAssetDestroy(panel->rendered);
        panel->rendered = job->rendered;
        job->rendered = NULL;
    }
    if (job->status == UMI_STATUS_OK && job->operation == ARRANGEMENT_LOAD_LIBRARY)
    {
        UmiCreativeAssetLibraryDestroy(panel->library);
        panel->library = job->loaded_library;
        job->loaded_library = NULL;
        Invalidate(panel);
        UmiCreativeAssetLibraryInfo info;
        if (UmiCreativeAssetLibraryInspect(panel->library, &info) == UMI_STATUS_OK)
        {
            GString *text = g_string_new(NULL);
            g_string_append_printf(text, "Loaded: %s (%zu assets, %zu bytes)\n", info.title, info.asset_count,
                                   info.byte_count);
            for (size_t i = 0U; i < info.asset_count; ++i)
            {
                UmiCreativeAssetLibraryEntry entry;
                if (UmiCreativeAssetLibraryAt(panel->library, i, &entry) == UMI_STATUS_OK)
                    g_string_append_printf(text, "%s — %s (%zu bytes)\n", entry.id, entry.asset.label,
                                           entry.asset.byte_count);
            }
            gtk_label_set_text(GTK_LABEL(panel->library_info), text->str);
            g_string_free(text, TRUE);
        }
    }
    else if (job->status == UMI_STATUS_OK && job->operation == ARRANGEMENT_LOAD_PLAN)
    {
        panel->plan = job->loaded_plan;
        panel->dirty = false;
        Invalidate(panel);
        gtk_editable_set_text(GTK_EDITABLE(panel->title), panel->plan.title);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->rate), (double)panel->plan.sample_rate);
    }
    else if (job->status == UMI_STATUS_OK && job->operation == ARRANGEMENT_SAVE_PLAN)
    {
        panel->dirty = false;
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
    }
    Summary(panel);
    const char *names[] = {"Load library", "Open arrangement", "Save arrangement", "Render mix",
                           "Export WAV"};
    char *message;
    if (job->operation == ARRANGEMENT_RENDER && job->status == UMI_STATUS_OK)
        message = g_strdup_printf(
            "Rendered %" G_GUINT64_FORMAT
            " stereo frames at %u Hz from %zu placements / %zu sources. Clipped samples: %" G_GUINT64_FORMAT
            ". %s No file has been written.",
            (guint64)job->report.frames, job->report.sample_rate, job->report.placements,
            job->report.unique_sources, (guint64)job->report.clipped_samples,
            job->report.clipped_samples ? "Reduce gains and render again before exporting."
                                        : "Ready for explicit WAV export.");
    else if (job->written.created)
        message = g_strdup_printf("%s: %s. New file retained: %s (%" G_GUINT64_FORMAT " bytes written).",
                                  names[job->operation], umi_status_text(job->status), job->written.file.path,
                                  (guint64)job->written.file.bytes_written);
    else
        message = g_strdup_printf("%s: %s.%s", names[job->operation], umi_status_text(job->status),
                                  job->status == UMI_STATUS_OK
                                      ? ""
                                      : " Current plan and library retained. Check asset IDs, PCM16 format, "
                                        "source ranges and matching sample rates.");
    Message(panel, message);
    g_free(message);
    g_object_unref(root);
}
static void Start(GtkWidget *root, Operation operation)
{
    Panel *panel = State(root);
    if (panel->busy)
        return;
    if (operation == ARRANGEMENT_LOAD_PLAN && panel->dirty &&
        !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approval)))
    {
        Message(panel, "Save the arrangement first, or explicitly allow replacing its unsaved edits.");
        return;
    }
    if (operation == ARRANGEMENT_RENDER && (panel->library == NULL || panel->plan.clip_count == 0U))
        return;
    if (operation == ARRANGEMENT_SAVE_WAVE && panel->rendered == NULL)
        return;
    Job *job = g_new0(Job, 1);
    g_weak_ref_init(&job->root, G_OBJECT(root));
    job->operation = operation;
    job->plan = panel->plan;
    job->path = g_strdup(
        gtk_editable_get_text(GTK_EDITABLE(operation == ARRANGEMENT_LOAD_LIBRARY ? panel->library_path
                                           : operation == ARRANGEMENT_SAVE_WAVE  ? panel->output_path
                                                                                 : panel->plan_path)));
    UmiStatus status = umi_cancellation_token_create(&job->cancel);
    if (status != UMI_STATUS_OK)
    {
        Message(panel, umi_status_text(status));
        JobFree(job);
        return;
    }
    if (operation == ARRANGEMENT_RENDER)
    {
        UmiCreativeAssetDestroy(panel->rendered);
        panel->rendered = NULL;
    }
    if (operation == ARRANGEMENT_SAVE_WAVE)
    {
        job->rendered = panel->rendered;
        panel->rendered = NULL;
    }
    job->library = panel->library;
    panel->library = NULL;
    panel->busy = true;
    panel->cancel = job->cancel;
    Sensitivity(panel);
    Message(panel, "Working. A cancelled export may leave a newly created partial file; read the result "
                   "before retrying.");
    GTask *task = g_task_new(NULL, NULL, Done, NULL);
    g_task_set_task_data(task, job, JobFree);
    g_task_run_in_thread(task, Work);
    g_object_unref(task);
}
static void LoadSourceLibrary(GtkButton *button, gpointer root)
{
    (void)button;
    Start(GTK_WIDGET(root), ARRANGEMENT_LOAD_LIBRARY);
}
static void Open(GtkButton *button, gpointer root)
{
    (void)button;
    Start(GTK_WIDGET(root), ARRANGEMENT_LOAD_PLAN);
}
static void Save(GtkButton *button, gpointer root)
{
    (void)button;
    Start(GTK_WIDGET(root), ARRANGEMENT_SAVE_PLAN);
}
static void Render(GtkButton *button, gpointer root)
{
    (void)button;
    Start(GTK_WIDGET(root), ARRANGEMENT_RENDER);
}
static void SaveWave(GtkButton *button, gpointer root)
{
    (void)button;
    Start(GTK_WIDGET(root), ARRANGEMENT_SAVE_WAVE);
}
static void Cancel(GtkButton *button, gpointer root)
{
    (void)button;
    Panel *panel = State(GTK_WIDGET(root));
    if (panel->cancel != NULL)
        umi_cancellation_token_request(panel->cancel);
}
static void ApprovalChanged(GtkCheckButton *button, gpointer root)
{
    (void)button;
    Sensitivity(State(GTK_WIDGET(root)));
}
static void PlanPathChanged(GtkEditable *entry, gpointer root)
{
    (void)entry;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(State(GTK_WIDGET(root))->approval), FALSE);
}
static GtkWidget *Text(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label);
    return label;
}
static GtkWidget *Entry(GtkWidget *box, const char *label, const char *id)
{
    Text(box, label);
    GtkWidget *field = gtk_entry_new();
    Tag(field, id);
    gtk_box_append(GTK_BOX(box), field);
    return field;
}
static GtkWidget *Spin(GtkWidget *box, const char *label, const char *id, double minimum, double maximum,
                       double value)
{
    Text(box, label);
    GtkWidget *field = gtk_spin_button_new_with_range(minimum, maximum, 1.0);
    Tag(field, id);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(field), value);
    gtk_box_append(GTK_BOX(box), field);
    return field;
}
static GtkWidget *Button(GtkWidget *box, GtkWidget *root, const char *label, const char *id,
                         GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    Tag(button, id);
    g_signal_connect_object(button, "clicked", callback, root, 0);
    gtk_box_append(GTK_BOX(box), button);
    return button;
}
GtkWidget *UmiCreativeAudioArrangementGtkCreate(void)
{
    Panel *panel = g_new0(Panel, 1);
    if (UmiCreativeAudioArrangementInit("Audio arrangement", 48000U, &panel->plan) != UMI_STATUS_OK)
    {
        g_free(panel);
        return NULL;
    }
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    Tag(root, "creative.arrangement");
    g_object_set_data_full(G_OBJECT(root), "umicom-audio-arrangement", panel, Dispose);
    Text(root, "Arrange PCM16 WAV assets from a saved Asset library. Sources must have the same sample rate; "
               "mono is copied to stereo. Up to 16 placements, 4 MiB per source and 64 MiB per rendered WAV. "
               "No resampling, playback or device recording is performed here.");
    panel->form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *form = panel->form;
    gtk_box_append(GTK_BOX(root), form);
    panel->library_path =
        Entry(form, "Absolute saved asset library path", "creative.arrangement.library-path");
    Button(form, root, "Load asset library", "creative.arrangement.load-library",
           G_CALLBACK(LoadSourceLibrary));
    panel->library_info = Text(form, "No asset library loaded.");
    gtk_label_set_selectable(GTK_LABEL(panel->library_info), TRUE);
    panel->title = Entry(form, "Arrangement title", "creative.arrangement.title");
    gtk_editable_set_text(GTK_EDITABLE(panel->title), panel->plan.title);
    panel->rate = Spin(form, "Output sample rate (must match every source)", "creative.arrangement.rate",
                       8000, 192000, 48000);
    Button(form, root, "Apply title and sample rate", "creative.arrangement.settings",
           G_CALLBACK(ApplySettings));
    panel->id = Entry(form, "Placement ID (unique within this arrangement)", "creative.arrangement.id");
    Button(form, root, "Load placement fields by ID", "creative.arrangement.select", G_CALLBACK(Select));
    panel->asset_id = Entry(form, "Source asset ID from the loaded library", "creative.arrangement.asset-id");
    panel->start = Spin(form, "Timeline start (milliseconds)", "creative.arrangement.start", 0, 600000, 0);
    panel->begin =
        Spin(form, "Source begin (milliseconds, included)", "creative.arrangement.begin", 0, 600000, 0);
    panel->end =
        Spin(form, "Source end (milliseconds, excluded)", "creative.arrangement.end", 1, 600000, 1000);
    panel->gain =
        Spin(form, "Gain (0 = silent, 1000 = original)", "creative.arrangement.gain", 0, 1000, 1000);
    panel->fade_in = Spin(form, "Fade in (milliseconds)", "creative.arrangement.fade-in", 0, 600000, 0);
    panel->fade_out = Spin(form, "Fade out (milliseconds)", "creative.arrangement.fade-out", 0, 600000, 0);
    Button(form, root, "Add new placement", "creative.arrangement.add", G_CALLBACK(Add));
    Button(form, root, "Replace existing placement", "creative.arrangement.replace", G_CALLBACK(Replace));
    Button(form, root, "Remove placement by ID", "creative.arrangement.remove", G_CALLBACK(Remove));
    Text(form, "The summary is the applied plan. Form changes take effect only after Apply, Add or Replace.");
    panel->summary = gtk_text_view_new();
    Tag(panel->summary, "creative.arrangement.summary");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->summary), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(panel->summary), GTK_WRAP_WORD_CHAR);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scroll, -1, 180);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), panel->summary);
    gtk_box_append(GTK_BOX(form), scroll);
    panel->render =
        Button(form, root, "Render arrangement in memory", "creative.arrangement.render", G_CALLBACK(Render));
    panel->output_path = Entry(form, "New absolute output WAV path", "creative.arrangement.output-path");
    panel->save_wave = Button(form, root, "Export rendered WAV to new file", "creative.arrangement.save-wave",
                              G_CALLBACK(SaveWave));
    panel->plan_path = Entry(form, "Absolute arrangement JSON path", "creative.arrangement.plan-path");
    Button(form, root, "Save arrangement to new file", "creative.arrangement.save-plan", G_CALLBACK(Save));
    panel->approval = gtk_check_button_new_with_label("Allow Open to replace my unsaved arrangement edits");
    Tag(panel->approval, "creative.arrangement.approval");
    gtk_box_append(GTK_BOX(form), panel->approval);
    panel->open_plan =
        Button(form, root, "Open arrangement", "creative.arrangement.open-plan", G_CALLBACK(Open));
    g_signal_connect_object(panel->approval, "toggled", G_CALLBACK(ApprovalChanged), root, 0);
    g_signal_connect_object(panel->plan_path, "changed", G_CALLBACK(PlanPathChanged), root, 0);
    panel->cancel_button =
        Button(root, root, "Cancel pending operation", "creative.arrangement.cancel", G_CALLBACK(Cancel));
    panel->status = Text(root, "Load a library and apply placements. Save the arrangement and library "
                               "separately to reopen the work later.");
    Tag(panel->status, "creative.arrangement.status");
    Summary(panel);
    return root;
}
UmiStatus UmiCreativeAudioArrangementGtkSnapshot(GtkWidget *root, UmiCreativeAudioArrangement *out,
                                                 bool *out_rendered)
{
    if (!GTK_IS_WIDGET(root) || State(root) == NULL || out == NULL || out_rendered == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    Panel *panel = State(root);
    if (panel->busy)
        return UMI_STATUS_BUSY;
    *out = panel->plan;
    *out_rendered = panel->rendered != NULL;
    return UMI_STATUS_OK;
}
