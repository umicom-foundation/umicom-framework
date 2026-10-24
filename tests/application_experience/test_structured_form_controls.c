/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_controls.c
 * PURPOSE: Cover native multiline, boolean and identifier-backed choice editing.
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
    GtkWidget *match = find(child, name);
    if (match != NULL)
      return match;
  }
  return NULL;
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  if (!gtk_init_check()) {
    fprintf(stderr, "SKIP: GTK display unavailable\n");
    return 77;
  }
  const char *name = argv[1];
  FormFixture *f = form_fixture_new();
  CHECK(f != NULL);
  UmiApplicationExperienceFieldOption options[] = {
      {"hidden", "Hidden", 1}, {"ready", "Ready", 0}, {"closed", "Closed", 0}};
  int boolean = strcmp(name, "boolean") == 0;
  int choice = strcmp(name, "choice") == 0 || strcmp(name, "choice-initial") == 0 ||
               strcmp(name, "choice-label") == 0 || strcmp(name, "choice-utf8") == 0;
  if (boolean) {
    f->fields[2].kind = UMI_APPLICATION_EXPERIENCE_FIELD_BOOLEAN;
    f->fields[2].value_type = UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN;
  } else if (choice) {
    f->fields[2].kind = UMI_APPLICATION_EXPERIENCE_FIELD_CHOICE;
    f->fields[2].options = options;
    f->fields[2].option_count = 3U;
    if (strcmp(name, "choice-initial") == 0)
      f->before.answers[2] = form_text("closed");
    if (strcmp(name, "choice-label") == 0)
      options[1].label = "Ready (translated label)";
    if (strcmp(name, "choice-utf8") == 0)
      options[1].label = "\xC0\xAF";
  } else
    f->fields[2].kind = UMI_APPLICATION_EXPERIENCE_FIELD_MULTILINE;
  GtkWidget *editor = UmiStructuredFormGtk4New(&f->schema, &f->before);
  if (strcmp(name, "choice-utf8") == 0) {
    CHECK(editor == NULL);
    free(f);
    return 0;
  }
  CHECK(editor != NULL);
  g_object_ref_sink(editor);
  GtkWidget *value = find(editor, "parcel.note.value"), *state = find(editor, "parcel.note.state");
  CHECK(value != NULL && state != NULL);
  CHECK(UmiStructuredFormGtk4CanClose(editor));
  gtk_drop_down_set_selected(GTK_DROP_DOWN(state), (guint)UMI_FORM_KNOWN);
  if (boolean || choice) {
    CHECK(GTK_IS_DROP_DOWN(value));
    CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(value))) == 2U);
    if (strcmp(name, "choice-initial") == 0)
      CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(value)) == 1U);
    else {
      CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(value)) == GTK_INVALID_LIST_POSITION);
      CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_INVALID_STATE);
      gtk_drop_down_set_selected(GTK_DROP_DOWN(value), 0U);
    }
    CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_OK);
    CHECK(UmiStructuredFormGtk4Snapshot(editor, &f->after) == UMI_STATUS_OK);
    if (boolean)
      CHECK(f->after.answers[2].value.type == UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN &&
            f->after.answers[2].value.boolean_value == 0);
    else
      CHECK(strcmp(f->after.answers[2].value.text_value,
                   strcmp(name, "choice-initial") == 0 ? "closed" : "ready") == 0);
  } else {
    CHECK(GTK_IS_TEXT_VIEW(value));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(value));
    if (strcmp(name, "multiline") == 0) {
      gtk_text_buffer_set_text(buffer, "\nFirst line\nSecond line", -1);
      CHECK(!UmiStructuredFormGtk4CanClose(editor));
      CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_OK);
      CHECK(UmiStructuredFormGtk4Snapshot(editor, &f->after) == UMI_STATUS_OK &&
            strcmp(f->after.answers[2].value.text_value, "\nFirst line\nSecond line") == 0);
    } else if (strcmp(name, "capacity") == 0) {
      char text[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY + 1U];
      memset(text, 'x', sizeof(text) - 1U);
      text[sizeof(text) - 1U] = '\0';
      gtk_text_buffer_set_text(buffer, text, -1);
      CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_CAPACITY_EXCEEDED);
      CHECK(UmiStructuredFormGtk4Snapshot(editor, &f->after) == UMI_STATUS_OK &&
            memcmp(&f->before, &f->after, sizeof(f->before)) == 0);
    } else if (strcmp(name, "retired-buffer") == 0) {
      UmiStructuredFormGtk4Retire(editor);
      gtk_text_buffer_set_text(buffer, "Later notification", -1);
      CHECK(UmiStructuredFormGtk4Apply(editor, 0) == UMI_STATUS_INVALID_STATE);
    } else
      CHECK(0);
  }
  g_object_unref(editor);
  free(f);
  return 0;
}
