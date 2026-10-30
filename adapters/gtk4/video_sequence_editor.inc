/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/video_sequence_editor.inc
 *
 * PURPOSE:
 *   Compose a media library, central player, inspector and timeline around the shared sequence editor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "media_workspace_style.h"

/* The editor keeps its existing command IDs and model ownership. The desktop
 * surface may add navigation around it without cloning or reloading a recipe. */
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
  UmiMediaWorkspaceStyle(panel->root);
  g_object_set_data_full(G_OBJECT(panel->root), "umicom-video-sequence", panel, Dispose);
  gtk_widget_set_hexpand(panel->root, TRUE); gtk_widget_set_vexpand(panel->root, TRUE);
  panel->form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_widget_set_vexpand(panel->form, TRUE);
  gtk_box_append(GTK_BOX(panel->root), panel->form);
  GtkWidget *toolbarRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(panel->form), toolbarRow);
  LayoutMenu(panel, toolbarRow);
  GtkWidget *toolbar = WorkspaceActions(toolbarRow);
  Button(panel, toolbar, "Undo", "video.undo", "undo", G_CALLBACK(Edit));
  Button(panel, toolbar, "Redo", "video.redo", "redo", G_CALLBACK(Edit));
  Button(panel, toolbar, "Save new recipe", "video.save", "save", G_CALLBACK(UmiVideoSequenceGtkJob));
  Button(panel, toolbar, "Project", "video.navigation.project", "project", G_CALLBACK(WorkspaceNavigate));
  Button(panel, toolbar, "Export", "video.navigation.export", "export", G_CALLBACK(WorkspaceNavigate));

  GtkWidget *vertical = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
  Tag(vertical, "video.workspace.vertical");
  panel->layoutPanes[2] = vertical;
  LayoutHandle(vertical, "Resize preview and timeline");
  gtk_widget_set_vexpand(vertical, TRUE);
  gtk_box_append(GTK_BOX(panel->form), vertical);
  gtk_paned_set_position(GTK_PANED(vertical), 440);
  GtkWidget *horizontal = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  Tag(horizontal, "video.workspace.sources");
  panel->layoutPanes[0] = horizontal;
  LayoutHandle(horizontal, "Resize media library and editor");
  gtk_paned_set_start_child(GTK_PANED(vertical), horizontal);
  GtkWidget *sourceBook = gtk_notebook_new();
  panel->sourceBook = sourceBook;
  panel->layoutRegions[0] = sourceBook;
  Tag(sourceBook, "video.source-pages");
  gtk_widget_add_css_class(sourceBook, "media-region");
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(sourceBook), TRUE);
  gtk_widget_set_size_request(sourceBook, 220, -1);
  gtk_paned_set_start_child(GTK_PANED(horizontal), sourceBook);
  gtk_paned_set_position(GTK_PANED(horizontal), 250);
  gtk_paned_set_resize_start_child(GTK_PANED(horizontal), FALSE);
  gtk_paned_set_shrink_start_child(GTK_PANED(horizontal), FALSE);
  gtk_paned_set_shrink_end_child(GTK_PANED(horizontal), FALSE);
  GtkWidget *sources = WorkspacePage(sourceBook, "Media");
  Tag(sources, "video.sources");
  gtk_box_append(GTK_BOX(sources), MediaLibrary(panel));
  panel->selector = gtk_drop_down_new(NULL, NULL);
  Tag(panel->selector, "video.clips");
  gtk_box_append(GTK_BOX(sources), panel->selector);
  g_signal_connect_object(panel->selector, "notify::selected", G_CALLBACK(Select), panel->root, 0);
  /* Explicit IDs and ranges remain visible; the library never guesses a file duration. */
  panel->id = gtk_entry_new();
  Tag(panel->id, "video.id");
  gtk_entry_set_placeholder_text(GTK_ENTRY(panel->id), "New clip name");
  gtk_accessible_update_property(GTK_ACCESSIBLE(panel->id), GTK_ACCESSIBLE_PROPERTY_LABEL, "Clip ID", -1);
  gtk_entry_set_max_length(GTK_ENTRY(panel->id), (int)sizeof(initial.clips[0].id) - 1);
  gtk_box_append(GTK_BOX(sources), panel->id);
  panel->path = gtk_entry_new();
  Tag(panel->path, "video.path");
  gtk_entry_set_placeholder_text(GTK_ENTRY(panel->path), "Local video file");
  gtk_accessible_update_property(GTK_ACCESSIBLE(panel->path), GTK_ACCESSIBLE_PROPERTY_LABEL, "Local video file", -1);
  gtk_entry_set_max_length(GTK_ENTRY(panel->path), (int)UMI_PATH_CAPACITY - 1);
  gtk_box_append(GTK_BOX(sources), panel->path);
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
  panel->layoutPanes[1] = right;
  LayoutHandle(right, "Resize preview and inspector");
  gtk_paned_set_end_child(GTK_PANED(horizontal), right);
  gtk_paned_set_position(GTK_PANED(right), 580);
  gtk_paned_set_resize_end_child(GTK_PANED(right), FALSE);
  gtk_paned_set_shrink_start_child(GTK_PANED(right), FALSE);
  gtk_paned_set_shrink_end_child(GTK_PANED(right), FALSE);
  GtkWidget *preview = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  Tag(preview, "video.preview-region");
  gtk_widget_add_css_class(preview, "media-region");
  gtk_widget_add_css_class(preview, "media-preview");
  gtk_widget_set_hexpand(preview, TRUE); gtk_widget_set_vexpand(preview, TRUE);
  gtk_widget_set_size_request(preview, 240, 180);
  gtk_paned_set_start_child(GTK_PANED(right), preview);
  GtkWidget *heading = Label(preview, "Preview");
  gtk_widget_add_css_class(heading, "heading");
  panel->video = gtk_video_new();
  Tag(panel->video, "video.player");
  gtk_widget_set_hexpand(panel->video, TRUE); gtk_widget_set_vexpand(panel->video, TRUE);
  gtk_box_append(GTK_BOX(preview), panel->video);
  GtkWidget *previewHint = Label(preview, "Select a clip, then preview its applied range. Preview is silent.");
  gtk_widget_add_css_class(previewHint, "media-muted");
  GtkWidget *transport = WorkspaceActions(preview);
  Button(panel, transport, "Preview selected clip", "video.preview", "preview", G_CALLBACK(UmiVideoSequenceGtkPreview));
  Button(panel, transport, "Stop preview", "video.stop", "stop", G_CALLBACK(UmiVideoSequenceGtkPreview));
  GtkWidget *inspectorBook = gtk_notebook_new();
  panel->inspectorBook = inspectorBook;
  panel->layoutRegions[1] = inspectorBook;
  Tag(inspectorBook, "video.inspector-pages");
  gtk_widget_add_css_class(inspectorBook, "media-region");
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(inspectorBook), TRUE);
  gtk_widget_set_size_request(inspectorBook, 260, -1);
  gtk_paned_set_end_child(GTK_PANED(right), inspectorBook);
  GtkWidget *inspector = WorkspacePage(inspectorBook, "Inspector");
  Tag(inspector, "video.inspector");
  Label(inspector, "Trim the selected source in milliseconds. Apply trim to change the sequence.");
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
  panel->layoutRegions[2] = timeline;
  gtk_widget_add_css_class(timeline, "media-timeline");
  gtk_widget_add_css_class(timeline, "media-region");
  gtk_paned_set_resize_end_child(GTK_PANED(vertical), FALSE);
  gtk_paned_set_shrink_end_child(GTK_PANED(vertical), FALSE);
  gtk_paned_set_end_child(GTK_PANED(vertical), timeline);
  GtkWidget *timelineBar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_box_append(GTK_BOX(timeline), timelineBar);
  panel->durationLabel = Label(timelineBar, "Timeline");
  gtk_widget_set_hexpand(panel->durationLabel, TRUE);
  Label(timelineBar, "Zoom");
  panel->zoom = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 4.0, 40.0, 1.0);
  Tag(panel->zoom, "video.timeline.zoom");
  gtk_widget_set_size_request(panel->zoom, 130, -1);
  gtk_scale_set_draw_value(GTK_SCALE(panel->zoom), FALSE);
  gtk_range_set_value(GTK_RANGE(panel->zoom), 12.0);
  gtk_accessible_update_property(GTK_ACCESSIBLE(panel->zoom),
      GTK_ACCESSIBLE_PROPERTY_LABEL, "Timeline zoom", -1);
  gtk_box_append(GTK_BOX(timelineBar), panel->zoom);
  g_signal_connect_object(panel->zoom, "value-changed", G_CALLBACK(TimelineZoomChanged), panel->root, 0);
  Label(timeline, "Video 1  |  Sequential cuts");
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
    gtk_widget_add_css_class(button, "media-clip");
    gtk_widget_set_size_request(button, 140, 64);
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
  GtkWidget *statusBar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_widget_add_css_class(statusBar, "media-status");
  gtk_box_append(GTK_BOX(panel->root), statusBar);
  panel->cancelButton = Button(panel, statusBar, "Cancel operation", "video.cancel", "cancel", G_CALLBACK(Cancel));
  gtk_widget_set_sensitive(panel->cancelButton, FALSE);
  panel->status = Label(statusBar, "Add clips, then save the recipe or render a new video.");
  Tag(panel->status, "video.status");
  UmiVideoSequenceGtkRefresh(panel);
  return panel->root;
}
