/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Exercise real native controls and SQLite history. Missing display coverage
 * is explicitly skipped; persistence failures never become successful skips. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_record.h"
#include "umicom/ui/gtk4/form_record_editor.h"
#include <glib/gstdio.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
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
static bool text(GtkWidget *root, const char *name, const char *value) {
  GtkWidget *control = find(root, name);
  if (control == NULL || !GTK_IS_EDITABLE(control))
    return false;
  gtk_editable_set_text(GTK_EDITABLE(control), value);
  return true;
}
static bool known(GtkWidget *root, const char *name) {
  GtkWidget *control = find(root, name);
  if (control == NULL || !GTK_IS_DROP_DOWN(control))
    return false;
  gtk_drop_down_set_selected(GTK_DROP_DOWN(control), UMI_FORM_KNOWN);
  return true;
}
static bool fill(GtkWidget *root, const char *mass) {
  return text(root, "parcel.mass.value", mass) && known(root, "parcel.mass.state") &&
         text(root, "parcel.observed.value", "2026-10-24T12:00:00+01:00") &&
         known(root, "parcel.observed.state");
}
static bool idle(GtkWidget *root) {
  gint64 end = g_get_monotonic_time() + 15 * G_USEC_PER_SEC;
  while (UmiFormRecordEditorGtk4Busy(root) && g_get_monotonic_time() < end) {
    (void)g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  return !UmiFormRecordEditorGtk4Busy(root);
}
typedef struct Observer {
  GtkWidget *root;
  const char *prefix;
  bool fired, close;
} Observer;
static void observe(GObject *object, GParamSpec *property, gpointer data) {
  (void)property;
  Observer *observer = data;
  if (observer->fired || !g_str_has_prefix(gtk_label_get_text(GTK_LABEL(object)), observer->prefix))
    return;
  observer->fired = true;
  if (observer->close)
    g_object_run_dispose(G_OBJECT(observer->root));
  else
    (void)text(observer->root, "parcel.mass.value", "9");
}
static void change_attribution(GObject *object, GParamSpec *property, gpointer data) {
  (void)object;
  (void)property;
  UmiRecordDraftAttribution *attribution = data;
  (void)g_strlcpy(attribution->actor, "Changed during notification", sizeof(attribution->actor));
}
static GtkWidget *discard_prompt(void) {
  GListModel *windows = gtk_window_get_toplevels();
  for (guint i = 0; i < g_list_model_get_n_items(windows); ++i) {
    GtkWindow *window = g_list_model_get_item(windows, i);
    const char *title = gtk_window_get_title(window);
    if (title != NULL && strcmp(title, "Discard unsaved answers?") == 0)
      return GTK_WIDGET(window);
    g_object_unref(window);
  }
  return NULL;
}
int main(int argc, char **argv) {
  if (argc != 3)
    return 2;
  const char *cases[] = {"options",
                         "relative-path",
                         "retired-actions",
                         "keys",
                         "missing",
                         "new-discard",
                         "required",
                         "draft",
                         "invalid-input",
                         "invalid-attribution",
                         "roundtrip",
                         "busy",
                         "later-edit",
                         "dirty-open",
                         "discard-saved",
                         "stale",
                         "read-later-edit",
                         "retire-connect",
                         "retire-read",
                         "retire-save",
                         "unroot-save",
                         "acceptance-close",
                         "acceptance-edit",
                         "completion-close",
                         "completion-edit",
                         "prompt-context",
                         "prompt-cancel",
                         "prompt-accept",
                         "input-attribution",
                         "recovery-new",
                         "recovery-saved",
                         "recovery-exclusive",
                         "recovery-dirty",
                         "recovery-later-edit",
                         "recovery-retire-write",
                         "recovery-retire-read",
                         "recovery-wrong-store",
                         "recovery-namespace"};
  bool selected = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      selected = true;
  if (!selected)
    return 2;
  if (!gtk_init_check()) {
    fprintf(stderr, "GTK display unavailable; native coverage skipped.\n");
    return 77;
  }
  int result = 0;
  const char *name = argv[1];
  FormFixture *fixture = form_fixture_new();
  GtkWidget *root = NULL, *window = NULL, *retained = NULL, *label = NULL;
  UmiDataServer *reader = NULL;
  UmiStructuredFormRecord *stored = calloc(1, sizeof(*stored));
  UmiStructuredFormSessionView *state = calloc(1, sizeof(*state));
  char *directory = NULL, *database = NULL, *recovery = NULL;
  Observer observer = {0};
  gulong observer_id = 0;
  CHECK(fixture != NULL && stored != NULL && state != NULL);
  UmiFormRecordEditorGtk4Options options = {
      .schema = &fixture->schema, .entity_prefix = "test.form.", .source = "Native form fixture"};
  if (strcmp(name, "options") == 0) {
    options.entity_prefix = "bad/prefix.";
    CHECK(UmiFormRecordEditorGtk4New(&options, &root) == UMI_STATUS_INVALID_ARGUMENT &&
          root == NULL);
    options.entity_prefix = "missing-dot";
    CHECK(UmiFormRecordEditorGtk4New(&options, &root) == UMI_STATUS_INVALID_ARGUMENT);
    options.entity_prefix = "test.form.";
    options.source = "";
    CHECK(UmiFormRecordEditorGtk4New(&options, &root) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  OK(UmiFormRecordEditorGtk4New(&options, &root));
  g_object_ref_sink(root);
  CHECK(UmiFormRecordEditorGtk4CanClose(root));
  if (strcmp(name, "relative-path") == 0) {
    CHECK(UmiFormRecordEditorGtk4Connect(root, "relative.sqlite3") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFormRecordEditorGtk4Begin(root, "one") == UMI_STATUS_INVALID_STATE);
    CHECK(!UmiFormRecordEditorGtk4Busy(root));
    goto cleanup;
  }
  if (strcmp(name, "retired-actions") == 0) {
    retained = g_object_ref(find(root, "form-record-new"));
    UmiFormRecordEditorGtk4Retire(root);
    g_signal_emit_by_name(retained, "clicked");
    CHECK(UmiFormRecordEditorGtk4Begin(root, "one") == UMI_STATUS_INVALID_STATE);
    OK(UmiFormRecordEditorGtk4State(root, state));
    CHECK(!state->record.ready);
    goto cleanup;
  }
  CHECK(g_mkdir_with_parents(argv[2], 0700) == 0);
  directory = g_build_filename(argv[2], "form-record-XXXXXX", NULL);
  CHECK(g_mkdtemp(directory) != NULL);
  database = g_build_filename(directory, "records.sqlite3", NULL);
  OK(UmiFormRecordEditorGtk4Connect(root, database));
  if (strcmp(name, "retire-connect") == 0) {
    UmiFormRecordEditorGtk4Retire(root);
    CHECK(idle(root));
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) == UMI_STATUS_CANCELLED);
    goto cleanup;
  }
  CHECK(idle(root));
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  CHECK(UmiFormRecordEditorGtk4Connect(root, database) == UMI_STATUS_ALREADY_EXISTS);
  OK(umi_data_server_create_sqlite(database, &reader));
  if (strcmp(name, "keys") == 0) {
    CHECK(UmiFormRecordEditorGtk4Begin(root, "../outside") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFormRecordEditorGtk4Open(root, "") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFormRecordEditorGtk4Begin(
              root,
              "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz") ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    OK(UmiFormRecordEditorGtk4State(root, state));
    CHECK(!state->record.ready);
    goto cleanup;
  }
  if (strcmp(name, "missing") == 0) {
    OK(UmiFormRecordEditorGtk4Open(root, "absent"));
    CHECK(idle(root));
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) == UMI_STATUS_NOT_FOUND);
    OK(UmiFormRecordEditorGtk4State(root, state));
    CHECK(!state->record.ready);
    CHECK(UmiFormRecordEditorGtk4CanClose(root));
    goto cleanup;
  }
  OK(UmiFormRecordEditorGtk4Begin(root, "one"));
  CHECK(!UmiFormRecordEditorGtk4CanClose(root));
  UmiRecordDraftAttribution attribution = {
      .source = "Native form fixture", .actor = "Fixture owner", .reason = "Synthetic regression"};
  if (strcmp(name, "new-discard") == 0) {
    CHECK(fill(root, "2"));
    OK(UmiFormRecordEditorGtk4Discard(root));
    OK(UmiFormRecordEditorGtk4State(root, state));
    CHECK(!state->record.ready);
    CHECK(UmiFormRecordEditorGtk4CanClose(root));
    goto cleanup;
  }
  if (strcmp(name, "required") == 0) {
    CHECK(UmiFormRecordEditorGtk4Save(root, &attribution, 1) == UMI_STATUS_INVALID_STATE);
    CHECK(!UmiFormRecordEditorGtk4Busy(root));
    CHECK(umi_structured_form_record_load(reader, "test.form.one", 0, &fixture->schema, stored) ==
          UMI_STATUS_NOT_FOUND);
    goto cleanup;
  }
  if (strcmp(name, "draft") != 0)
    CHECK(fill(root, "2"));
  if (strcmp(name, "input-attribution") == 0) {
    GtkWidget *feedback = g_object_ref(find(root, "structured-form-feedback"));
    gulong id =
        g_signal_connect(feedback, "notify::label", G_CALLBACK(change_attribution), &attribution);
    UmiStatus accepted = UmiFormRecordEditorGtk4Save(root, &attribution, 1);
    g_signal_handler_disconnect(feedback, id);
    g_object_unref(feedback);
    OK(accepted);
    CHECK(idle(root));
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    OK(umi_structured_form_record_load(reader, "test.form.one", 0, &fixture->schema, stored));
    CHECK(strcmp(stored->record.amendment.actor, "Fixture owner") == 0);
    CHECK(strcmp(attribution.actor, "Changed during notification") == 0);
    goto cleanup;
  }
  if (strcmp(name, "recovery-new") == 0) {
    recovery = g_build_filename(directory, "new-draft.umidraft", NULL);
    OK(UmiFormRecordEditorGtk4KeepRecovery(root, recovery));
    CHECK(idle(root));
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    CHECK(!UmiFormRecordEditorGtk4CanClose(root));
    OK(UmiFormRecordEditorGtk4Discard(root));
    OK(UmiFormRecordEditorGtk4Recover(root, recovery));
    CHECK(idle(root));
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    OK(UmiFormRecordEditorGtk4State(root, state));
    CHECK(state->record.is_new && state->record.dirty);
    CHECK(state->form.answers[0].value.decimal_value == 2);
    goto cleanup;
  }

  if (strcmp(name, "invalid-input") == 0) {
    CHECK(text(root, "parcel.mass.value", "NaN"));
    CHECK(UmiFormRecordEditorGtk4Save(root, &attribution, 0) != UMI_STATUS_OK);
    OK(UmiFormRecordEditorGtk4State(root, state));
    CHECK(state->form.answers[0].state == UMI_FORM_UNANSWERED);
    CHECK(!UmiFormRecordEditorGtk4CanClose(root));
    goto cleanup;
  }
  if (strcmp(name, "invalid-attribution") == 0) {
    attribution.actor[0] = '\0';
    CHECK(UmiFormRecordEditorGtk4Save(root, &attribution, 1) != UMI_STATUS_OK);
    CHECK(!UmiFormRecordEditorGtk4Busy(root));
    OK(UmiFormRecordEditorGtk4State(root, state));
    CHECK(state->record.dirty);
    goto cleanup;
  }
  if (g_str_has_prefix(name, "acceptance-") || g_str_has_prefix(name, "completion-")) {
    observer = (Observer){
        root, g_str_has_prefix(name, "acceptance-") ? "Saving the captured" : "Saved the captured",
        false, g_str_has_suffix(name, "close")};
    label = g_object_ref(find(root, "form-record-feedback"));
    observer_id = g_signal_connect(label, "notify::label", G_CALLBACK(observe), &observer);
  }
  OK(UmiFormRecordEditorGtk4Save(root, &attribution, strcmp(name, "draft") != 0));
  if (strcmp(name, "busy") == 0) {
    CHECK(UmiFormRecordEditorGtk4Discard(root) == UMI_STATUS_BUSY);
    CHECK(UmiFormRecordEditorGtk4Begin(root, "two") == UMI_STATUS_BUSY);
    CHECK(UmiFormRecordEditorGtk4Save(root, &attribution, 0) == UMI_STATUS_BUSY);
  }
  if (strcmp(name, "later-edit") == 0)
    CHECK(text(root, "parcel.mass.value", "3"));
  if (strcmp(name, "retire-save") == 0)
    UmiFormRecordEditorGtk4Retire(root);
  if (strcmp(name, "unroot-save") == 0) {
    window = gtk_window_new();
    gtk_window_set_child(GTK_WINDOW(window), root);
    gtk_window_set_child(GTK_WINDOW(window), NULL);
  }
  CHECK(idle(root));
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  OK(umi_structured_form_record_load(reader, "test.form.one", 0, &fixture->schema, stored));
  CHECK(stored->record.revision == 1);
  if (strcmp(name, "draft") == 0) {
    CHECK(stored->form.answers[0].state == UMI_FORM_UNANSWERED);
    goto cleanup;
  }
  CHECK(stored->form.answers[0].value.decimal_value == 2.0);
  if (observer_id != 0) {
    CHECK(observer.fired);
    if (!observer.close) {
      CHECK(!UmiFormRecordEditorGtk4CanClose(root));
      CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "parcel.mass.value"))), "9") == 0);
    }
    goto cleanup;
  }
  if (strcmp(name, "retire-save") == 0 || strcmp(name, "unroot-save") == 0) {
    CHECK(UmiFormRecordEditorGtk4Open(root, "one") == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "later-edit") == 0) {
    CHECK(!UmiFormRecordEditorGtk4CanClose(root));
    OK(UmiFormRecordEditorGtk4Save(root, &attribution, 1));
    CHECK(idle(root));
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    OK(umi_structured_form_record_load(reader, "test.form.one", 0, &fixture->schema, stored));
    CHECK(stored->record.revision == 2 && stored->form.answers[0].value.decimal_value == 3.0);
    goto cleanup;
  }
  if (g_str_has_prefix(name, "prompt-")) {
    window = gtk_window_new();
    gtk_window_set_child(GTK_WINDOW(window), root);
    CHECK(text(root, "parcel.mass.value", "7"));
    GtkWidget *trigger = find(root, "form-record-discard");
    CHECK(trigger != NULL);
    g_signal_emit_by_name(trigger, "clicked");
    GtkWidget *prompt = discard_prompt();
    CHECK(prompt != NULL);
    if (strcmp(name, "prompt-context") == 0) {
      OK(UmiFormRecordEditorGtk4Discard(root));
      OK(UmiFormRecordEditorGtk4Begin(root, "two"));
    }
    GtkWidget *decision =
        find(prompt, strcmp(name, "prompt-cancel") == 0 ? "form-record-discard-cancel"
                                                        : "form-record-discard-confirm");
    if (decision == NULL) {
      gtk_window_destroy(GTK_WINDOW(prompt));
      g_object_unref(prompt);
      CHECK(false);
    }
    g_signal_emit_by_name(decision, "clicked");
    g_object_unref(prompt);
    OK(UmiFormRecordEditorGtk4State(root, state));
    if (strcmp(name, "prompt-context") == 0)
      CHECK(state->record.is_new && strcmp(state->record.entity_id, "test.form.two") == 0);
    else if (strcmp(name, "prompt-cancel") == 0)
      CHECK(!UmiFormRecordEditorGtk4CanClose(root) &&
            strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "parcel.mass.value"))), "7") == 0);
    else
      CHECK(UmiFormRecordEditorGtk4CanClose(root) &&
            strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "parcel.mass.value"))), "2") == 0);
    goto cleanup;
  }
  if (g_str_has_prefix(name, "recovery-")) {
    recovery = g_build_filename(directory, "saved-draft.umidraft", NULL);
    CHECK(text(root, "parcel.mass.value", "6"));
    OK(UmiFormRecordEditorGtk4KeepRecovery(root, recovery));
    if (strcmp(name, "recovery-retire-write") == 0)
      UmiFormRecordEditorGtk4Retire(root);
    if (strcmp(name, "recovery-later-edit") == 0)
      CHECK(text(root, "parcel.mass.value", "7"));
    CHECK(idle(root));
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    CHECK(g_file_test(recovery, G_FILE_TEST_IS_REGULAR));
    if (strcmp(name, "recovery-retire-write") == 0)
      goto cleanup;
    if (strcmp(name, "recovery-later-edit") == 0) {
      CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "parcel.mass.value"))), "7") == 0);
      CHECK(!UmiFormRecordEditorGtk4CanClose(root));
      goto cleanup;
    }
    if (strcmp(name, "recovery-exclusive") == 0) {
      OK(UmiFormRecordEditorGtk4KeepRecovery(root, recovery));
      CHECK(idle(root));
      CHECK(UmiFormRecordEditorGtk4LastStatus(root) == UMI_STATUS_ALREADY_EXISTS);
      goto cleanup;
    }
    if (strcmp(name, "recovery-dirty") == 0) {
      CHECK(UmiFormRecordEditorGtk4Recover(root, recovery) == UMI_STATUS_INVALID_STATE);
      goto cleanup;
    }
    OK(UmiFormRecordEditorGtk4Discard(root));
    if (strcmp(name, "recovery-wrong-store") == 0 || strcmp(name, "recovery-namespace") == 0) {
      UmiFormRecordEditorGtk4Retire(root);
      g_object_unref(root);
      root = NULL;
      if (strcmp(name, "recovery-namespace") == 0)
        options.entity_prefix = "other.form.";
      OK(UmiFormRecordEditorGtk4New(&options, &root));
      g_object_ref_sink(root);
      char *other = strcmp(name, "recovery-wrong-store") == 0
                        ? g_build_filename(directory, "other.sqlite3", NULL)
                        : g_strdup(database);
      UmiStatus connected = UmiFormRecordEditorGtk4Connect(root, other);
      g_free(other);
      OK(connected);
      CHECK(idle(root));
      OK(UmiFormRecordEditorGtk4LastStatus(root));
      OK(UmiFormRecordEditorGtk4Recover(root, recovery));
      CHECK(idle(root));
      CHECK(UmiFormRecordEditorGtk4LastStatus(root) != UMI_STATUS_OK);
      OK(UmiFormRecordEditorGtk4State(root, state));
      CHECK(!state->record.ready);
      goto cleanup;
    }
    OK(UmiFormRecordEditorGtk4Recover(root, recovery));
    if (strcmp(name, "recovery-retire-read") == 0)
      UmiFormRecordEditorGtk4Retire(root);
    CHECK(idle(root));
    if (strcmp(name, "recovery-retire-read") == 0) {
      CHECK(UmiFormRecordEditorGtk4LastStatus(root) == UMI_STATUS_CANCELLED);
      goto cleanup;
    }
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    OK(UmiFormRecordEditorGtk4State(root, state));
    CHECK(state->record.expected_revision == 1 && state->record.dirty &&
          state->form.answers[0].value.decimal_value == 6);
    CHECK(!UmiFormRecordEditorGtk4CanClose(root));
    goto cleanup;
  }
  CHECK(UmiFormRecordEditorGtk4CanClose(root));
  if (strcmp(name, "dirty-open") == 0) {
    CHECK(text(root, "parcel.mass.value", "broken"));
    CHECK(UmiFormRecordEditorGtk4Open(root, "two") == UMI_STATUS_INVALID_STATE);
    CHECK(UmiFormRecordEditorGtk4Begin(root, "two") == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "discard-saved") == 0) {
    CHECK(text(root, "parcel.mass.value", "broken"));
    OK(UmiFormRecordEditorGtk4Discard(root));
    CHECK(UmiFormRecordEditorGtk4CanClose(root));
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "parcel.mass.value"))), "2") == 0);
    goto cleanup;
  }
  if (strcmp(name, "stale") == 0) {
    UmiRecordRevision written;
    UmiFormAnswer replacement = form_mass(8);
    OK(umi_structured_form_set(&fixture->schema, &stored->form, stored->form.revision,
                               "parcel.mass", &replacement));
    OK(umi_structured_form_record_save(reader, &stored->record.amendment, &fixture->schema,
                                       &stored->form, 1, &written));
    CHECK(text(root, "parcel.mass.value", "3"));
    OK(UmiFormRecordEditorGtk4Save(root, &attribution, 1));
    CHECK(idle(root));
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) == UMI_STATUS_INVALID_STATE);
    CHECK(!UmiFormRecordEditorGtk4CanClose(root));
    OK(umi_structured_form_record_load(reader, "test.form.one", 0, &fixture->schema, stored));
    CHECK(stored->form.answers[0].value.decimal_value == 8);
    goto cleanup;
  }
  OK(UmiFormRecordEditorGtk4Open(root, "one"));
  if (strcmp(name, "read-later-edit") == 0)
    CHECK(text(root, "parcel.mass.value", "7"));
  if (strcmp(name, "retire-read") == 0)
    UmiFormRecordEditorGtk4Retire(root);
  CHECK(idle(root));
  if (strcmp(name, "read-later-edit") == 0) {
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) == UMI_STATUS_INVALID_STATE);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "parcel.mass.value"))), "7") == 0);
  } else if (strcmp(name, "retire-read") == 0)
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) == UMI_STATUS_CANCELLED);
  else {
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    OK(UmiFormRecordEditorGtk4State(root, state));
    CHECK(state->record.expected_revision == 1 && state->form.answers[0].value.decimal_value == 2);
  }
cleanup:
  if (root != NULL) {
    if (!idle(root))
      result = 1;
  }
  if (observer_id != 0)
    g_signal_handler_disconnect(label, observer_id);
  g_clear_object(&label);
  g_clear_object(&retained);
  if (window != NULL)
    gtk_window_destroy(GTK_WINDOW(window));
  if (root != NULL) {
    UmiFormRecordEditorGtk4Retire(root);
    g_object_unref(root);
  }
  umi_data_server_destroy(reader);
  if (database != NULL) {
    (void)g_remove(database);
    const char *suffixes[] = {"-journal", "-wal", "-shm"};
    for (size_t i = 0; i < 3; ++i) {
      char *side = g_strconcat(database, suffixes[i], NULL);
      (void)g_remove(side);
      g_free(side);
    }
  }
  if (recovery != NULL)
    (void)g_remove(recovery);
  if (directory != NULL) {
    char *other = g_build_filename(directory, "other.sqlite3", NULL);
    (void)g_remove(other);
    g_free(other);
    (void)g_rmdir(directory);
  }
  g_free(recovery);
  g_free(database);
  g_free(directory);
  free(stored);
  free(state);
  free(fixture);
  return result;
}
