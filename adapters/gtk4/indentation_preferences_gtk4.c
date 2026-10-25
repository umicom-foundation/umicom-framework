/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/indentation_preferences_gtk4.c
 *
 * PURPOSE:
 *   Present a validated indentation draft independently of document edits and settings publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/indentation_preferences.h"
typedef struct IndentationControls {
  GtkWidget *width, *spaces;
} IndentationControls;
static IndentationControls *IndentationControlsFor(GtkWidget *widget) {
  return GTK_IS_BOX(widget) ? g_object_get_data(G_OBJECT(widget), "umicom-indentation-controls")
                            : NULL;
}
UmiStatus UmiGtk4IndentationPreferencesCreate(const UmiEditorIndentation *initial,
                                              GtkWidget **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (UmiEditorIndentationValidate(initial) != UMI_STATUS_OK)
    return UMI_STATUS_INVALID_ARGUMENT;
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  IndentationControls *controls = g_new0(IndentationControls, 1);
  g_object_set_data_full(G_OBJECT(box), "umicom-indentation-controls", controls, g_free);
  GtkWidget *label = gtk_label_new_with_mnemonic("_Tab width (1–16 columns)");
  controls->width = gtk_spin_button_new_with_range(1.0, UMI_EDITOR_INDENTATION_MAXIMUM_WIDTH, 1.0);
  gtk_spin_button_set_digits(GTK_SPIN_BUTTON(controls->width), 0);
  gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(controls->width), GTK_UPDATE_IF_VALID);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(controls->width), initial->tab_width);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label), controls->width);
  controls->spaces = gtk_check_button_new_with_mnemonic("Insert _spaces instead of tabs");
  gtk_check_button_set_active(GTK_CHECK_BUTTON(controls->spaces), initial->insert_spaces != 0);
  gtk_widget_set_tooltip_text(controls->width, "The number of logical columns between tab stops.");
  gtk_box_append(GTK_BOX(box), label);
  gtk_box_append(GTK_BOX(box), controls->width);
  gtk_box_append(GTK_BOX(box), controls->spaces);
  GtkWidget *help = gtk_label_new("Tab indents selected lines. Shift+Tab removes one indentation "
                                  "level. Existing text changes only when you edit it.");
  gtk_label_set_wrap(GTK_LABEL(help), TRUE);
  gtk_box_append(GTK_BOX(box), help);
  *out = box;
  return UMI_STATUS_OK;
}
UmiStatus UmiGtk4IndentationPreferencesRead(GtkWidget *widget, UmiEditorIndentation *out) {
  IndentationControls *controls = IndentationControlsFor(widget);
  if (controls == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  const char *text = gtk_editable_get_text(GTK_EDITABLE(controls->width));
  uint32_t width = 0U;
  if (text == NULL || text[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0U; text[i] != '\0'; ++i) {
    if (i >= 2U || text[i] < '0' || text[i] > '9')
      return UMI_STATUS_INVALID_ARGUMENT;
    width = width * 10U + (uint32_t)(text[i] - '0');
  }
  UmiEditorIndentation candidate = {
      width, gtk_check_button_get_active(GTK_CHECK_BUTTON(controls->spaces)) ? 1 : 0};
  UmiStatus status = UmiEditorIndentationValidate(&candidate);
  if (status == UMI_STATUS_OK)
    *out = candidate;
  return status;
}
GtkWidget *UmiGtk4IndentationPreferencesWidth(GtkWidget *widget) {
  IndentationControls *controls = IndentationControlsFor(widget);
  return controls == NULL ? NULL : controls->width;
}
GtkWidget *UmiGtk4IndentationPreferencesSpaces(GtkWidget *widget) {
  IndentationControls *controls = IndentationControlsFor(widget);
  return controls == NULL ? NULL : controls->spaces;
}
