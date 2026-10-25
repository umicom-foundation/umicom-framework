/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_filter_gtk4.c
 *
 * PURPOSE:
 *   Native controls must preserve invalid text and distinguish every accepted edit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native controls must preserve invalid text and distinguish every accepted edit. */
#include "structured_form_fixture.h"
#undef CHECK
#include "umicom/ui/gtk4/structured_form_filter.h"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static GtkWidget *named(GtkWidget *root, const char *name) {
  if (!root)
    return NULL;
  if (strcmp(gtk_widget_get_name(root), name) == 0)
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *found = named(child, name);
    if (found)
      return found;
  }
  return NULL;
}
static GtkWidget *first_row(GtkWidget *widget) {
  return gtk_widget_get_first_child(named(widget, "form-filter-rows"));
}
static void drain(void) {
  unsigned steps = 0;
  while (g_main_context_pending(NULL) && steps++ < 100)
    g_main_context_iteration(NULL, FALSE);
}
typedef struct Observation {
  const char *mode;
  unsigned calls;
  bool edited, failed;
  uint64_t first, latest;
  UmiStatus reentry;
} Observation;
static void observed(GtkWidget *widget, guint64 revision, gpointer data) {
  Observation *o = data;
  ++o->calls;
  o->latest = revision;
  if (!o->first)
    o->first = revision;
  UmiStructuredFormFilter *value = g_new0(UmiStructuredFormFilter, 1);
  uint64_t actual = 0;
  if (UmiStructuredFormFilterGtk4Read(widget, value, &actual) != UMI_STATUS_OK ||
      actual != revision)
    o->failed = true;
  g_free(value);
  if (strcmp(o->mode, "reenter-observer") == 0)
    o->reentry = UmiStructuredFormFilterGtk4Add(widget, revision);
  if (strcmp(o->mode, "retire-observer") == 0)
    UmiStructuredFormFilterGtk4Retire(widget);
  if (strcmp(o->mode, "edit-observer") == 0 && !o->edited) {
    o->edited = true;
    gtk_editable_set_text(GTK_EDITABLE(named(first_row(widget), "form-filter-value")), "6");
    if (UmiStructuredFormFilterGtk4Revision(widget) <= revision)
      o->failed = true;
  }
}
int main(int argc, char **argv) {
  const char *cases[] = {"empty",
                         "add-button",
                         "remove-button",
                         "typed",
                         "invalid-text",
                         "outside-threshold",
                         "state-retains-text",
                         "unit",
                         "set",
                         "set-invalid-preserved",
                         "stale-add",
                         "stale-remove",
                         "stale-set",
                         "capacity",
                         "remove-invalid",
                         "retained-row",
                         "retained-control",
                         "retired",
                         "disposed",
                         "null",
                         "secret",
                         "utf8-label",
                         "utf8-value",
                         "reenter-observer",
                         "edit-observer",
                         "read-observer",
                         "retire-observer",
                         "clear",
                         "multiterm",
                         "long-value",
                         "invalid-selection",
                         "retained-add"};
  if (argc != 2)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      known = true;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new();
  GtkWidget *widget = NULL, *retained = NULL;
  UmiStructuredFormFilter *filter = g_new0(UmiStructuredFormFilter, 1),
                          *read = g_new0(UmiStructuredFormFilter, 1),
                          *sentinel = g_new0(UmiStructuredFormFilter, 1);
  CHECK(f);
  if (strcmp(name, "secret") == 0)
    f->fields[2].kind = UMI_APPLICATION_EXPERIENCE_FIELD_SECRET;
  if (strcmp(name, "utf8-label") == 0)
    f->fields[2].label = "\xff";
  UmiStatus status = UmiStructuredFormFilterGtk4New(&f->schema, &widget);
  if (strcmp(name, "secret") == 0 || strcmp(name, "utf8-label") == 0) {
    CHECK(status != UMI_STATUS_OK && !widget);
    goto cleanup;
  }
  OK(status);
  g_object_ref_sink(widget);
  uint64_t revision = UmiStructuredFormFilterGtk4Revision(widget), actual = 0;
  CHECK(revision != 0);
  OK(UmiStructuredFormFilterGtk4Read(widget, read, &actual));
  CHECK(read->count == 0 && actual == revision);
  if (strcmp(name, "empty") == 0)
    goto cleanup;
  if (strcmp(name, "null") == 0) {
    CHECK(UmiStructuredFormFilterGtk4Add(NULL, 0) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiStructuredFormFilterGtk4Read(widget, NULL, &actual) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiStructuredFormFilterGtk4Read(widget, read, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiStructuredFormFilterGtk4Set(widget, NULL, revision) != UMI_STATUS_OK);
    goto cleanup;
  }
  filter->count = 1;
  strcpy(filter->terms[0].field_id, "parcel.mass");
  filter->terms[0].operation = UMI_FORM_FILTER_EQUAL;
  filter->terms[0].operand = form_mass(5);
  if (strstr(name, "observer")) {
    Observation observation = {.mode = name};
    gulong handler = g_signal_connect(widget, "filter-changed", G_CALLBACK(observed), &observation);
    OK(UmiStructuredFormFilterGtk4Set(widget, filter, revision));
    drain();
    CHECK(!observation.failed && observation.calls > 0);
    if (strcmp(name, "reenter-observer") == 0)
      CHECK(observation.reentry == UMI_STATUS_BUSY);
    if (strcmp(name, "retire-observer") == 0)
      CHECK(UmiStructuredFormFilterGtk4Revision(widget) == 0);
    if (strcmp(name, "edit-observer") == 0) {
      CHECK(observation.edited && observation.latest > observation.first);
      OK(UmiStructuredFormFilterGtk4Read(widget, read, &actual));
      CHECK(read->terms[0].operand.value.decimal_value == 6 && actual == observation.latest);
    }
    g_signal_handler_disconnect(widget, handler);
    goto cleanup;
  }
  if (strcmp(name, "add-button") == 0 || strcmp(name, "remove-button") == 0) {
    g_signal_emit_by_name(named(widget, "form-filter-add"), "clicked");
    OK(UmiStructuredFormFilterGtk4Read(widget, read, &actual));
    CHECK(read->count == 1 && actual > revision);
    CHECK(read->terms[0].operation == UMI_FORM_FILTER_STATE &&
          read->terms[0].operand.state == UMI_FORM_KNOWN);
    if (strcmp(name, "remove-button") == 0) {
      g_signal_emit_by_name(named(first_row(widget), "form-filter-remove"), "clicked");
      OK(UmiStructuredFormFilterGtk4Read(widget, read, &actual));
      CHECK(read->count == 0);
    }
    goto cleanup;
  }
  OK(UmiStructuredFormFilterGtk4Set(widget, filter, revision));
  uint64_t current = UmiStructuredFormFilterGtk4Revision(widget);
  CHECK(current > revision);
  if (strcmp(name, "stale-add") == 0 || strcmp(name, "stale-remove") == 0 ||
      strcmp(name, "stale-set") == 0) {
    status = strcmp(name, "stale-add") == 0 ? UmiStructuredFormFilterGtk4Add(widget, revision)
             : strcmp(name, "stale-remove") == 0
                 ? UmiStructuredFormFilterGtk4Remove(widget, 0, revision)
                 : UmiStructuredFormFilterGtk4Set(widget, filter, revision);
    CHECK(status == UMI_STATUS_INVALID_STATE &&
          UmiStructuredFormFilterGtk4Revision(widget) == current);
    goto cleanup;
  }
  if (strcmp(name, "set-invalid-preserved") == 0 || strcmp(name, "utf8-value") == 0) {
    if (strcmp(name, "utf8-value") == 0) {
      strcpy(filter->terms[0].field_id, "parcel.note");
      filter->terms[0].operand = form_text("\xff");
    } else
      strcpy(filter->terms[0].operand.unit_id, "mass.lb");
    CHECK(UmiStructuredFormFilterGtk4Set(widget, filter, current) != UMI_STATUS_OK);
    OK(UmiStructuredFormFilterGtk4Read(widget, read, &actual));
    CHECK(actual == current && read->terms[0].operand.value.decimal_value == 5);
    goto cleanup;
  }
  if (strcmp(name, "capacity") == 0) {
    for (size_t i = 1; i < UMI_FORM_FILTER_CAPACITY; ++i)
      OK(UmiStructuredFormFilterGtk4Add(widget, UmiStructuredFormFilterGtk4Revision(widget)));
    current = UmiStructuredFormFilterGtk4Revision(widget);
    CHECK(UmiStructuredFormFilterGtk4Add(widget, current) == UMI_STATUS_CAPACITY_EXCEEDED);
    OK(UmiStructuredFormFilterGtk4Read(widget, read, &actual));
    CHECK(actual == current && read->count == UMI_FORM_FILTER_CAPACITY);
    goto cleanup;
  }
  if (strcmp(name, "remove-invalid") == 0) {
    CHECK(UmiStructuredFormFilterGtk4Remove(widget, 1, current) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiStructuredFormFilterGtk4Revision(widget) == current);
    goto cleanup;
  }
  if (strcmp(name, "retained-row") == 0 || strcmp(name, "retained-control") == 0) {
    retained = g_object_ref(strcmp(name, "retained-row") == 0
                                ? first_row(widget)
                                : named(first_row(widget), "form-filter-remove"));
    OK(UmiStructuredFormFilterGtk4Remove(widget, 0, current));
    current = UmiStructuredFormFilterGtk4Revision(widget);
    if (strcmp(name, "retained-row") == 0)
      gtk_editable_set_text(GTK_EDITABLE(named(retained, "form-filter-value")), "99");
    else
      g_signal_emit_by_name(retained, "clicked");
    CHECK(UmiStructuredFormFilterGtk4Revision(widget) == current);
    OK(UmiStructuredFormFilterGtk4Read(widget, read, &actual));
    CHECK(read->count == 0);
    goto cleanup;
  }
  if (strcmp(name, "retained-add") == 0) {
    retained = g_object_ref(named(widget, "form-filter-add"));
    g_object_unref(widget);
    widget = NULL;
    g_signal_emit_by_name(retained, "clicked");
    goto cleanup;
  }
  if (strcmp(name, "retired") == 0 || strcmp(name, "disposed") == 0) {
    if (strcmp(name, "retired") == 0)
      UmiStructuredFormFilterGtk4Retire(widget);
    else
      g_object_run_dispose(G_OBJECT(widget));
    CHECK(UmiStructuredFormFilterGtk4Revision(widget) == 0);
    CHECK(UmiStructuredFormFilterGtk4Read(widget, read, &actual) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiStructuredFormFilterGtk4Add(widget, current) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "clear") == 0) {
    filter->count = 0;
    OK(UmiStructuredFormFilterGtk4Set(widget, filter, current));
    OK(UmiStructuredFormFilterGtk4Read(widget, read, &actual));
    CHECK(read->count == 0);
    goto cleanup;
  }
  if (strcmp(name, "multiterm") == 0) {
    filter->count = 2;
    strcpy(filter->terms[1].field_id, "parcel.note");
    filter->terms[1].operation = UMI_FORM_FILTER_CONTAINS;
    filter->terms[1].operand = form_text("crate");
    OK(UmiStructuredFormFilterGtk4Set(widget, filter, current));
    OK(UmiStructuredFormFilterGtk4Read(widget, read, &actual));
    CHECK(read->count == 2 && strcmp(read->terms[1].operand.value.text_value, "crate") == 0);
    goto cleanup;
  }
  GtkWidget *row = first_row(widget), *entry = named(row, "form-filter-value"),
            *operation = named(row, "form-filter-operation");
  if (strcmp(name, "unit") == 0) {
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(named(row, "form-filter-unit"))), "mass.kg") == 0);
    goto cleanup;
  }
  if (strcmp(name, "set") == 0) {
    OK(UmiStructuredFormFilterGtk4Read(widget, read, &actual));
    CHECK(read->terms[0].operand.value.decimal_value == 5);
    goto cleanup;
  }
  if (strcmp(name, "state-retains-text") == 0) {
    gtk_editable_set_text(GTK_EDITABLE(entry), "incomplete");
    gtk_drop_down_set_selected(GTK_DROP_DOWN(operation), 8);
    OK(UmiStructuredFormFilterGtk4Read(widget, read, &actual));
    CHECK(read->terms[0].operand.state == UMI_FORM_UNKNOWN && !gtk_widget_get_sensitive(entry));
    gtk_drop_down_set_selected(GTK_DROP_DOWN(operation), 0);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "incomplete") == 0 &&
          gtk_widget_get_sensitive(entry));
    CHECK(UmiStructuredFormFilterGtk4Read(widget, read, &actual) != UMI_STATUS_OK);
    goto cleanup;
  }
  if (strcmp(name, "outside-threshold") == 0)
    gtk_drop_down_set_selected(GTK_DROP_DOWN(operation), 2);
  if (strcmp(name, "invalid-selection") == 0)
    gtk_drop_down_set_selected(GTK_DROP_DOWN(named(row, "form-filter-field")),
                               GTK_INVALID_LIST_POSITION);
  else if (strcmp(name, "long-value") == 0) {
    char long_value[600];
    memset(long_value, '1', sizeof long_value - 1);
    long_value[sizeof long_value - 1] = '\0';
    gtk_editable_set_text(GTK_EDITABLE(entry), long_value);
  } else
    gtk_editable_set_text(GTK_EDITABLE(entry), strcmp(name, "invalid-text") == 0        ? "-"
                                               : strcmp(name, "outside-threshold") == 0 ? "1000"
                                                                                        : "12.25");
  memset(read, 0x5a, sizeof *read);
  *sentinel = *read;
  actual = UINT64_MAX;
  status = UmiStructuredFormFilterGtk4Read(widget, read, &actual);
  if (strcmp(name, "invalid-text") == 0 || strcmp(name, "long-value") == 0 ||
      strcmp(name, "invalid-selection") == 0) {
    CHECK(status != UMI_STATUS_OK && actual == UINT64_MAX &&
          memcmp(read, sentinel, sizeof *read) == 0);
  } else {
    OK(status);
    CHECK(read->terms[0].operand.value.decimal_value ==
          (strcmp(name, "outside-threshold") == 0 ? 1000 : 12.25));
    CHECK(actual > current);
  }
cleanup:
  if (widget) {
    UmiStructuredFormFilterGtk4Retire(widget);
    g_object_unref(widget);
  }
  if (retained)
    g_object_unref(retained);
  drain();
  g_free(filter);
  g_free(read);
  g_free(sentinel);
  free(f);
  return result;
}
