/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/video_sequence_presentation.inc
 *
 * PURPOSE:
 *   Present existing sequence commands in compact, accessible editing controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Presentation wraps the existing command factory. It does not acquire a
 * second editor, create a second history, or dispatch edits from layout events. */
static GtkWidget *PresentationButton(UmiVideoSequencePanel *panel, GtkWidget *box,
    const char *label, const char *icon, const char *id, const char *action,
    GCallback callback)
{
  GtkWidget *button = Button(panel, box, label, id, action, callback);
  gtk_button_set_child(GTK_BUTTON(button), gtk_image_new_from_icon_name(icon));
  gtk_widget_add_css_class(button, "media-icon-button");
  gtk_widget_set_tooltip_text(button, label);
  gtk_accessible_update_property(GTK_ACCESSIBLE(button),
      GTK_ACCESSIBLE_PROPERTY_LABEL, label, -1);
  return button;
}

/* Keep section children alive when collapsed so partially entered names and
 * timing values survive navigation between editing tasks. */
static GtkWidget *PresentationSection(GtkWidget *parent, const char *title,
                                      const char *id, bool expanded)
{
  GtkWidget *section = gtk_expander_new(title);
  GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  Tag(section, id);
  gtk_widget_add_css_class(section, "media-section");
  gtk_widget_add_css_class(body, "media-section-body");
  gtk_expander_set_child(GTK_EXPANDER(section), body);
  gtk_expander_set_expanded(GTK_EXPANDER(section), expanded);
  gtk_box_append(GTK_BOX(parent), section);
  return body;
}

/* A timeline shortcut reveals the real split form. It cannot silently split at
 * an invented playhead position: the user reviews the source time and new ID. */
static void PresentationSplit(GtkButton *button, gpointer root)
{
  (void)button;
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (!LayoutAvailable(panel) || panel->busy || panel->refreshing || panel->changing) return;
  GtkWidget *section = gtk_widget_get_ancestor(panel->split, GTK_TYPE_EXPANDER);
  if (section == NULL) return;
  LayoutReveal(panel, 1U);
  if (panel->closed) return;
  /* Showing a section can synchronously notify observers. Protect the model
   * throughout the reveal, and stop immediately if the owner retires it. */
  bool wasRefreshing = panel->refreshing;
  panel->refreshing = true;
  gtk_notebook_set_current_page(GTK_NOTEBOOK(panel->inspectorBook), 0);
  if (!panel->closed) gtk_expander_set_expanded(GTK_EXPANDER(section), TRUE);
  if (!panel->closed) (void)gtk_widget_grab_focus(panel->split);
  panel->refreshing = wasRefreshing;
}
