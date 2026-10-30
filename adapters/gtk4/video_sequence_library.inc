/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/video_sequence_library.inc
 *
 * PURPOSE:
 *   Keep searchable media cards and duration-scaled timeline clips bound to the sequence model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static gboolean LibraryMatches(const UmiVideoSequenceClip *clip, const char *query)
{
  char *id = g_utf8_casefold(clip->id, -1);
  char *path = g_utf8_casefold(clip->path, -1);
  gboolean matches = query[0] == '\0' || strstr(id, query) != NULL || strstr(path, query) != NULL;
  g_free(id); g_free(path);
  return matches;
}

static void RefreshTimeline(UmiVideoSequencePanel *panel)
{
  if (panel->closed || panel->durationLabel == NULL || panel->browsing) return;
  panel->browsing = true;
  bool wasRefreshing = panel->refreshing;
  panel->refreshing = true; /* Presentation notifications must not dispatch edits. */
  UmiVideoSequence value = *UmiVideoSequenceEditorRead(panel->editor);
  guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector));
  char *query = g_utf8_casefold(gtk_editable_get_text(GTK_EDITABLE(panel->search)), -1);
  double zoom = gtk_range_get_value(GTK_RANGE(panel->zoom));
  uint32_t cursor = 0U;
  size_t matches = 0U;
  for (size_t i = 0U; i < UMI_VIDEO_SEQUENCE_MAX_CLIPS && !panel->closed; ++i) {
    gboolean exists = i < value.clip_count;
    GtkWidget *clipButton = panel->clipButtons[i];
    GtkWidget *card = panel->libraryButtons[i];
    gboolean shown = exists && LibraryMatches(&value.clips[i], query);
    gtk_widget_set_visible(clipButton, exists);
    if (panel->closed) break;
    gtk_widget_set_visible(gtk_widget_get_parent(card), shown);
    if (panel->closed) break;
    if (!exists) continue;
    matches += shown ? 1U : 0U;
    const UmiVideoSequenceClip *clip = &value.clips[i];
    uint32_t duration = clip->end_ms - clip->begin_ms;
    uint32_t end = cursor + duration;
    char *text = g_strdup_printf("%s\n%.2f - %.2f s", clip->id,
        (double)cursor / 1000.0, (double)end / 1000.0);
    gtk_button_set_label(GTK_BUTTON(clipButton), text);
    g_free(text);
    if (panel->closed) break;
    /* Small cuts retain a usable keyboard/click target. Exact start/end times
     * remain on each clip; zoom changes presentation, never source timing. */
    int width = (int)((double)duration * zoom / 1000.0);
    gtk_widget_set_size_request(clipButton, MAX(96, width), 64);
    if (panel->closed) break;
    char *basename = g_path_get_basename(clip->path);
    text = g_strdup_printf("%s\n%s\n%.2f seconds", clip->id, basename, (double)duration / 1000.0);
    gtk_label_set_text(GTK_LABEL(panel->libraryLabels[i]), text);
    g_free(text); g_free(basename);
    if (panel->closed) break;
    gtk_widget_set_tooltip_text(card, clip->path);
    if (panel->closed) break;
    gtk_widget_set_tooltip_text(clipButton, clip->path);
    if (panel->closed) break;
    if (i == selected) {
      gtk_widget_add_css_class(card, "suggested-action");
      gtk_widget_add_css_class(clipButton, "suggested-action");
    } else {
      gtk_widget_remove_css_class(card, "suggested-action");
      gtk_widget_remove_css_class(clipButton, "suggested-action");
    }
    cursor = end;
  }
  g_free(query);
  if (!panel->closed) {
    gtk_widget_set_visible(panel->emptyLibrary, matches == 0U);
    if (!panel->closed) gtk_label_set_text(GTK_LABEL(panel->emptyLibrary),
        value.clip_count == 0U ? "No media yet. Choose a video file and add its source range."
                              : "No clips match this search. Clear the search to see all media.");
  }
  if (!panel->closed) gtk_widget_set_visible(panel->emptyTimeline, value.clip_count == 0U);
  if (!panel->closed) {
    char *text = g_strdup_printf("%zu clips | %.2f seconds | %s", value.clip_count,
        (double)cursor / 1000.0, panel->dirty ? "Unsaved recipe" : "Recipe unchanged");
    gtk_label_set_text(GTK_LABEL(panel->durationLabel), text);
    g_free(text);
  }
  panel->refreshing = wasRefreshing;
  panel->browsing = false;
}

static void LibrarySearchChanged(GtkEditable *search, gpointer root)
{
  (void)search;
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (!panel->closed && !panel->refreshing) RefreshTimeline(panel);
}
static void TimelineZoomChanged(GtkRange *range, gpointer root)
{
  (void)range;
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (!panel->closed && !panel->refreshing) RefreshTimeline(panel);
}
static void WorkspaceNavigate(GtkButton *button, gpointer root)
{
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (panel->closed || panel->busy) return;
  const char *action = g_object_get_data(G_OBJECT(button), "video-action");
  if (action == NULL) return;
  LayoutReveal(panel, strcmp(action, "media") == 0 || strcmp(action, "project") == 0 ? 0U : 1U);
  if (panel->closed) return;
  if (strcmp(action, "media") == 0)
    gtk_notebook_set_current_page(GTK_NOTEBOOK(panel->sourceBook), 0);
  else if (strcmp(action, "project") == 0)
    gtk_notebook_set_current_page(GTK_NOTEBOOK(panel->sourceBook), 1);
  else if (strcmp(action, "export") == 0)
    gtk_notebook_set_current_page(GTK_NOTEBOOK(panel->inspectorBook), 1);
  else if (strcmp(action, "inspector") == 0)
    gtk_notebook_set_current_page(GTK_NOTEBOOK(panel->inspectorBook), 0);
}

static GtkWidget *MediaLibrary(UmiVideoSequencePanel *panel)
{
  GtkWidget *library = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  Tag(library, "video.library");
  panel->search = gtk_search_entry_new();
  Tag(panel->search, "video.library.search");
  gtk_accessible_update_property(GTK_ACCESSIBLE(panel->search),
      GTK_ACCESSIBLE_PROPERTY_LABEL, "Search sequence clips by name or file", -1);
  gtk_box_append(GTK_BOX(library), panel->search);
  g_signal_connect_object(panel->search, "changed", G_CALLBACK(LibrarySearchChanged), panel->root, 0);
  GtkWidget *cards = gtk_flow_box_new();
  Tag(cards, "video.library.cards");
  gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(cards), GTK_SELECTION_NONE);
  gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(cards), 1);
  gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(cards), 2);
  gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(cards), 6);
  gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(cards), 6);
  GtkWidget *scroll = WorkspaceScroll(cards);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 100);
  gtk_box_append(GTK_BOX(library), scroll);
  for (guint i = 0U; i < UMI_VIDEO_SEQUENCE_MAX_CLIPS; ++i) {
    GtkWidget *button = gtk_button_new();
    panel->libraryButtons[i] = button;
    char id[48]; (void)g_snprintf(id, sizeof id, "video.library.clip.%u", i);
    Tag(button, id);
    gtk_widget_add_css_class(button, "media-library-card");
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *icon = gtk_image_new_from_icon_name("video-x-generic-symbolic");
    gtk_image_set_pixel_size(GTK_IMAGE(icon), 28);
    gtk_box_append(GTK_BOX(body), icon);
    GtkWidget *label = gtk_label_new("");
    panel->libraryLabels[i] = label;
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_MIDDLE);
    gtk_label_set_max_width_chars(GTK_LABEL(label), 18);
    gtk_box_append(GTK_BOX(body), label);
    gtk_button_set_child(GTK_BUTTON(button), body);
    g_object_set_data(G_OBJECT(button), "video-clip-index", GUINT_TO_POINTER(i));
    g_signal_connect_object(button, "clicked", G_CALLBACK(TimelineSelect), panel->root, 0);
    gtk_flow_box_append(GTK_FLOW_BOX(cards), button);
  }
  panel->emptyLibrary = Label(library, "No media yet.");
  Tag(panel->emptyLibrary, "video.library.empty");
  gtk_widget_add_css_class(panel->emptyLibrary, "media-muted");
  return library;
}
