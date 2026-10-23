/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/video_sequence_gtk4.c
 * PURPOSE: Expose real sequence edits while keeping source media and recipe storage separate.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "video_sequence_private.h"
#include <string.h>
UmiVideoSequencePanel *UmiVideoSequenceGtkState(GtkWidget *root) {
  return g_object_get_data(G_OBJECT(root), "umicom-video-sequence");
}
void UmiVideoSequenceGtkMessage(UmiVideoSequencePanel *panel, const char *message) {
  if (!panel->closed)
    gtk_label_set_text(GTK_LABEL(panel->status), message);
}
static void Tag(GtkWidget *widget, const char *name) {
  g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(name), g_free);
}
static const char *Text(GtkWidget *entry) { return gtk_editable_get_text(GTK_EDITABLE(entry)); }
static uint32_t Number(GtkWidget *entry) {
  return (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(entry));
}
static UmiStatus Copy(char *out, size_t capacity, const char *text) {
  if (strlen(text) >= capacity)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  memcpy(out, text, strlen(text) + 1U);
  return UMI_STATUS_OK;
}
/* Copy a clip before notifying GTK observers. Changes to controls can cause
 * nested signals, so borrowed model storage must not escape into that work. */
static void LoadSelection(UmiVideoSequencePanel *panel) {
  if (panel->closed) return;
  const UmiVideoSequence *sequence = UmiVideoSequenceEditorRead(panel->editor);
  guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector));
  if ((size_t)selected >= sequence->clip_count)
    return;
  UmiVideoSequenceClip selectedClip = sequence->clips[selected];
  const UmiVideoSequenceClip *clip = &selectedClip;
  gtk_editable_set_text(GTK_EDITABLE(panel->id), clip->id);
  if (panel->closed) return;
  gtk_editable_set_text(GTK_EDITABLE(panel->path), clip->path);
  if (panel->closed) return;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->begin), clip->begin_ms);
  if (panel->closed) return;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->end), clip->end_ms);
  if (panel->closed) return;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->split),
                            clip->begin_ms + (clip->end_ms - clip->begin_ms) / 2U);
  if (panel->closed) return;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->position), selected + 1U);
}
static void Select(GtkDropDown *selector, GParamSpec *spec, gpointer root) {
  (void)selector;
  (void)spec;
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (panel->closed || panel->busy || panel->refreshing || panel->changing)
    return;
  panel->changing = true;
  LoadSelection(panel);
  panel->changing = false;
}
/* Replacing the list used to select a different clip after edits. The identity-
 * preserving refresh below supersedes this version; retain it for review. */
#if 0
void UmiVideoSequenceGtkRefresh(UmiVideoSequencePanel *panel) {
  if (panel->closed)
    return;
  const UmiVideoSequence *sequence = UmiVideoSequenceEditorRead(panel->editor);
  panel->refreshing = true;
  GtkStringList *items = gtk_string_list_new(NULL);
  GString *summary = g_string_new(NULL);
  uint32_t start = 0U;
  g_string_append_printf(summary, "%s — %u × %u; %zu clips. %s\n", sequence->title, sequence->width,
                         sequence->height, sequence->clip_count,
                         panel->dirty ? "Unsaved edits." : "Recipe saved or unchanged.");
  for (size_t i = 0U; i < sequence->clip_count; ++i) {
    const UmiVideoSequenceClip *clip = &sequence->clips[i];
    gtk_string_list_append(items, clip->id);
    g_string_append_printf(summary, "%zu. %s — timeline %u ms; source [%u, %u) ms\n%s\n", i + 1U,
                           clip->id, start, clip->begin_ms, clip->end_ms, clip->path);
    start += clip->end_ms - clip->begin_ms;
  }
  g_string_append_printf(summary, "Assembled duration: %u ms. Render output is silent video.",
                         start);
  gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->summary)), summary->str,
                           -1);
  g_string_free(summary, TRUE);
  gtk_drop_down_set_model(GTK_DROP_DOWN(panel->selector), G_LIST_MODEL(items));
  g_object_unref(items);
  gtk_editable_set_text(GTK_EDITABLE(panel->title), sequence->title);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->width), sequence->width);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->height), sequence->height);
  LoadSelection(panel);
  panel->refreshing = false;
}
#endif

void UmiVideoSequenceGtkRefresh(UmiVideoSequencePanel *panel) {
  if (panel->closed)
    return;
  /* Own the value across GTK notifications; a retired editor must not leave
   * a borrowed recipe driving later widget updates. */
  UmiVideoSequence snapshot = *UmiVideoSequenceEditorRead(panel->editor);
  const UmiVideoSequence *sequence = &snapshot;
  /* Keep identity from the old list before replacing its model. A move or
   * removal has already changed the sequence, so its old numeric index is
   * not a reliable identity. Copy GTK's borrowed label before notifications. */
  guint oldPosition = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector));
  GtkStringObject *selected = gtk_drop_down_get_selected_item(GTK_DROP_DOWN(panel->selector));
  char *selectedId = selected != NULL ? g_strdup(gtk_string_object_get_string(selected)) : NULL;
  panel->refreshing = true;
  GtkStringList *items = gtk_string_list_new(NULL);
  GString *summary = g_string_new(NULL);
  uint32_t start = 0U;
  g_string_append_printf(summary, "%s — %u × %u; %zu clips. %s\n", sequence->title, sequence->width,
                         sequence->height, sequence->clip_count,
                         panel->dirty ? "Unsaved edits." : "Recipe saved or unchanged.");
  for (size_t i = 0U; i < sequence->clip_count; ++i) {
    const UmiVideoSequenceClip *clip = &sequence->clips[i];
    gtk_string_list_append(items, clip->id);
    g_string_append_printf(summary, "%zu. %s — timeline %u ms; source [%u, %u) ms\n%s\n", i + 1U,
                           clip->id, start, clip->begin_ms, clip->end_ms, clip->path);
    start += clip->end_ms - clip->begin_ms;
  }
  g_string_append_printf(summary, "Assembled duration: %u ms. Render output is silent video.",
                         start);
  gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->summary)), summary->str,
                           -1);
  g_string_free(summary, TRUE);
  if (panel->closed) { g_object_unref(items); g_free(selectedId); goto finished; }
  gtk_drop_down_set_model(GTK_DROP_DOWN(panel->selector), G_LIST_MODEL(items));
  g_object_unref(items);
  if (panel->closed) { g_free(selectedId); goto finished; }
  guint position = GTK_INVALID_LIST_POSITION;
  for (size_t i = 0U; selectedId != NULL && i < sequence->clip_count; ++i)
    if (strcmp(sequence->clips[i].id, selectedId) == 0)
      position = (guint)i;
  if (position == GTK_INVALID_LIST_POSITION && sequence->clip_count != 0U)
    position = oldPosition < sequence->clip_count ? oldPosition : (guint)sequence->clip_count - 1U;
  if (selectedId == NULL && sequence->clip_count != 0U)
    position = 0U;
  gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->selector), position);
  g_free(selectedId);
  if (panel->closed) goto finished;
  gtk_editable_set_text(GTK_EDITABLE(panel->title), sequence->title);
  if (panel->closed) goto finished;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->width), sequence->width);
  if (panel->closed) goto finished;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->height), sequence->height);
  if (panel->closed) goto finished;
  LoadSelection(panel);
finished:
  panel->refreshing = false;
}

/* Buttons share the Framework model. Form changes do not silently alter a
 * recipe: Add, Replace or Apply settings publishes one undoable operation. */
static void Edit(GtkButton *button, gpointer root) {
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (panel->closed || panel->busy || panel->refreshing || panel->changing)
    return;
  panel->changing = true;
  const char *action = g_object_get_data(G_OBJECT(button), "video-action");
  UmiVideoSequence value = *UmiVideoSequenceEditorRead(panel->editor);
  UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
  if (strcmp(action, "undo") == 0)
    status = UmiVideoSequenceEditorUndo(panel->editor);
  else if (strcmp(action, "redo") == 0)
    status = UmiVideoSequenceEditorRedo(panel->editor);
  else {
    if (strcmp(action, "settings") == 0) {
      status = Copy(value.title, sizeof(value.title), Text(panel->title));
      value.width = Number(panel->width);
      value.height = Number(panel->height);
    } else if (strcmp(action, "add") == 0 || strcmp(action, "replace") == 0) {
      UmiVideoSequenceClip clip = {0};
      status = Copy(clip.id, sizeof(clip.id), Text(panel->id));
      if (status == UMI_STATUS_OK)
        status = Copy(clip.path, sizeof(clip.path), Text(panel->path));
      clip.begin_ms = Number(panel->begin);
      clip.end_ms = Number(panel->end);
      if (status == UMI_STATUS_OK)
        status = UmiVideoSequencePut(&value, &clip, strcmp(action, "replace") == 0);
    } else if (strcmp(action, "remove") == 0)
      status = UmiVideoSequenceRemove(&value, Text(panel->id));
    else if (strcmp(action, "move") == 0)
      status = UmiVideoSequenceMove(&value, Text(panel->id), (size_t)Number(panel->position) - 1U);
    else if (strcmp(action, "split") == 0)
      status = UmiVideoSequenceSplit(&value, Text(panel->id), Number(panel->split),
                                     Text(panel->rightId));
    if (status == UMI_STATUS_OK)
      status = UmiVideoSequenceEditorApply(panel->editor, &value);
  }
  if (status == UMI_STATUS_OK) {
    UmiVideoSequenceGtkStopPreview(panel);
    panel->dirty = !UmiVideoSequenceEqual(UmiVideoSequenceEditorRead(panel->editor), &panel->saved);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
    UmiVideoSequenceGtkRefresh(panel);
    UmiVideoSequenceGtkMessage(
        panel, panel->dirty ? "Edit applied. Save the recipe to retain it after closing."
                            : "This recipe matches the saved or initial state.");
  } else {
    char *message = g_strdup_printf("Edit not applied: %s. Check the ID, time range and capacity.",
                                    umi_status_text(status));
    UmiVideoSequenceGtkMessage(panel, message);
    g_free(message);
  }
  panel->changing = false;
}
static void Cancel(GtkButton *button, gpointer root) {
  (void)button;
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (!panel->closed && panel->cancel != NULL) {
    umi_cancellation_token_request(panel->cancel);
    UmiVideoSequenceGtkMessage(panel, "Cancellation requested. A new partial output may remain.");
  }
}
void UmiVideoSequenceGtk4Retire(GtkWidget *root) {
  if (root == NULL)
    return;
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(root);
  if (panel == NULL || panel->closed)
    return;
  /* Mark closed before notifications: observers may retain and click controls
   * while the host window is being removed. Their handlers then do nothing. */
  panel->closed = true;
  if (panel->cancel != NULL)
    umi_cancellation_token_request(panel->cancel);
  UmiVideoSequenceGtkStopPreview(panel);
  gtk_widget_set_sensitive(root, FALSE);
}
static void Dispose(gpointer data) {
  UmiVideoSequencePanel *panel = data;
  if (panel->cancel != NULL)
    umi_cancellation_token_request(panel->cancel);
  /* Preview signals have object-bound closures; disconnect without touching
   * already disposed child widgets when the root's final reference goes. */
  if (panel->stream != NULL) {
    g_signal_handlers_disconnect_by_data(panel->stream, panel->root);
    gtk_media_stream_pause(panel->stream);
    g_object_unref(panel->stream);
  }
  UmiVideoSequenceEditorDestroy(panel->editor);
  g_free(panel);
}
static GtkWidget *Label(GtkWidget *box, const char *text) {
  GtkWidget *label = gtk_label_new(text);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
  gtk_label_set_wrap(GTK_LABEL(label), TRUE);
  gtk_box_append(GTK_BOX(box), label);
  return label;
}
static GtkWidget *Entry(GtkWidget *box, const char *label, const char *id) {
  Label(box, label);
  GtkWidget *entry = gtk_entry_new();
  Tag(entry, id);
  gtk_entry_set_max_length(GTK_ENTRY(entry), (int)UMI_PATH_CAPACITY - 1);
  gtk_box_append(GTK_BOX(box), entry);
  return entry;
}
static GtkWidget *Spin(GtkWidget *box, const char *label, const char *id, double low, double high) {
  Label(box, label);
  GtkWidget *spin = gtk_spin_button_new_with_range(low, high, 1.0);
  Tag(spin, id);
  gtk_box_append(GTK_BOX(box), spin);
  return spin;
}
static void Browse(GtkWidget *box, GtkWidget *entry, UmiPathPickerMode mode, const char *title) {
  gtk_box_append(GTK_BOX(box), UmiPathPickerGtk4Create(GTK_ENTRY(entry), mode, title, NULL));
}
static GtkWidget *Button(UmiVideoSequencePanel *panel, GtkWidget *box, const char *label,
                         const char *id, const char *action, GCallback callback) {
  GtkWidget *button = gtk_button_new_with_label(label);
  Tag(button, id);
  g_object_set_data_full(G_OBJECT(button), "video-action", g_strdup(action), g_free);
  g_signal_connect_object(button, "clicked", callback, panel->root, 0);
  gtk_box_append(GTK_BOX(box), button);
  return button;
}
GtkWidget *UmiVideoSequenceGtk4Create(void) {
  UmiVideoSequence initial = {0};
  UmiVideoSequencePanel *panel = g_new0(UmiVideoSequencePanel, 1);
  if (UmiVideoSequenceInit("Video sequence", 1280U, 720U, &initial) != UMI_STATUS_OK ||
      UmiVideoSequenceEditorCreate(&initial, &panel->editor) != UMI_STATUS_OK) {
    g_free(panel);
    return NULL;
  }
  panel->saved = initial;
  panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  Tag(panel->root, "video-sequence");
  g_object_set_data_full(G_OBJECT(panel->root), "umicom-video-sequence", panel, Dispose);
  Label(panel->root,
        "Build a video sequence from local clips. Source files stay unchanged. "
        "Up to six clips and ten minutes; silent 30 fps MP4 export. Choose all storage locations.");
  panel->form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_box_append(GTK_BOX(panel->root), panel->form);
  GtkWidget *settings = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  GtkWidget *expander = gtk_expander_new("Sequence settings");
  gtk_expander_set_child(GTK_EXPANDER(expander), settings);
  gtk_box_append(GTK_BOX(panel->form), expander);
  panel->title = Entry(settings, "Title", "video.title");
  panel->width = Spin(settings, "Output width (even pixels)", "video.width", 16.0, 1920.0);
  panel->height = Spin(settings, "Output height (even pixels)", "video.height", 16.0, 1080.0);
  Button(panel, settings, "Apply settings", "video.settings", "settings", G_CALLBACK(Edit));
  panel->selector = gtk_drop_down_new(NULL, NULL);
  Tag(panel->selector, "video.clips");
  gtk_box_append(GTK_BOX(panel->form), panel->selector);
  g_signal_connect_object(panel->selector, "notify::selected", G_CALLBACK(Select), panel->root, 0);
  panel->id = Entry(panel->form, "Clip ID (use a new ID when adding)", "video.id");
  panel->path = Entry(panel->form, "Local video file", "video.path");
  Browse(panel->form, panel->path, UMI_PATH_PICKER_EXISTING_FILE, "Choose a video clip");
  panel->begin = Spin(panel->form, "Source start (milliseconds)", "video.begin", 0.0, 600000.0);
  panel->end = Spin(panel->form, "Source end (milliseconds, excluded)", "video.end", 1.0, 600000.0);
  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  gtk_box_append(GTK_BOX(panel->form), row);
  Button(panel, row, "Add", "video.add", "add", G_CALLBACK(Edit));
  Button(panel, row, "Replace / trim", "video.replace", "replace", G_CALLBACK(Edit));
  Button(panel, row, "Remove", "video.remove", "remove", G_CALLBACK(Edit));
  Button(panel, row, "Undo", "video.undo", "undo", G_CALLBACK(Edit));
  Button(panel, row, "Redo", "video.redo", "redo", G_CALLBACK(Edit));
  GtkWidget *editing = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  expander = gtk_expander_new("Split and reorder");
  gtk_expander_set_child(GTK_EXPANDER(expander), editing);
  gtk_box_append(GTK_BOX(panel->form), expander);
  panel->split =
      Spin(editing, "Split at source time (milliseconds)", "video.split-time", 0.0, 600000.0);
  panel->rightId = Entry(editing, "New ID for the right part", "video.right-id");
  Button(panel, editing, "Split clip", "video.split", "split", G_CALLBACK(Edit));
  panel->position = Spin(editing, "New position (starts at 1)", "video.position", 1.0, 6.0);
  Button(panel, editing, "Move clip", "video.move", "move", G_CALLBACK(Edit));
  /* Relinking changes only committed recipe references. It is separate from
   * Replace / trim so draft timing fields cannot accidentally change a cut. */
  GtkWidget *relink = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  expander = gtk_expander_new("Relink source media");
  gtk_expander_set_child(GTK_EXPANDER(expander), relink);
  gtk_box_append(GTK_BOX(panel->form), expander);
  Label(relink, "Select a clip above, then choose its replacement file. Timing and order stay "
                "unchanged. Preview the replacement to check its content and duration.");
  panel->relinkPath = Entry(relink, "Replacement local video", "video.relink-path");
  Browse(relink, panel->relinkPath, UMI_PATH_PICKER_EXISTING_FILE, "Choose replacement video");
  panel->relinkAll = gtk_check_button_new_with_label("Relink every clip with the same source path");
  Tag(panel->relinkAll, "video.relink-all");
  gtk_box_append(GTK_BOX(relink), panel->relinkAll);
  Button(panel, relink, "Relink source", "video.relink", "relink", G_CALLBACK(UmiVideoSequenceGtkRelink));
  panel->summary = gtk_text_view_new();
  Tag(panel->summary, "video.summary");
  gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->summary), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(panel->summary), GTK_WRAP_WORD_CHAR);
  gtk_box_append(GTK_BOX(panel->form), panel->summary);
  panel->video = gtk_video_new();
  gtk_widget_set_size_request(panel->video, 320, 180);
  gtk_box_append(GTK_BOX(panel->form), panel->video);
  Label(panel->form,
        "Preview plays the saved clip range silently. GTK media codecs must be installed. "
        "Unsaved form values require Replace / trim first.");
  Button(panel, panel->form, "Preview selected clip", "video.preview", "preview",
         G_CALLBACK(UmiVideoSequenceGtkPreview));
  Button(panel, panel->form, "Stop preview", "video.stop", "stop",
         G_CALLBACK(UmiVideoSequenceGtkPreview));
  panel->projectPath = Entry(panel->form, "Recipe JSON path", "video.recipe");
  Browse(panel->form, panel->projectPath, UMI_PATH_PICKER_NEW_FILE, "Choose a recipe path");
  panel->approval =
      gtk_check_button_new_with_label("Replace my current edits when opening a recipe");
  Tag(panel->approval, "video.approve-open");
  gtk_box_append(GTK_BOX(panel->form), panel->approval);
  Button(panel, panel->form, "Open recipe", "video.open", "open",
         G_CALLBACK(UmiVideoSequenceGtkJob));
  Button(panel, panel->form, "Save new recipe", "video.save", "save",
         G_CALLBACK(UmiVideoSequenceGtkJob));
  panel->program = Entry(panel->form, "Trusted FFmpeg executable (with libx264)", "video.program");
  Browse(panel->form, panel->program, UMI_PATH_PICKER_EXISTING_FILE, "Choose FFmpeg");
  panel->output = Entry(panel->form, "New silent MP4 output path", "video.output");
  Browse(panel->form, panel->output, UMI_PATH_PICKER_NEW_FILE, "Choose a new MP4 output");
  Button(panel, panel->form, "Render silent MP4", "video.render", "render",
         G_CALLBACK(UmiVideoSequenceGtkJob));
  /* Soundtrack assembly is a separate export, so reopening a silent recipe
   * cannot accidentally publish audio or replace an existing rendered video. */
  GtkWidget *soundtrack = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  expander = gtk_expander_new("Add soundtrack to a rendered video");
  gtk_expander_set_child(GTK_EXPANDER(expander), soundtrack);
  gtk_box_append(GTK_BOX(panel->form), expander);
  Label(soundtrack,
        "Choose a rendered MP4 and an audio file. This creates a new MP4. "
        "Audio starts at the beginning and replaces existing audio; short audio ends in silence.");
  panel->soundtrackVideo = Entry(soundtrack, "Rendered video file", "video.soundtrack-video");
  Browse(soundtrack, panel->soundtrackVideo, UMI_PATH_PICKER_EXISTING_FILE,
         "Choose rendered video");
  panel->soundtrackAudio = Entry(soundtrack, "Local soundtrack file", "video.soundtrack-audio");
  Browse(soundtrack, panel->soundtrackAudio, UMI_PATH_PICKER_EXISTING_FILE, "Choose soundtrack");
  panel->soundtrackOutput = Entry(soundtrack, "New video with sound", "video.soundtrack-output");
  Browse(soundtrack, panel->soundtrackOutput, UMI_PATH_PICKER_NEW_FILE, "Choose new video output");
  Button(panel, soundtrack, "Export with soundtrack", "video.soundtrack", "soundtrack",
         G_CALLBACK(UmiVideoSequenceGtkJob));
  GtkWidget *captions = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  expander = gtk_expander_new("Add captions to a rendered video");
  gtk_expander_set_child(GTK_EXPANDER(expander), captions);
  gtk_box_append(GTK_BOX(panel->form), expander);
  Label(captions, "Choose a rendered MP4 and UTF-8 SRT captions exported from the captions editor. "
                  "This creates a selectable subtitle track, not burned-in text. Check cue timing "
                  "against the video.");
  panel->captionVideo = Entry(captions, "Video file for captions", "video.caption-video");
  Browse(captions, panel->captionVideo, UMI_PATH_PICKER_EXISTING_FILE, "Choose video for captions");
  panel->captionSource = Entry(captions, "SubRip caption file", "video.caption-source");
  Browse(captions, panel->captionSource, UMI_PATH_PICKER_EXISTING_FILE, "Choose SRT captions");
  panel->captionOutput = Entry(captions, "New video with captions", "video.caption-output");
  Browse(captions, panel->captionOutput, UMI_PATH_PICKER_NEW_FILE, "Choose captioned video output");
  Button(panel, captions, "Export with captions", "video.captions", "captions",
         G_CALLBACK(UmiVideoSequenceGtkJob));
  panel->cancelButton =
      Button(panel, panel->root, "Cancel operation", "video.cancel", "cancel", G_CALLBACK(Cancel));
  gtk_widget_set_sensitive(panel->cancelButton, FALSE);
  panel->status = Label(panel->root, "Add clips, then save the recipe or render a new video.");
  Tag(panel->status, "video.status");
  UmiVideoSequenceGtkRefresh(panel);
  return panel->root;
}
