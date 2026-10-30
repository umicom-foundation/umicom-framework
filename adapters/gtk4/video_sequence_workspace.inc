/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/video_sequence_workspace.inc
 *
 * PURPOSE:
 *   Compose persistent source, preview, inspector and timeline regions over the shared video editor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* This composition owns presentation only. Editing, persistence, rendering and
 * cancellation continue through the existing Framework callbacks above. */
static GtkWidget *WorkspaceScroll(GtkWidget *child) {
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC,
                                GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), child);
  gtk_widget_set_hexpand(scroll, TRUE);
  gtk_widget_set_vexpand(scroll, TRUE);
  return scroll;
}
static GtkWidget *WorkspacePage(GtkWidget *book, const char *title) {
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_widget_set_margin_start(box, 10); gtk_widget_set_margin_end(box, 10);
  gtk_widget_set_margin_top(box, 10); gtk_widget_set_margin_bottom(box, 10);
  gtk_notebook_append_page(GTK_NOTEBOOK(book), WorkspaceScroll(box), gtk_label_new(title));
  return box;
}
static GtkWidget *WorkspaceActions(GtkWidget *box) {
  GtkWidget *actions = UmiGtk4ActionGroupCreate();
  gtk_box_append(GTK_BOX(box), actions);
  return actions;
}
static void TimelineSelect(GtkButton *button, gpointer root) {
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (panel->closed || panel->busy || panel->changing || panel->refreshing) return;
  guint index = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "video-clip-index"));
  const UmiVideoSequence *sequence = UmiVideoSequenceEditorRead(panel->editor);
  if ((size_t)index < sequence->clip_count)
    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->selector), index);
}
/* Retain the earlier row-only layout for review. The replacement below adds
 * a searchable library and task navigation without replacing its edit owner. */
#if 0
static void RefreshTimeline(UmiVideoSequencePanel *panel) {
  if (panel->closed || panel->durationLabel == NULL) return;
  UmiVideoSequence value = *UmiVideoSequenceEditorRead(panel->editor);
  guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector));
  uint32_t cursor = 0U;
  for (size_t i = 0U; i < UMI_VIDEO_SEQUENCE_MAX_CLIPS && !panel->closed; ++i) {
    GtkWidget *button = panel->clipButtons[i];
    gboolean visible = i < value.clip_count;
    gtk_widget_set_visible(button, visible);
    if (panel->closed) return;
    if (!visible) continue;
    const UmiVideoSequenceClip *clip = &value.clips[i];
    uint32_t end = cursor + clip->end_ms - clip->begin_ms;
    char *label = g_strdup_printf("%s\n%.2f - %.2f s", clip->id,
                                  (double)cursor / 1000.0, (double)end / 1000.0);
    gtk_button_set_label(GTK_BUTTON(button), label);
    g_free(label);
    if (panel->closed) return;
    gtk_widget_set_tooltip_text(button, clip->path);
    if (panel->closed) return;
    if (i == selected) gtk_widget_add_css_class(button, "suggested-action");
    else gtk_widget_remove_css_class(button, "suggested-action");
    cursor = end;
  }
  if (panel->closed) return;
  gtk_widget_set_visible(panel->emptyTimeline, value.clip_count == 0U);
  if (panel->closed) return;
  char *duration = g_strdup_printf("%zu clips | %.2f seconds | %s", value.clip_count,
      (double)cursor / 1000.0, panel->dirty ? "Unsaved recipe" : "Recipe unchanged");
  gtk_label_set_text(GTK_LABEL(panel->durationLabel), duration);
  g_free(duration);
}

GtkWidget *UmiVideoSequenceGtk4Create(void) {
  UmiVideoSequence initial = {0};
  UmiVideoSequencePanel *panel = g_new0(UmiVideoSequencePanel, 1);
  if (UmiVideoSequenceInit("Video sequence", 1280U, 720U, &initial) != UMI_STATUS_OK ||
      UmiVideoSequenceEditorCreate(&initial, &panel->editor) != UMI_STATUS_OK) {
    g_free(panel); return NULL;
  }
  panel->saved = initial;
  panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  Tag(panel->root, "video-sequence");
  g_object_set_data_full(G_OBJECT(panel->root), "umicom-video-sequence", panel, Dispose);
  gtk_widget_set_hexpand(panel->root, TRUE); gtk_widget_set_vexpand(panel->root, TRUE);
  panel->form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_widget_set_vexpand(panel->form, TRUE);
  gtk_box_append(GTK_BOX(panel->root), panel->form);
  GtkWidget *toolbar = WorkspaceActions(panel->form);
  Button(panel, toolbar, "Undo", "video.undo", "undo", G_CALLBACK(Edit));
  Button(panel, toolbar, "Redo", "video.redo", "redo", G_CALLBACK(Edit));
  Button(panel, toolbar, "Save new recipe", "video.save", "save", G_CALLBACK(UmiVideoSequenceGtkJob));
  Label(panel->form, "Local video editing | Up to six clips and ten minutes | Source files stay unchanged");

  GtkWidget *vertical = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
  Tag(vertical, "video.workspace.vertical");
  gtk_widget_set_vexpand(vertical, TRUE);
  gtk_box_append(GTK_BOX(panel->form), vertical);
  gtk_paned_set_position(GTK_PANED(vertical), 440);
  GtkWidget *horizontal = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  Tag(horizontal, "video.workspace.sources");
  gtk_paned_set_start_child(GTK_PANED(vertical), horizontal);
  GtkWidget *sourceBook = gtk_notebook_new();
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(sourceBook), TRUE);
  gtk_widget_set_size_request(sourceBook, 220, -1);
  gtk_paned_set_start_child(GTK_PANED(horizontal), sourceBook);
  gtk_paned_set_position(GTK_PANED(horizontal), 250);
  gtk_paned_set_resize_start_child(GTK_PANED(horizontal), FALSE);
  gtk_paned_set_shrink_start_child(GTK_PANED(horizontal), FALSE);
  gtk_paned_set_shrink_end_child(GTK_PANED(horizontal), FALSE);
  GtkWidget *sources = WorkspacePage(sourceBook, "Clips");
  Tag(sources, "video.sources");
  Label(sources, "Select an added clip to inspect or preview it.");
  panel->selector = gtk_drop_down_new(NULL, NULL);
  Tag(panel->selector, "video.clips");
  gtk_box_append(GTK_BOX(sources), panel->selector);
  g_signal_connect_object(panel->selector, "notify::selected", G_CALLBACK(Select), panel->root, 0);
  Label(sources, "To add media, enter a new clip ID and choose its file. Set its source range in the inspector, then choose Add clip.");
  panel->id = Entry(sources, "Clip ID", "video.id");
  panel->path = Entry(sources, "Local video file", "video.path");
  Browse(sources, panel->path, UMI_PATH_PICKER_EXISTING_FILE, "Choose a video clip");
  Button(panel, sources, "Add clip", "video.add", "add", G_CALLBACK(Edit));
  GtkWidget *project = WorkspacePage(sourceBook, "Project");
  panel->title = Entry(project, "Sequence title", "video.title");
  panel->width = Spin(project, "Output width (even pixels)", "video.width", 16.0, 1920.0);
  panel->height = Spin(project, "Output height (even pixels)", "video.height", 16.0, 1080.0);
  Button(panel, project, "Apply settings", "video.settings", "settings", G_CALLBACK(Edit));
  panel->projectPath = Entry(project, "Recipe JSON path", "video.recipe");
  Browse(project, panel->projectPath, UMI_PATH_PICKER_NEW_FILE, "Choose a recipe path");
  Label(project, "Save new recipe writes a new file. Opening an existing recipe requires permission to replace current edits.");
  panel->approval = gtk_check_button_new_with_label("Replace my current edits when opening a recipe");
  Tag(panel->approval, "video.approve-open");
  gtk_box_append(GTK_BOX(project), panel->approval);
  Button(panel, project, "Open recipe", "video.open", "open", G_CALLBACK(UmiVideoSequenceGtkJob));

  GtkWidget *right = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  Tag(right, "video.workspace.inspector");
  gtk_paned_set_end_child(GTK_PANED(horizontal), right);
  gtk_paned_set_position(GTK_PANED(right), 580);
  gtk_paned_set_resize_end_child(GTK_PANED(right), FALSE);
  gtk_paned_set_shrink_start_child(GTK_PANED(right), FALSE);
  gtk_paned_set_shrink_end_child(GTK_PANED(right), FALSE);
  GtkWidget *preview = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  Tag(preview, "video.preview-region");
  gtk_widget_set_hexpand(preview, TRUE); gtk_widget_set_vexpand(preview, TRUE);
  gtk_widget_set_size_request(preview, 240, 180);
  gtk_paned_set_start_child(GTK_PANED(right), preview);
  GtkWidget *heading = Label(preview, "Preview");
  gtk_widget_add_css_class(heading, "heading");
  panel->video = gtk_video_new();
  Tag(panel->video, "video.player");
  gtk_widget_set_hexpand(panel->video, TRUE); gtk_widget_set_vexpand(panel->video, TRUE);
  gtk_box_append(GTK_BOX(preview), panel->video);
  Label(preview, "Preview plays the applied clip range silently. Playback needs the installed media backend.");
  GtkWidget *transport = WorkspaceActions(preview);
  Button(panel, transport, "Preview selected clip", "video.preview", "preview", G_CALLBACK(UmiVideoSequenceGtkPreview));
  Button(panel, transport, "Stop preview", "video.stop", "stop", G_CALLBACK(UmiVideoSequenceGtkPreview));
  GtkWidget *inspectorBook = gtk_notebook_new();
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(inspectorBook), TRUE);
  gtk_widget_set_size_request(inspectorBook, 260, -1);
  gtk_paned_set_end_child(GTK_PANED(right), inspectorBook);
  GtkWidget *inspector = WorkspacePage(inspectorBook, "Inspector");
  Tag(inspector, "video.inspector");
  Label(inspector, "Source range uses milliseconds. Apply trim before previewing; changing a field alone does not edit the recipe.");
  panel->begin = Spin(inspector, "Source start", "video.begin", 0.0, 600000.0);
  panel->end = Spin(inspector, "Source end (excluded)", "video.end", 1.0, 600000.0);
  GtkWidget *edits = WorkspaceActions(inspector);
  Button(panel, edits, "Apply trim", "video.replace", "replace", G_CALLBACK(Edit));
  Button(panel, edits, "Remove clip", "video.remove", "remove", G_CALLBACK(Edit));
  panel->split = Spin(inspector, "Split at source time", "video.split-time", 0.0, 600000.0);
  panel->rightId = Entry(inspector, "New ID for right part", "video.right-id");
  Button(panel, inspector, "Split clip", "video.split", "split", G_CALLBACK(Edit));
  panel->position = Spin(inspector, "New position (starts at 1)", "video.position", 1.0, 6.0);
  Button(panel, inspector, "Move clip", "video.move", "move", G_CALLBACK(Edit));
  GtkWidget *relink = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  GtkWidget *expander = gtk_expander_new("Relink source media");
  gtk_expander_set_child(GTK_EXPANDER(expander), relink);
  gtk_box_append(GTK_BOX(inspector), expander);
  Label(relink, "Replace references while retaining timing and order. Preview the replacement before rendering.");
  panel->relinkPath = Entry(relink, "Replacement local video", "video.relink-path");
  Browse(relink, panel->relinkPath, UMI_PATH_PICKER_EXISTING_FILE, "Choose replacement video");
  panel->relinkAll = gtk_check_button_new_with_label("Relink every clip with this source path");
  Tag(panel->relinkAll, "video.relink-all");
  gtk_box_append(GTK_BOX(relink), panel->relinkAll);
  Button(panel, relink, "Relink source", "video.relink", "relink", G_CALLBACK(UmiVideoSequenceGtkRelink));

  GtkWidget *exports = WorkspacePage(inspectorBook, "Export");
  panel->program = Entry(exports, "Trusted FFmpeg executable (with libx264)", "video.program");
  Browse(exports, panel->program, UMI_PATH_PICKER_EXISTING_FILE, "Choose FFmpeg");
  panel->output = Entry(exports, "New silent MP4 output path", "video.output");
  Browse(exports, panel->output, UMI_PATH_PICKER_NEW_FILE, "Choose a new MP4 output");
  Label(exports, "Render produces silent 30 fps MP4. Existing output files are never overwritten.");
  Button(panel, exports, "Render silent MP4", "video.render", "render", G_CALLBACK(UmiVideoSequenceGtkJob));
  GtkWidget *soundtrack = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  expander = gtk_expander_new("Add soundtrack to a rendered video");
  gtk_expander_set_child(GTK_EXPANDER(expander), soundtrack);
  gtk_box_append(GTK_BOX(exports), expander);
  Label(soundtrack, "This creates a new video. Audio starts at the beginning and replaces existing audio; short audio ends in silence.");
  panel->soundtrackVideo = Entry(soundtrack, "Rendered video", "video.soundtrack-video");
  Browse(soundtrack, panel->soundtrackVideo, UMI_PATH_PICKER_EXISTING_FILE, "Choose rendered video");
  panel->soundtrackAudio = Entry(soundtrack, "Soundtrack", "video.soundtrack-audio");
  Browse(soundtrack, panel->soundtrackAudio, UMI_PATH_PICKER_EXISTING_FILE, "Choose soundtrack");
  panel->soundtrackOutput = Entry(soundtrack, "New video with sound", "video.soundtrack-output");
  Browse(soundtrack, panel->soundtrackOutput, UMI_PATH_PICKER_NEW_FILE, "Choose new video output");
  Button(panel, soundtrack, "Export with soundtrack", "video.soundtrack", "soundtrack", G_CALLBACK(UmiVideoSequenceGtkJob));
  GtkWidget *captions = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  expander = gtk_expander_new("Add captions to a rendered video");
  gtk_expander_set_child(GTK_EXPANDER(expander), captions);
  gtk_box_append(GTK_BOX(exports), expander);
  Label(captions, "Add a selectable subtitle track from UTF-8 SRT. Check cue timing against the rendered video.");
  panel->captionVideo = Entry(captions, "Rendered video", "video.caption-video");
  Browse(captions, panel->captionVideo, UMI_PATH_PICKER_EXISTING_FILE, "Choose video for captions");
  panel->captionSource = Entry(captions, "SubRip captions", "video.caption-source");
  Browse(captions, panel->captionSource, UMI_PATH_PICKER_EXISTING_FILE, "Choose SRT captions");
  panel->captionOutput = Entry(captions, "New video with captions", "video.caption-output");
  Browse(captions, panel->captionOutput, UMI_PATH_PICKER_NEW_FILE, "Choose captioned output");
  Button(panel, captions, "Export with captions", "video.captions", "captions", G_CALLBACK(UmiVideoSequenceGtkJob));

  GtkWidget *timeline = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  Tag(timeline, "video.timeline");
  gtk_paned_set_end_child(GTK_PANED(vertical), timeline);
  panel->durationLabel = Label(timeline, "Timeline");
  Tag(panel->durationLabel, "video.duration");
  panel->emptyTimeline = Label(timeline, "No clips yet. Choose a local file in Clips, set the source range, then choose Add clip.");
  GtkWidget *clips = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  GtkWidget *clipScroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(clipScroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(clipScroll), 76);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(clipScroll), clips);
  gtk_box_append(GTK_BOX(timeline), clipScroll);
  /* Buttons are allocated once. A refresh changes labels and selection but
   * never destroys the focused clip control or dispatches an edit. */
  for (guint i = 0U; i < UMI_VIDEO_SEQUENCE_MAX_CLIPS; ++i) {
    GtkWidget *button = gtk_button_new();
    panel->clipButtons[i] = button;
    char id[48]; (void)g_snprintf(id, sizeof(id), "video.timeline.clip.%u", i);
    Tag(button, id);
    gtk_widget_set_size_request(button, 140, 60);
    g_object_set_data(G_OBJECT(button), "video-clip-index", GUINT_TO_POINTER(i));
    g_signal_connect_object(button, "clicked", G_CALLBACK(TimelineSelect), panel->root, 0);
    gtk_box_append(GTK_BOX(clips), button);
  }
  panel->summary = gtk_text_view_new();
  Tag(panel->summary, "video.summary");
  gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->summary), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(panel->summary), GTK_WRAP_WORD_CHAR);
  expander = gtk_expander_new("Sequence details");
  gtk_expander_set_child(GTK_EXPANDER(expander), WorkspaceScroll(panel->summary));
  gtk_box_append(GTK_BOX(timeline), expander);
  panel->cancelButton = Button(panel, panel->root, "Cancel operation", "video.cancel", "cancel", G_CALLBACK(Cancel));
  gtk_widget_set_sensitive(panel->cancelButton, FALSE);
  panel->status = Label(panel->root, "Add clips, then save the recipe or render a new video.");
  Tag(panel->status, "video.status");
  UmiVideoSequenceGtkRefresh(panel);
  return panel->root;
}

#endif

#include "video_sequence_layout.inc"
#include "video_sequence_library.inc"
#include "video_sequence_editor.inc"
