/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/captions_gtk4.c
 * PURPOSE: Compose shared caption editing, preview and explicit worker-owned file operations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/captions.h"
#include "umicom/ui/gtk4/path_picker.h"
#include <string.h>
#define CAPTION_PANEL_KEY "umicom-caption-panel"
typedef struct CaptionPanel
{
    GtkWidget *root, *form, *editor, *preview, *path, *approval, *message, *stop;
    GtkWidget *offset, *begin, *end, *rebase, *time;
    UmiCaptionTrack *track, *previous;
    UmiCancellationToken *pending;
    bool busy, painting;
    /* Cue forms are drafts. A prepared-document change retires their loaded identity. */
    GtkWidget *rowNumber, *rowBegin, *rowEnd, *rowText, *rowSecondText, *rowSplit;
    size_t loadedRow;
    bool rowLoaded, rowLoading;
} CaptionPanel;
typedef struct CaptionJob
{
    GWeakRef root;
    char *path, *source;
    UmiCaptionTrack *track;
    UmiCreativeAsset *asset;
    UmiCancellationToken *cancel;
    UmiCreativeAssetWriteResult written;
    UmiStatus status;
    bool opening;
} CaptionJob;
static CaptionPanel *Panel(gpointer root)
{
    return g_object_get_data(G_OBJECT(root), CAPTION_PANEL_KEY);
}
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
static void Message(CaptionPanel *panel, const char *text)
{
    gtk_label_set_text(GTK_LABEL(panel->message), text);
}
static GtkTextBuffer *Buffer(GtkWidget *view)
{
    return gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
}
static void Display(GtkWidget *view, const char *text)
{
    gtk_text_buffer_set_text(Buffer(view), text, -1);
}
/* A character bound prevents copying an arbitrarily large pasted draft. The
 * byte bound below remains necessary because UTF-8 characters use several bytes. */
static char *Source(CaptionPanel *panel)
{
    GtkTextBuffer *buffer = Buffer(panel->editor);
    if (gtk_text_buffer_get_char_count(buffer) > (int)UMI_CAPTION_DOCUMENT_LIMIT)
        return NULL;
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
    if (strlen(text) > UMI_CAPTION_DOCUMENT_LIMIT)
    {
        g_free(text);
        return NULL;
    }
    return text;
}
/* Manual edits retire both prepared output and its undo partner. An old review
 * must not be used to export a newer, unvalidated draft. */
static void Invalidate(CaptionPanel *panel)
{
    panel->rowLoaded = false;
    UmiCaptionTrackDestroy(panel->track);
    UmiCaptionTrackDestroy(panel->previous);
    panel->track = panel->previous = NULL;
}
static void Changed(GObject *unused, gpointer root)
{
    (void)unused;
    CaptionPanel *panel = Panel(root);
    if (panel && !panel->painting)
    {
        Invalidate(panel);
        Display(panel->preview, "");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
        Message(panel, "Draft changed. Prepare captions before previewing or saving.");
    }
}
static void PathChanged(GObject *unused, gpointer root)
{
    (void)unused;
    CaptionPanel *panel = Panel(root);
    if (panel)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
}
static void Release(gpointer data)
{
    CaptionPanel *panel = data;
    if (panel->pending)
        umi_cancellation_token_request(panel->pending);
    Invalidate(panel);
    g_free(panel);
}
static void FreeJob(gpointer data)
{
    CaptionJob *job = data;
    g_weak_ref_clear(&job->root);
    g_free(job->path);
    g_free(job->source);
    UmiCreativeAssetDestroy(job->asset);
    UmiCaptionTrackDestroy(job->track);
    umi_cancellation_token_destroy(job->cancel);
    g_free(job);
}
/* Preview all active rows: overlapping speakers must not disappear just because
 * another cue began later. The end boundary is excluded, as in the core API. */
static void Preview(CaptionPanel *panel)
{
    if (!panel->track)
    {
        Message(panel, "Prepare the current captions first.");
        return;
    }
    UmiCaptionInfo info;
    if (UmiCaptionTrackInspect(panel->track, &info) != UMI_STATUS_OK)
        return;
    uint32_t time = (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->time));
    GString *text = g_string_new("");
    for (size_t i = 0; i < info.cue_count; ++i)
    {
        UmiCaptionCue cue;
        if (UmiCaptionTrackCue(panel->track, i, &cue) == UMI_STATUS_OK && cue.begin_ms <= time &&
            time < cue.end_ms)
        {
            if (text->len)
                g_string_append(text, "\n\n");
            g_string_append(text, cue.text);
        }
    }
    Display(panel->preview, text->len ? text->str : "(No caption at this time)");
    g_string_free(text, TRUE);
    if (!panel->track)
        return; /* A host signal may have invalidated the draft. */
    char *summary =
        g_strdup_printf("%zu captions prepared. Latest end: %u ms. Preview is text only.",
                        info.cue_count, (unsigned)info.end_ms);
    Message(panel, summary);
    g_free(summary);
}
/* Serialise before changing ownership. A failed export leaves the existing
 * source and undo partner available. The final comparison catches programmatic
 * edits made by another signal handler while the canonical text is displayed. */
static UmiStatus Publish(CaptionPanel *panel, UmiCaptionTrack **next)
{
    UmiCreativeAsset *asset = NULL;
    UmiStatus status = UmiCaptionTrackExport(*next, UMI_CAPTION_SRT, NULL, &asset);
    const void *bytes = NULL;
    size_t size = 0;
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetBytes(asset, &bytes, &size);
    if (status == UMI_STATUS_OK)
    {
        char *text = g_strndup(bytes, size);
        UmiCaptionTrackDestroy(panel->previous);
        panel->previous = panel->track;
        panel->track = *next;
        *next = NULL;
        panel->rowLoaded = false;
        panel->painting = true;
        Display(panel->editor, text);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
        char *actual = Source(panel);
        panel->painting = false;
        if (!actual || strcmp(actual, text))
        {
            Invalidate(panel);
            Display(panel->preview, "");
            status = UMI_STATUS_BUSY;
        }
        g_free(actual);
        g_free(text);
    }
    UmiCreativeAssetDestroy(asset);
    if (status == UMI_STATUS_OK)
        Preview(panel);
    return status;
}
/* Workers own immutable inputs and share only a cancellation token with GTK.
 * The existing asset reader/writer supplies bounds and create-new semantics. */
static void Worker(GTask *task, gpointer source, gpointer data, GCancellable *unused)
{
    (void)source;
    (void)unused;
    CaptionJob *job = data;
    if (job->opening)
    {
        job->status =
            UmiCreativeAssetLoadFile(job->path, "Captions", UMI_CREATIVE_ASSET_DOCUMENT,
                                     UMI_CAPTION_DOCUMENT_LIMIT, job->cancel, &job->asset);
        const void *bytes = NULL;
        size_t size = 0;
        if (job->status == UMI_STATUS_OK)
            job->status = UmiCreativeAssetBytes(job->asset, &bytes, &size);
        if (job->status == UMI_STATUS_OK)
            /* The shared reader now recognises both caption formats. Keep the SRT-only call for review; the worker still owns every import byte. */
#if 0
            job->status = UmiCaptionTrackParseSrt("Captions", bytes, size, &job->track);
#else
            job->status = UmiCaptionTrackParse("Captions", bytes, size, &job->track);
#endif
        if (job->status == UMI_STATUS_OK && umi_cancellation_token_is_requested(job->cancel))
            job->status = UMI_STATUS_CANCELLED;
    }
    else
        job->status = UmiCreativeAssetWriteNew(job->asset, job->path, job->cancel, &job->written);
    g_task_return_boolean(task, TRUE);
}
/* A failed or stale import cannot replace a draft. Compare the exact captured
 * text even though controls are disabled: host code can still change a buffer. */
static void Finished(GObject *source, GAsyncResult *result, gpointer unused)
{
    (void)source;
    (void)unused;
    CaptionJob *job = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GObject *root = g_weak_ref_get(&job->root);
    if (!root)
        return;
    CaptionPanel *panel = Panel(root);
    if (panel)
    {
        panel->pending = NULL;
        if (job->opening && job->status == UMI_STATUS_OK)
        {
            char *actual = Source(panel);
            if (!actual || strcmp(actual, job->source) ||
                umi_cancellation_token_is_requested(job->cancel))
                job->status = UMI_STATUS_BUSY;
            else
                job->status = Publish(panel, &job->track);
            g_free(actual);
        }
        char *text = g_strdup_printf(
            "%s: %s. %s%s", job->opening ? "Open captions" : "Save captions",
            umi_status_text(job->status),
            job->written.created ? "New file retained at " : "Existing files were preserved. ",
            job->written.created ? job->path : "");
        Message(panel, text);
        g_free(text);
        /* Keep actions guarded while sensitivity notifications are emitted. */
        gtk_widget_set_sensitive(panel->stop, FALSE);
        gtk_widget_set_sensitive(panel->form, TRUE);
        panel->busy = false;
    }
    g_object_unref(root);
}
static void Start(CaptionPanel *panel, bool opening, UmiCaptionFormat format)
{
    char *source = Source(panel);
    if (!source)
    {
        Message(panel, "Caption draft exceeds the document limit.");
        return;
    }
    if (opening && *source && !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approval)))
    {
        g_free(source);
        Message(panel, "Save your current draft, then approve replacing it before opening a file.");
        return;
    }
    CaptionJob *job = g_new0(CaptionJob, 1);
    g_weak_ref_init(&job->root, G_OBJECT(panel->root));
    job->source = source;
    job->path = g_strdup(gtk_editable_get_text(GTK_EDITABLE(panel->path)));
    job->opening = opening;
    UmiStatus status = umi_cancellation_token_create(&job->cancel);
    if (status == UMI_STATUS_OK && !opening)
        status = UmiCaptionTrackExport(panel->track, format, job->cancel, &job->asset);
    if (status != UMI_STATUS_OK)
    {
        FreeJob(job);
        Message(panel, umi_status_text(status));
        return;
    }
    panel->busy = true;
    panel->pending = job->cancel;
    gtk_widget_set_sensitive(panel->form, FALSE);
    gtk_widget_set_sensitive(panel->stop, TRUE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
    Message(panel, opening ? "Reading a local SRT file..." : "Saving a new caption file...");
    GTask *task = g_task_new(NULL, NULL, Finished, NULL);
    g_task_set_task_data(task, job, FreeJob);
    g_task_run_in_thread(task, Worker);
    g_object_unref(task);
}
/* Buttons call shared operations; no timing, provider or file policy is copied
 * into Media or Music. New frontends can use the same immutable track API. */
#include "captions_rows_gtk4.inc"

static void Action(GtkButton *button, gpointer root)
{
    CaptionPanel *panel = Panel(root);
    if (!panel)
        return;
    const char *action = g_object_get_data(G_OBJECT(button), "caption-action");
    if (!strcmp(action, "stop"))
    {
        if (panel->pending)
            umi_cancellation_token_request(panel->pending);
        return;
    }
    if (panel->busy || panel->painting)
        return;
    /* A row operation uses the same prepared owner and publication path as
     * shift/clip. Ignore reentrant actions while filling the row draft. */
    if (panel->rowLoading) return;
    if (g_str_has_prefix(action, "row-")) {
        EditRow(panel, action);
        return;
    }
    if (!strcmp(action, "open"))
    {
        Start(panel, true, UMI_CAPTION_SRT);
        return;
    }
    if (!strcmp(action, "save-srt") || !strcmp(action, "save-vtt"))
    {
        if (!panel->track)
        {
            Message(panel, "Prepare the current captions first.");
            return;
        }
        Start(panel, false, !strcmp(action, "save-srt") ? UMI_CAPTION_SRT : UMI_CAPTION_WEBVTT);
        return;
    }
    if (!strcmp(action, "preview"))
    {
        Preview(panel);
        return;
    }
    UmiCaptionTrack *next = NULL;
    UmiStatus status = UMI_STATUS_OK;
    if (!strcmp(action, "prepare"))
    {
        char *text = Source(panel);
        /* Use the same reader for pasted drafts and opened files. The former SRT-only dispatch is retained for comparison. */
#if 0
        status = text ? UmiCaptionTrackParseSrt("Captions", text, strlen(text), &next)
                      : UMI_STATUS_CAPACITY_EXCEEDED;
#else
        status = text ? UmiCaptionTrackParse("Captions", text, strlen(text), &next)
                      : UMI_STATUS_CAPACITY_EXCEEDED;
#endif
        g_free(text);
    }
    else if (!panel->track)
        status = UMI_STATUS_INVALID_STATE;
    else if (!strcmp(action, "shift"))
        status = UmiCaptionTrackShift(
            panel->track, (int64_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->offset)),
            &next);
    else if (!strcmp(action, "clip"))
        status = UmiCaptionTrackClip(
            panel->track, (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->begin)),
            (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->end)),
            gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->rebase)), &next);
    else if (!strcmp(action, "undo"))
    {
        if (!panel->previous)
            status = UMI_STATUS_NOT_FOUND;
        else
            status = UmiCaptionTrackShift(panel->previous, 0, &next);
    }
    else
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
        status = Publish(panel, &next);
    UmiCaptionTrackDestroy(next);
    if (status != UMI_STATUS_OK)
        /* Explain the accepted interchange subset. Retain the SRT-only message for review. */
#if 0
        Message(panel, status == UMI_STATUS_PARSE_ERROR
                           ? "Check SRT numbering, timestamps and plain UTF-8 text. Formatting "
                             "tags are not supported."
                           : umi_status_text(status));
#else
        Message(panel, status == UMI_STATUS_PARSE_ERROR
                           ? "Check the caption header, numbering, timestamps and plain UTF-8 text. "
                             "WebVTT styles, settings, notes, named labels and markup are not supported."
                           : umi_status_text(status));
#endif
}
static GtkWidget *Button(CaptionPanel *panel, GtkWidget *box, const char *label, const char *action)
{
    GtkWidget *widget = gtk_button_new_with_label(label);
    char *id = g_strdup_printf("captions.%s", action);
    Tag(widget, id);
    g_free(id);
    g_object_set_data_full(G_OBJECT(widget), "caption-action", g_strdup(action), g_free);
    g_signal_connect_object(widget, "clicked", G_CALLBACK(Action), G_OBJECT(panel->root), 0);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}
static GtkWidget *Spin(CaptionPanel *panel, const char *label, const char *id, double low,
                       double value)
{
    Label(panel->form, label);
    GtkWidget *widget = gtk_spin_button_new_with_range(low, UMI_CAPTION_TIME_LIMIT_MS, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), value);
    Tag(widget, id);
    gtk_box_append(GTK_BOX(panel->form), widget);
    return widget;
}
static GtkWidget *View(GtkWidget *box, const char *id, bool editable)
{
    GtkWidget *view = gtk_text_view_new(), *scroll = gtk_scrolled_window_new();
    Tag(view, id);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), editable);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_size_request(scroll, -1, 160);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_box_append(GTK_BOX(box), scroll);
    return view;
}
/* The form keeps numbering one-based for readers while the shared API stays
 * zero-based. Loading is explicit, so typing never mutates the prepared track. */
static void BuildRowForm(CaptionPanel *panel)
{
    GtkWidget *expander = gtk_expander_new("Edit an individual caption");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_expander_set_child(GTK_EXPANDER(expander), box);
    gtk_box_append(GTK_BOX(panel->form), expander);
    Label(box, "Prepare captions first. Choose a row, then load it before replacing, removing, splitting or merging.");
    panel->rowNumber = gtk_spin_button_new_with_range(1, UMI_CAPTION_CUE_LIMIT + 1U, 1);
    Tag(panel->rowNumber, "captions.row-number");
    Label(box, "Caption number (count + 1 inserts at the end)");
    gtk_box_append(GTK_BOX(box), panel->rowNumber);
    Button(panel, box, "Load caption into form", "row-load");
    panel->rowBegin = gtk_spin_button_new_with_range(0, UMI_CAPTION_TIME_LIMIT_MS, 1);
    panel->rowEnd = gtk_spin_button_new_with_range(1, UMI_CAPTION_TIME_LIMIT_MS, 1);
    Tag(panel->rowBegin, "captions.row-begin");
    Tag(panel->rowEnd, "captions.row-end");
    Label(box, "Begins at millisecond"); gtk_box_append(GTK_BOX(box), panel->rowBegin);
    Label(box, "Ends at millisecond"); gtk_box_append(GTK_BOX(box), panel->rowEnd);
    Label(box, "Plain caption text");
    panel->rowText = View(box, "captions.row-text", true);
    Button(panel, box, "Replace loaded caption", "row-replace");
    Button(panel, box, "Insert before caption number", "row-insert");
    Button(panel, box, "Remove loaded caption", "row-remove");
    Label(box, "Split keeps the original time range. Text above belongs to the first half.");
    panel->rowSplit = gtk_spin_button_new_with_range(0, UMI_CAPTION_TIME_LIMIT_MS, 1);
    Tag(panel->rowSplit, "captions.row-split-time");
    Label(box, "Split at millisecond"); gtk_box_append(GTK_BOX(box), panel->rowSplit);
    Label(box, "Text for the second half");
    panel->rowSecondText = View(box, "captions.row-second-text", true);
    Button(panel, box, "Split loaded caption", "row-split");
    Label(box, "Merge joins the loaded and following captions, including any silence between them. Unsaved form text is not used.");
    Button(panel, box, "Merge loaded caption with next", "row-merge");
    Label(box, "Each successful edit can be reversed using Undo / redo last prepared change. Save a new subtitle file to keep it.");
}

UmiStatus UmiCaptionsGtkCreate(GtkWidget **out)
{
    if (!out || *out)
        return UMI_STATUS_INVALID_ARGUMENT;
    CaptionPanel *panel = g_new0(CaptionPanel, 1);
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    Tag(panel->root, "captions.panel");
    g_object_set_data_full(G_OBJECT(panel->root), CAPTION_PANEL_KEY, panel, Release);
    /* Describe both supported import formats; retain the earlier caption for review. */
#if 0
    Label(panel->root, "Captions: edit SRT text, adjust timing and export a subtitle file.");
#else
    Label(panel->root, "Captions: open SRT or WebVTT, adjust timing and export a subtitle file.");
#endif
    panel->form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    Tag(panel->form, "captions.form");
    gtk_box_append(GTK_BOX(panel->root), panel->form);
    Label(panel->form,
          "Each caption has a number, a timing line such as "
          "00:00:01,000 --> 00:00:03,000, then text. Separate captions with a blank line.");
    /* Canonical SRT keeps one editable representation after either import. */
    Label(panel->form, "You can also paste plain WebVTT. Preparing converts it to editable SRT. "
                       "Numeric labels are renumbered; styled captions and named labels are not supported.");
    panel->editor = View(panel->form, "captions.editor", true);
    Button(panel, panel->form, "Prepare captions", "prepare");
    BuildRowForm(panel);
    panel->offset = Spin(panel, "Shift all captions by milliseconds (negative moves earlier)",
                         "captions.offset", -(double)UMI_CAPTION_TIME_LIMIT_MS, 0);
    Button(panel, panel->form, "Apply time shift", "shift");
    panel->begin = Spin(panel, "Clip begins at millisecond", "captions.begin", 0, 0);
    panel->end = Spin(panel, "Clip ends at millisecond", "captions.end", 1, 30000);
    panel->rebase = gtk_check_button_new_with_label("Start the extracted clip timeline at zero");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->rebase), TRUE);
    Tag(panel->rebase, "captions.rebase");
    gtk_box_append(GTK_BOX(panel->form), panel->rebase);
    Button(panel, panel->form, "Extract captions for this clip", "clip");
    Button(panel, panel->form, "Undo / redo last prepared change", "undo");
    panel->time = Spin(panel, "Preview at millisecond", "captions.time", 0, 0);
    Button(panel, panel->form, "Preview active captions", "preview");
    panel->preview = View(panel->form, "captions.output", false);
    /* Opening now recognises WebVTT from its contents. Preserve the previous label for review. */
#if 0
    Label(panel->form, "Full local path to open .srt or save a new .srt / .vtt file");
#else
    Label(panel->form, "Full local path to open or save a new .srt / .vtt file");
#endif
    panel->path = gtk_entry_new();
    Tag(panel->path, "captions.path");
    gtk_box_append(GTK_BOX(panel->form), panel->path);
    /* Native selection changes only the draft path; Open and Save retain their
     * explicit approval, immutable input and exclusive-create behaviour. */
    GtkWidget *openPath = UmiPathPickerGtk4Create(GTK_ENTRY(panel->path),
        UMI_PATH_PICKER_EXISTING_FILE, "Choose a caption file to open", NULL);
    GtkWidget *savePath = UmiPathPickerGtk4Create(GTK_ENTRY(panel->path),
        UMI_PATH_PICKER_NEW_FILE, "Choose a new subtitle file", NULL);
    if (openPath != NULL) gtk_box_append(GTK_BOX(panel->form), openPath);
    if (savePath != NULL) gtk_box_append(GTK_BOX(panel->form), savePath);

    panel->approval = gtk_check_button_new_with_label("I saved my draft and approve replacing it");
    Tag(panel->approval, "captions.approval");
    gtk_box_append(GTK_BOX(panel->form), panel->approval);
    /* The same action accepts either plain caption format; keep the earlier button wording for review. */
#if 0
    Button(panel, panel->form, "Open SRT from path", "open");
#else
    Button(panel, panel->form, "Open SRT or WebVTT from path", "open");
#endif
    Button(panel, panel->form, "Save new SRT file", "save-srt");
    Button(panel, panel->form, "Save new WebVTT file", "save-vtt");
    panel->stop = Button(panel, panel->root, "Stop file operation", "stop");
    gtk_widget_set_sensitive(panel->stop, FALSE);
    panel->message =
        Label(panel->root,
              "Captions remain local. Save explicitly; scene saving does not save this draft.");
    Tag(panel->message, "captions.message");
    g_signal_connect_object(Buffer(panel->editor), "changed", G_CALLBACK(Changed),
                            G_OBJECT(panel->root), 0);
    g_signal_connect_object(panel->path, "changed", G_CALLBACK(PathChanged), G_OBJECT(panel->root),
                            0);
    *out = panel->root;
    return UMI_STATUS_OK;
}
