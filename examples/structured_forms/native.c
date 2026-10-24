/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/structured_forms/native.c
 * PURPOSE: Host a shared native form without duplicating its validation rules.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/structured_form.h"
static const UmiApplicationExperienceField fields[] = {
    {.field_id = "parcel.mass",
     .label = "Parcel mass",
     .help_text = "Enter a number in kilograms. This is fictional example data.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_DECIMAL,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL,
     .required = 1,
     .enforce_number_range = 1,
     .minimum_number = 0.0,
     .maximum_number = 100.0},
    {.field_id = "parcel.observed",
     .label = "Observation timestamp",
     .help_text = "For example 2024-02-29T12:00:00+01:00.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
     .required = 1}};
static const UmiStructuredFormField rules[] = {
    {"parcel.mass", "mass.kg", UMI_FORM_TEXT_PLAIN, 1, 0},
    {"parcel.observed", "", UMI_FORM_TEXT_OFFSET_TIMESTAMP, 0, 0}};
static const UmiApplicationExperienceFormSchema base = {sizeof(base),
                                                        UMI_APPLICATION_EXPERIENCE_API_VERSION,
                                                        "example.parcel",
                                                        "Fictional parcel review",
                                                        "Local example with no persistence",
                                                        "example.save",
                                                        fields,
                                                        2U};
static const UmiStructuredFormSchema schema = {&base, 1U, rules, 2U};
/* The example host owns the close decision. Retain the earlier bare host for
 * review; the replacement blocks window-close while answers remain unsaved and
 * offers a separate explicit discard button. */
#if 0
static void activate(GtkApplication *app, gpointer data) {
  (void)data;
  GtkWidget *editor = UmiStructuredFormGtk4New(&schema, NULL);
  if (editor == NULL)
    return;
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Structured form example — unsaved demonstration");
  gtk_window_set_default_size(GTK_WINDOW(window), 640, 640);
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), editor);
  gtk_window_set_child(GTK_WINDOW(window), scroll);
  gtk_window_present(GTK_WINDOW(window));
}
#endif
static gboolean close_requested(GtkWindow *window, gpointer data) {
  (void)window;
  return !UmiStructuredFormGtk4CanClose(GTK_WIDGET(data));
}
static void discard_clicked(GtkButton *button, gpointer data) {
  (void)button;
  gtk_window_destroy(GTK_WINDOW(data));
}
static void activate(GtkApplication *app, gpointer data) {
  (void)data;
  GtkWidget *editor = UmiStructuredFormGtk4New(&schema, NULL);
  if (editor == NULL)
    return;
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Structured form example — unsaved demonstration");
  gtk_window_set_default_size(GTK_WINDOW(window), 640, 640);
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_widget_set_vexpand(scroll, TRUE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), editor);
  gtk_box_append(GTK_BOX(box), scroll);
  GtkWidget *notice = gtk_label_new("This example does not save. Window close is blocked after "
                                    "editing; use Discard and close to leave.");
  gtk_label_set_wrap(GTK_LABEL(notice), TRUE);
  gtk_box_append(GTK_BOX(box), notice);
  GtkWidget *discard = gtk_button_new_with_label("Discard and close");
  gtk_box_append(GTK_BOX(box), discard);
  gtk_window_set_child(GTK_WINDOW(window), box);
  g_signal_connect_object(window, "close-request", G_CALLBACK(close_requested), editor, 0);
  g_signal_connect_object(discard, "clicked", G_CALLBACK(discard_clicked), window, 0);
  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
  GtkApplication *app =
      gtk_application_new("org.umicom.example.structuredform", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
