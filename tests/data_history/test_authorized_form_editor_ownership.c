/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Completion belongs to captured work, even after later typing or view disposal. */
#include "authorized_form_editor_fixture.h"
typedef struct NativeObserver {
  NativeAccessFixture *fixture;
  unsigned count;
  const char *action;
  UmiStatus nested;
} NativeObserver;
static void observe(GObject *object, GParamSpec *property, gpointer data) {
  (void)object;
  (void)property;
  NativeObserver *observer = data;
  if (observer->count++)
    return;
  NativeAccessFixture *f = observer->fixture;
  if (!strcmp(observer->action, "retire"))
    UmiAuthorizedFormEditorGtk4Retire(f->editor);
  else if (!strcmp(observer->action, "dispose"))
    g_object_run_dispose(G_OBJECT(f->editor));
  else if (!strcmp(observer->action, "type"))
    native_mass(f, "9");
  else
    observer->nested = UmiAuthorizedFormEditorGtk4Open(f->editor, "example.stored");
}
static int exercise(NativeAccessFixture *f, const char *name) {
  UmiAuthorizedFormEditorGtk4State state;
  if (!strcmp(name, "disposed-load")) {
    OK(UmiAuthorizedFormEditorGtk4Open(f->editor, "example.stored"));
    g_object_run_dispose(G_OBJECT(f->editor));
    CHECK(native_wait(f) == UMI_STATUS_CANCELLED);
    OK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state));
    CHECK(state.retired && !state.ready && !state.busy);
    return 0;
  }
  OK(native_open(f));
  if (!strcmp(name, "late-open")) {
    OK(UmiAuthorizedFormEditorGtk4Open(f->editor, "example.stored"));
    native_mass(f, "9");
    CHECK(native_wait(f) == UMI_STATUS_INVALID_STATE);
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(native_find(f->editor, "parcel.mass.value"))),
                  "9"));
    OK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state));
    CHECK(state.generation == 1 && state.record_revision == 1 && !state.can_close);
    return 0;
  }
  native_mass(f, "8");
  NativeObserver observer = {.fixture = f, .nested = UMI_STATUS_OK};
  GtkWidget *watched = NULL;
  gulong handler = 0;
  bool apply_observer = !strcmp(name, "type-on-apply") || !strcmp(name, "retire-on-apply") ||
                        !strcmp(name, "dispose-on-apply") || !strcmp(name, "nested-on-apply");
  if (apply_observer) {
    observer.action = !strcmp(name, "type-on-apply")      ? "type"
                      : !strcmp(name, "retire-on-apply")  ? "retire"
                      : !strcmp(name, "dispose-on-apply") ? "dispose"
                                                          : "nested";
    watched = g_object_ref(native_find(f->editor, "structured-form-feedback"));
    handler = g_signal_connect(watched, "notify::label", G_CALLBACK(observe), &observer);
  }
  UmiStatus started =
      UmiAuthorizedFormEditorGtk4Save(f->editor, "Capture fictional answers", 0, false);
  if (apply_observer) {
    g_signal_handler_disconnect(watched, handler);
    g_object_unref(watched);
    watched = NULL;
    CHECK(observer.count > 0);
  }
  if (!strcmp(name, "retire-on-apply") || !strcmp(name, "dispose-on-apply")) {
    CHECK(started == UMI_STATUS_CANCELLED && !UmiAuthorizedFormEditorGtk4Busy(f->editor));
    OK(native_read(f, "example.stored"));
    CHECK(f->loaded->record.revision == 1);
    return 0;
  }
  OK(started);
  if (!strcmp(name, "late-input") || !strcmp(name, "late-save-retry"))
    native_mass(f, "9");
  else if (!strcmp(name, "late-applied") || !strcmp(name, "discard-after-late-save")) {
    native_mass(f, "9");
    OK(UmiStructuredFormGtk4Apply(native_find(f->editor, "authorized-form-values"), 0));
  } else if (!strcmp(name, "retired-save"))
    UmiAuthorizedFormEditorGtk4Retire(f->editor);
  else if (!strcmp(name, "disposed-save"))
    g_object_run_dispose(G_OBJECT(f->editor));
  else if (!strcmp(name, "busy")) {
    CHECK(UmiAuthorizedFormEditorGtk4Open(f->editor, "example.stored") == UMI_STATUS_BUSY);
    CHECK(UmiAuthorizedFormEditorGtk4Begin(f->editor, "example.new") == UMI_STATUS_BUSY);
    CHECK(UmiAuthorizedFormEditorGtk4Discard(f->editor) == UMI_STATUS_BUSY);
    CHECK(UmiAuthorizedFormEditorGtk4Save(f->editor, "Again", 0, false) == UMI_STATUS_BUSY);
    CHECK(!UmiAuthorizedFormEditorGtk4CanClose(f->editor));
  } else if (!strcmp(name, "nested-on-complete") || !strcmp(name, "retire-on-complete")) {
    observer.action = !strcmp(name, "retire-on-complete") ? "retire" : "nested";
    watched = g_object_ref(native_find(f->editor, "authorized-form-feedback"));
    handler = g_signal_connect(watched, "notify::label", G_CALLBACK(observe), &observer);
  } else if (!strcmp(name, "type-on-apply") || !strcmp(name, "nested-on-apply")) {
  } else
    return 2;
  OK(native_wait(f));
  if (watched) {
    g_signal_handler_disconnect(watched, handler);
    g_object_unref(watched);
  }
  OK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state));
  CHECK(state.record_revision == 2 && state.last_status == UMI_STATUS_OK && !state.busy);
  OK(native_read(f, "example.stored"));
  CHECK(f->loaded->form.answers[0].value.decimal_value == 8.0);
  if (!strcmp(name, "retired-save") || !strcmp(name, "disposed-save") ||
      !strcmp(name, "retire-on-complete"))
    CHECK(state.retired && !state.can_close);
  bool late = !strcmp(name, "late-input") || !strcmp(name, "late-applied") ||
              !strcmp(name, "late-save-retry") || !strcmp(name, "discard-after-late-save") ||
              !strcmp(name, "type-on-apply");
  if (late) {
    CHECK(!state.can_close);
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(native_find(f->editor, "parcel.mass.value"))),
                  "9"));
  }
  if (!strcmp(name, "nested-on-apply") || !strcmp(name, "nested-on-complete"))
    CHECK(observer.count > 0 && observer.nested == UMI_STATUS_BUSY);
  if (!strcmp(name, "late-save-retry")) {
    OK(UmiAuthorizedFormEditorGtk4Save(f->editor, "Save later typing", 0, false));
    OK(native_wait(f));
    OK(native_read(f, "example.stored"));
    CHECK(f->loaded->record.revision == 3 && f->loaded->form.answers[0].value.decimal_value == 9.0);
    CHECK(UmiAuthorizedFormEditorGtk4CanClose(f->editor));
  }
  if (!strcmp(name, "discard-after-late-save")) {
    OK(UmiAuthorizedFormEditorGtk4Discard(f->editor));
    OK(UmiAuthorizedFormEditorGtk4Snapshot(f->editor, &f->values->after));
    CHECK(f->values->after.answers[0].value.decimal_value == 8.0);
    CHECK(UmiAuthorizedFormEditorGtk4CanClose(f->editor));
  }
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 3)
    return 2;
  if (!gtk_init_check()) {
    puts("SKIP: GTK4 display unavailable");
    return 77;
  }
  NativeAccessFixture *f = NULL;
  UmiStatus s = native_new(argv[1], &f);
  int result = s == UMI_STATUS_OK ? exercise(f, argv[2]) : 1;
  if (!native_free(f))
    result = 1;
  return result;
}
