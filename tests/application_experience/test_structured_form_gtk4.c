/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_gtk4.c
 * PURPOSE: Exercise native draft capture, atomic refusal and callback ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_fixture.h"
#include "umicom/ui/gtk4/structured_form.h"
static GtkWidget *find(GtkWidget *root, const char *name) {
  if (strcmp(gtk_widget_get_name(root), name) == 0)
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *result = find(child, name);
    if (result != NULL)
      return result;
  }
  return NULL;
}
typedef struct Observer {
  GtkWidget *editor, *entry;
  UmiStatus nested;
  int count, retire;
} Observer;
static void notified(GObject *object, GParamSpec *property, gpointer data) {
  (void)object;
  (void)property;
  Observer *observer = data;
  ++observer->count;
  if (observer->retire)
    UmiStructuredFormGtk4Retire(observer->editor);
  else {
    observer->nested = UmiStructuredFormGtk4Apply(observer->editor, 0);
    gtk_editable_set_text(GTK_EDITABLE(observer->entry), "8.0");
  }
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  if (!gtk_init_check()) {
    puts("SKIP: GTK4 display unavailable");
    return 77;
  }
  FormFixture *f = form_fixture_new();
  CHECK(f != NULL);
  const char *name = argv[1];
  if (strcmp(name, "utf8") == 0) {
    f->fields[0].label = "\xff";
    CHECK(UmiStructuredFormGtk4New(&f->schema, NULL) == NULL);
    free(f);
    return 0;
  }
  GtkWidget *editor = UmiStructuredFormGtk4New(&f->schema, NULL);
  CHECK(editor != NULL);
  g_object_ref_sink(editor);
  GtkWidget *mass = find(editor, "parcel.mass.value"),
            *mass_state = find(editor, "parcel.mass.state"),
            *reason = find(editor, "parcel.mass.reason");
  GtkWidget *time = find(editor, "parcel.observed.value"),
            *time_state = find(editor, "parcel.observed.state");
  GtkWidget *label = find(editor, "structured-form-feedback");
  CHECK(mass && mass_state && reason && time && time_state && label);
  CHECK(UmiStructuredFormGtk4CanClose(editor));
  gtk_drop_down_set_selected(GTK_DROP_DOWN(mass_state), (guint)UMI_FORM_KNOWN);
  gtk_editable_set_text(GTK_EDITABLE(mass), "3.5");
  CHECK(!UmiStructuredFormGtk4CanClose(editor));
  if (strcmp(name, "apply") == 0) {
    CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_OK);
    CHECK(UmiStructuredFormGtk4Snapshot(editor, &f->after) == UMI_STATUS_OK &&
          f->after.answers[0].value.decimal_value == 3.5 && f->after.revision == 2U);
    CHECK(!UmiStructuredFormGtk4CanClose(editor));
    CHECK(UmiStructuredFormGtk4MarkSaved(editor, 1U) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiStructuredFormGtk4MarkSaved(editor, 2U) == UMI_STATUS_OK &&
          UmiStructuredFormGtk4CanClose(editor));
  } else if (strcmp(name, "submit") == 0) {
    CHECK(UmiStructuredFormGtk4Apply(editor, 1) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiStructuredFormGtk4Snapshot(editor, &f->after) == UMI_STATUS_OK &&
          memcmp(&f->after, &f->before, sizeof(f->after)) == 0);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(time_state), (guint)UMI_FORM_KNOWN);
    gtk_editable_set_text(GTK_EDITABLE(time), "2024-02-29T12:00:00+01:00");
    CHECK(UmiStructuredFormGtk4Apply(editor, 1) == UMI_STATUS_OK);
  } else if (strcmp(name, "missing") == 0) {
    gtk_drop_down_set_selected(GTK_DROP_DOWN(mass_state), (guint)UMI_FORM_UNKNOWN);
    CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_INVALID_STATE);
    gtk_editable_set_text(GTK_EDITABLE(reason), "Not measured");
    CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_OK);
    CHECK(UmiStructuredFormGtk4Snapshot(editor, &f->after) == UMI_STATUS_OK &&
          f->after.answers[0].state == UMI_FORM_UNKNOWN &&
          f->after.answers[0].value.type == UMI_APPLICATION_EXPERIENCE_VALUE_NONE);
  } else if (strcmp(name, "invalid-second") == 0) {
    gtk_drop_down_set_selected(GTK_DROP_DOWN(time_state), (guint)UMI_FORM_KNOWN);
    gtk_editable_set_text(GTK_EDITABLE(time), "2024-02-29T12:00:00");
    CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiStructuredFormGtk4Snapshot(editor, &f->after) == UMI_STATUS_OK &&
          memcmp(&f->after, &f->before, sizeof(f->after)) == 0);
  } else if (strcmp(name, "numeric") == 0) {
    const char *bad[] = {"nan", "inf", "0x1p0", " 3", "3,5", "3kg", "1e999", "1e-999", "+", "3."};
    for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i) {
      gtk_editable_set_text(GTK_EDITABLE(mass), bad[i]);
      CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_PARSE_ERROR);
    }
    CHECK(UmiStructuredFormGtk4Snapshot(editor, &f->after) == UMI_STATUS_OK &&
          memcmp(&f->after, &f->before, sizeof(f->after)) == 0);
  } else if (strcmp(name, "later-edit") == 0 || strcmp(name, "notification-retire") == 0) {
    Observer observer = {editor, mass, UMI_STATUS_OK, 0, strcmp(name, "notification-retire") == 0};
    gulong signal = g_signal_connect(label, "notify::label", G_CALLBACK(notified), &observer);
    CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_OK && observer.count > 0);
    if (observer.retire)
      CHECK(UmiStructuredFormGtk4Snapshot(editor, &f->after) == UMI_STATUS_INVALID_STATE);
    else {
      CHECK(observer.nested == UMI_STATUS_BUSY && !UmiStructuredFormGtk4CanClose(editor));
      CHECK(UmiStructuredFormGtk4Snapshot(editor, &f->after) == UMI_STATUS_OK &&
            f->after.answers[0].value.decimal_value == 3.5);
    }
    g_signal_handler_disconnect(label, signal);
  } else if (strcmp(name, "retired") == 0) {
    UmiStructuredFormGtk4Retire(editor);
    CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiStructuredFormGtk4Snapshot(editor, &f->after) == UMI_STATUS_INVALID_STATE &&
          memcmp(&f->after, &f->before, sizeof(f->after)) == 0);
    CHECK(UmiStructuredFormGtk4MarkSaved(editor, 1U) == UMI_STATUS_INVALID_STATE);
  } else if (strcmp(name, "unroot") == 0) {
    GtkWidget *window = gtk_window_new();
    g_object_ref_sink(window);
    gtk_window_set_child(GTK_WINDOW(window), editor);
    gtk_window_set_child(GTK_WINDOW(window), NULL);
    CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_INVALID_STATE);
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
  } else
    CHECK(0);
  g_object_unref(editor);
  free(f);
  return 0;
}
