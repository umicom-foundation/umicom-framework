/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Inspection must show the captured value, units and missing reasons without
 * editable controls, markup interpretation or dependence on later input. */
#include "structured_form_fixture.h"
#undef CHECK
#include "umicom/ui/gtk4/structured_form_inspect.h"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static GtkWidget *find(GtkWidget *widget, const char *name) {
  if (strcmp(gtk_widget_get_name(widget), name) == 0)
    return widget;
  for (GtkWidget *child = gtk_widget_get_first_child(widget); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *match = find(child, name);
    if (match)
      return match;
  }
  return NULL;
}
static bool read_only(GtkWidget *widget) {
  if (GTK_IS_EDITABLE(widget) || GTK_IS_BUTTON(widget) || GTK_IS_DROP_DOWN(widget) ||
      GTK_IS_TEXT_VIEW(widget))
    return false;
  for (GtkWidget *child = gtk_widget_get_first_child(widget); child;
       child = gtk_widget_get_next_sibling(child))
    if (!read_only(child))
      return false;
  return true;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *cases[] = {"known",  "missing", "unanswered", "owned",      "readonly",
                         "secret", "invalid", "utf8",       "label-utf8", "markup"};
  bool selected = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      selected = true;
  if (!selected)
    return 2;
  if (!gtk_init_check()) {
    fprintf(stderr, "GTK display unavailable; inspection coverage skipped.\n");
    return 77;
  }
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new();
  GtkWidget *root = NULL;
  CHECK(f != NULL);
  UmiFormAnswer answer = form_mass(2.5);
  if (strcmp(name, "missing") == 0)
    answer = form_missing(UMI_FORM_UNKNOWN, "Awaiting measurement");
  if (strcmp(name, "unanswered") != 0)
    OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &answer));
  if (strcmp(name, "secret") == 0) {
    f->fields[2].kind = UMI_APPLICATION_EXPERIENCE_FIELD_SECRET;
    CHECK(UmiStructuredFormInspectGtk4New(&f->schema, &f->before, &root) ==
              UMI_STATUS_PERMISSION_DENIED &&
          root == NULL);
    goto cleanup;
  }
  if (strcmp(name, "invalid") == 0) {
    f->before.schema_version++;
    CHECK(UmiStructuredFormInspectGtk4New(&f->schema, &f->before, &root) != UMI_STATUS_OK &&
          root == NULL);
    goto cleanup;
  }
  if (strcmp(name, "label-utf8") == 0) {
    f->fields[2].label = "\xff";
    CHECK(UmiStructuredFormInspectGtk4New(&f->schema, &f->before, &root) ==
              UMI_STATUS_PARSE_ERROR &&
          root == NULL);
    goto cleanup;
  }
  if (strcmp(name, "utf8") == 0 || strcmp(name, "markup") == 0) {
    answer = form_text(strcmp(name, "utf8") == 0 ? "\xff" : "<b>Keep literal</b>");
    OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.note", &answer));
  }
  if (strcmp(name, "utf8") == 0) {
    CHECK(UmiStructuredFormInspectGtk4New(&f->schema, &f->before, &root) ==
              UMI_STATUS_PARSE_ERROR &&
          root == NULL);
    goto cleanup;
  }
  OK(UmiStructuredFormInspectGtk4New(&f->schema, &f->before, &root));
  g_object_ref_sink(root);
  CHECK(read_only(root));
  GtkWidget *value = find(root, "form-inspect.parcel.mass.value");
  CHECK(value != NULL && GTK_IS_LABEL(value));
  if (strcmp(name, "missing") == 0)
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(value)), "Awaiting measurement") != NULL);
  else if (strcmp(name, "unanswered") == 0)
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(value)), "Unanswered") != NULL);
  else
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(value)), "2.5 mass.kg") != NULL);
  if (strcmp(name, "owned") == 0) {
    answer = form_mass(9);
    OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &answer));
    free(f);
    f = NULL;
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(value)), "2.5 mass.kg") != NULL);
  }
  if (strcmp(name, "markup") == 0) {
    GtkWidget *note = find(root, "form-inspect.parcel.note.value");
    CHECK(note != NULL);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(note)), "<b>Keep literal</b>") != NULL &&
          !gtk_label_get_use_markup(GTK_LABEL(note)));
  }
cleanup:
  if (root != NULL)
    g_object_unref(root);
  free(f);
  return result;
}
