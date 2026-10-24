/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native replacement and retirement must preserve ownership and unsaved input. */
#include "authorized_form_editor_fixture.h"
static int exercise(NativeAccessFixture *f, const char *name) {
  UmiAuthorizedFormEditorGtk4State state;
  if (!strcmp(name, "initial")) {
    OK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state));
    CHECK(!state.ready && !state.busy && state.can_close && state.generation == 0);
  } else if (!strcmp(name, "begin")) {
    OK(UmiAuthorizedFormEditorGtk4Begin(f->editor, "example.new"));
    OK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state));
    CHECK(state.ready && !state.has_baseline && state.generation == 1 && !state.can_close);
    CHECK(!strcmp(state.entity_id, "example.new"));
  } else if (!strcmp(name, "invalid-id") || !strcmp(name, "long-id")) {
    char long_id[64];
    memset(long_id, 'x', sizeof long_id);
    CHECK(UmiAuthorizedFormEditorGtk4Begin(f->editor,
                                           !strcmp(name, "long-id") ? long_id : "example.*") ==
          UMI_STATUS_INVALID_ARGUMENT);
    OK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state));
    CHECK(!state.ready);
  } else if (!strcmp(name, "dirty-begin") || !strcmp(name, "dirty-open")) {
    OK(native_open(f));
    native_mass(f, "8");
    CHECK((!strcmp(name, "dirty-begin")
               ? UmiAuthorizedFormEditorGtk4Begin(f->editor, "example.next")
               : UmiAuthorizedFormEditorGtk4Open(f->editor, "example.next")) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(native_find(f->editor, "parcel.mass.value"))),
                  "8"));
    OK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state));
    CHECK(!strcmp(state.entity_id, "example.stored") && !state.can_close);
  } else if (!strcmp(name, "discard-new") || !strcmp(name, "old-controls")) {
    OK(UmiAuthorizedFormEditorGtk4Begin(f->editor, "example.new"));
    native_mass(f, "8");
    GtkWidget *old = g_object_ref(native_find(f->editor, "authorized-form-values"));
    OK(UmiAuthorizedFormEditorGtk4Discard(f->editor));
    OK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state));
    CHECK(!state.ready && state.can_close);
    CHECK(UmiStructuredFormGtk4Apply(old, 0) == UMI_STATUS_INVALID_STATE);
    g_object_unref(old);
  } else if (!strcmp(name, "discard-saved")) {
    OK(native_open(f));
    native_mass(f, "8");
    OK(UmiAuthorizedFormEditorGtk4Discard(f->editor));
    OK(UmiAuthorizedFormEditorGtk4Snapshot(f->editor, &f->values->after));
    CHECK(f->values->after.answers[0].value.decimal_value == 3.5);
    CHECK(UmiAuthorizedFormEditorGtk4CanClose(f->editor));
  } else if (!strcmp(name, "retired") || !strcmp(name, "dispose") || !strcmp(name, "unroot")) {
    OK(native_open(f));
    if (!strcmp(name, "dispose"))
      g_object_run_dispose(G_OBJECT(f->editor));
    else if (!strcmp(name, "unroot")) {
      GtkWidget *window = gtk_window_new();
      g_object_ref_sink(window);
      gtk_window_set_child(GTK_WINDOW(window), f->editor);
      gtk_window_set_child(GTK_WINDOW(window), NULL);
      gtk_window_destroy(GTK_WINDOW(window));
      g_object_unref(window);
    } else
      UmiAuthorizedFormEditorGtk4Retire(f->editor);
    OK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state));
    CHECK(state.retired && !state.can_close);
    CHECK(UmiAuthorizedFormEditorGtk4Begin(f->editor, "example.new") == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAuthorizedFormEditorGtk4Snapshot(f->editor, &f->values->after) ==
          UMI_STATUS_INVALID_STATE);
    OK(native_read(f, "example.stored")); /* Retiring one view does not retire the shared host. */
  } else if (!strcmp(name, "retained-host")) {
    umi_record_host_release(f->host);
    f->host = NULL;
    OK(native_open(f));
    CHECK(UmiAuthorizedFormEditorGtk4CanClose(f->editor));
  } else if (!strcmp(name, "buttons-new") || !strcmp(name, "buttons-open") ||
             !strcmp(name, "buttons-discard")) {
    gtk_editable_set_text(GTK_EDITABLE(native_find(f->editor, "authorized-form-record-id")),
                          !strcmp(name, "buttons-open") ? "example.stored" : "example.new");
    g_signal_emit_by_name(native_find(f->editor, !strcmp(name, "buttons-open")
                                                     ? "authorized-form-open"
                                                     : "authorized-form-new"),
                          "clicked");
    OK(native_wait(f));
    if (!strcmp(name, "buttons-discard")) {
      native_mass(f, "8");
      g_signal_emit_by_name(native_find(f->editor, "authorized-form-discard"), "clicked");
      OK(native_wait(f));
    }
    OK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state));
    CHECK(state.ready == (strcmp(name, "buttons-discard") != 0));
  } else if (!strcmp(name, "invalid-session") || !strcmp(name, "invalid-purpose") ||
             !strcmp(name, "null-schema")) {
    char unclosed[128];
    memset(unclosed, 's', sizeof unclosed);
    UmiAuthorizedFormEditorGtk4Options options = {f->host, &f->values->schema,
                                                  f->session.session_id, "review"};
    if (!strcmp(name, "invalid-session"))
      options.session_id = unclosed;
    if (!strcmp(name, "invalid-purpose"))
      options.purpose = "review/*";
    if (!strcmp(name, "null-schema"))
      options.schema = NULL;
    GtkWidget *out = f->editor;
    CHECK(UmiAuthorizedFormEditorGtk4New(&options, &out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(out == f->editor);
  } else if (!strcmp(name, "null")) {
    CHECK(UmiAuthorizedFormEditorGtk4New(NULL, &f->editor) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAuthorizedFormEditorGtk4ReadState(NULL, &state) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!UmiAuthorizedFormEditorGtk4CanClose(NULL));
    CHECK(!UmiAuthorizedFormEditorGtk4Busy(NULL));
    UmiAuthorizedFormEditorGtk4Retire(NULL);
  } else
    return 2;
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
