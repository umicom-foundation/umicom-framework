/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/video_sequence_layout.inc
 *
 * PURPOSE:
 *   Keep panel resizing, visibility and session layout recall independent of media edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static bool LayoutAvailable(UmiVideoSequencePanel *panel)
{
  return panel != NULL && !panel->closed && !panel->arranging &&
         panel->layoutPanes[0] != NULL && panel->layoutRegions[2] != NULL;
}

static void LayoutSetVisible(UmiVideoSequencePanel *panel, size_t index, bool visible)
{
  if (panel->closed || index >= 3U) return;
  gtk_widget_set_visible(panel->layoutRegions[index], visible);
  if (!panel->closed)
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->layoutChecks[index]), visible);
}

static void LayoutVisibility(GtkCheckButton *check, gpointer root)
{
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (!LayoutAvailable(panel)) return;
  size_t index = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(check), "media-region-index"));
  bool wasRefreshing = panel->refreshing;
  panel->arranging = true;
  panel->refreshing = true;
  LayoutSetVisible(panel, index, gtk_check_button_get_active(check));
  panel->refreshing = wasRefreshing;
  panel->arranging = false;
}

static void LayoutCommand(GtkButton *button, gpointer root)
{
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (!LayoutAvailable(panel)) return;
  const char *action = g_object_get_data(G_OBJECT(button), "video-action");
  if (action == NULL) return;
  /* Visibility and allocation notifications are presentation work. Reuse
   * the editor guard so nested callbacks cannot consume an undo entry. */
  bool wasRefreshing = panel->refreshing;
  panel->arranging = true;
  panel->refreshing = true;
  if (strcmp(action, "remember-layout") == 0) {
    for (size_t i = 0U; i < 3U; ++i) {
      panel->rememberedPositions[i] = gtk_paned_get_position(GTK_PANED(panel->layoutPanes[i]));
      panel->rememberedVisible[i] = gtk_widget_get_visible(panel->layoutRegions[i]);
    }
    panel->rememberedLayout = true;
    UmiVideoSequenceGtkMessage(panel, "Layout remembered for this window. The media recipe is unchanged.");
  } else if (strcmp(action, "recall-layout") == 0 && panel->rememberedLayout) {
    for (size_t i = 0U; i < 3U && !panel->closed; ++i) {
      LayoutSetVisible(panel, i, panel->rememberedVisible[i]);
      if (!panel->closed)
        gtk_paned_set_position(GTK_PANED(panel->layoutPanes[i]), panel->rememberedPositions[i]);
    }
  } else if (strcmp(action, "focus-preview") == 0) {
    for (size_t i = 0U; i < 3U && !panel->closed; ++i) LayoutSetVisible(panel, i, false);
  } else if (strcmp(action, "reset-layout") == 0) {
    const int positions[] = {250, 580, 440};
    for (size_t i = 0U; i < 3U && !panel->closed; ++i) {
      LayoutSetVisible(panel, i, true);
      if (!panel->closed) gtk_paned_set_position(GTK_PANED(panel->layoutPanes[i]), positions[i]);
    }
  } else if (strcmp(action, "recall-layout") == 0) {
    UmiVideoSequenceGtkMessage(panel, "Choose Remember layout first. This window has no remembered layout yet.");
  }
  panel->refreshing = wasRefreshing;
  panel->arranging = false;
}

/* Navigation can reveal an inspector hidden by a focus layout. Changing
 * visibility does not recreate its entries or discard unsubmitted edits. */
static void LayoutReveal(UmiVideoSequencePanel *panel, size_t region)
{
  if (!LayoutAvailable(panel)) return;
  bool wasRefreshing = panel->refreshing;
  panel->arranging = true;
  panel->refreshing = true;
  LayoutSetVisible(panel, region, true);
  panel->refreshing = wasRefreshing;
  panel->arranging = false;
}

static void LayoutHandle(GtkWidget *pane, const char *label)
{
  gtk_paned_set_wide_handle(GTK_PANED(pane), TRUE);
  gtk_widget_set_focusable(pane, TRUE);
  gtk_accessible_update_property(GTK_ACCESSIBLE(pane),
      GTK_ACCESSIBLE_PROPERTY_LABEL, label, -1);
}

static void LayoutMenu(UmiVideoSequencePanel *panel, GtkWidget *toolbar)
{
  GtkWidget *menu = gtk_menu_button_new();
  Tag(menu, "video.layout");
  gtk_menu_button_set_label(GTK_MENU_BUTTON(menu), "Layout");
  GtkWidget *popover = gtk_popover_new();
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_popover_set_child(GTK_POPOVER(popover), box);
  gtk_menu_button_set_popover(GTK_MENU_BUTTON(menu), popover);
  gtk_box_append(GTK_BOX(toolbar), menu);
  Label(box, "Drag the dividers to resize panels.");
  const char *labels[] = {"Show media library", "Show inspector", "Show timeline"};
  const char *ids[] = {"video.layout.library", "video.layout.inspector", "video.layout.timeline"};
  for (guint i = 0U; i < 3U; ++i) {
    GtkWidget *check = gtk_check_button_new_with_label(labels[i]);
    panel->layoutChecks[i] = check;
    Tag(check, ids[i]);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(check), TRUE);
    g_object_set_data(G_OBJECT(check), "media-region-index", GUINT_TO_POINTER(i));
    g_signal_connect_object(check, "toggled", G_CALLBACK(LayoutVisibility), panel->root, 0);
    gtk_box_append(GTK_BOX(box), check);
  }
  Button(panel, box, "Focus preview", "video.layout.focus", "focus-preview", G_CALLBACK(LayoutCommand));
  Button(panel, box, "Remember layout (this window)", "video.layout.remember", "remember-layout", G_CALLBACK(LayoutCommand));
  Button(panel, box, "Restore remembered layout", "video.layout.recall", "recall-layout", G_CALLBACK(LayoutCommand));
  Button(panel, box, "Reset panel layout", "video.layout.reset", "reset-layout", G_CALLBACK(LayoutCommand));
}
