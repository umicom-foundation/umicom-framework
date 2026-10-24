/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native controls use the same current authority as the record host. */
#include "authorized_form_editor_fixture.h"
static int exercise(NativeAccessFixture *f, const char *name) {
  UmiAuthorizedFormEditorGtk4State state;
  if (!strcmp(name, "open") || !strcmp(name, "missing") || !strcmp(name, "open-denied")) {
    if (!strcmp(name, "open-denied")) {
      uint64_t revision;
      OK(host_policy(f->host, UMI_POLICY_DENY, &revision));
    }
    OK(UmiAuthorizedFormEditorGtk4Open(f->editor, !strcmp(name, "missing") ? "example.missing"
                                                                           : "example.stored"));
    UmiStatus expected = !strcmp(name, "missing")       ? UMI_STATUS_NOT_FOUND
                         : !strcmp(name, "open-denied") ? UMI_STATUS_PERMISSION_DENIED
                                                        : UMI_STATUS_OK;
    CHECK(native_wait(f) == expected);
    OK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state));
    CHECK(state.ready == (expected == UMI_STATUS_OK));
    return 0;
  }
  bool create = !strcmp(name, "create") || !strcmp(name, "scope");
  if (create)
    OK(UmiAuthorizedFormEditorGtk4Begin(f->editor,
                                        !strcmp(name, "scope") ? "other.form" : "example.new"));
  else
    OK(native_open(f));
  if (!strcmp(name, "clean-save")) {
    CHECK(UmiAuthorizedFormEditorGtk4Save(f->editor, "No new edits", 0, false) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(!UmiAuthorizedFormEditorGtk4Busy(f->editor));
    OK(native_read(f, "example.stored"));
    CHECK(f->loaded->record.revision == 1);
    return 0;
  }
  native_mass(f, !strcmp(name, "invalid-value") ? "3kg" : "8");
  UmiStatus admission = UMI_STATUS_OK, completion = UMI_STATUS_OK;
  const char *reason = "Review fictional native answers";
  char unclosed[256];
  bool submit = false;
  if (!strcmp(name, "create") || !strcmp(name, "amend") || !strcmp(name, "buttons-save")) {
  } else if (!strcmp(name, "scope"))
    completion = UMI_STATUS_PERMISSION_DENIED;
  else if (!strcmp(name, "invalid-value"))
    admission = UMI_STATUS_PARSE_ERROR;
  else if (!strcmp(name, "submit-missing")) {
    submit = true;
    admission = UMI_STATUS_INVALID_STATE;
  } else if (!strcmp(name, "invalid-reason")) {
    reason = "bad\nreason";
    admission = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "unclosed-reason")) {
    memset(unclosed, 'r', sizeof unclosed);
    reason = unclosed;
    admission = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "disabled")) {
    OK(umi_record_host_set_identity_enabled(f->host, "example.editor", 0));
    completion = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "revoked")) {
    OK(umi_record_host_revoke_session(f->host, f->session.session_id));
    completion = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "expired")) {
    atomic_store_explicit(&f->now, 1100, memory_order_relaxed);
    completion = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "policy-denied")) {
    uint64_t revision;
    OK(host_policy(f->host, UMI_POLICY_DENY, &revision));
    completion = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "host-retired")) {
    OK(umi_record_host_retire(f->host));
    completion = UMI_STATUS_CANCELLED;
  } else if (!strcmp(name, "stale-head")) {
    UmiFormAnswer other = form_mass(6);
    OK(umi_structured_form_set(&f->values->schema, &f->values->before, 2, "parcel.mass", &other));
    f->request.operation = UMI_RECORD_ACCESS_AMEND;
    OK(umi_record_host_form_save_current(f->host, &f->request, &f->values->schema,
                                         &f->values->before, "Another fictional writer", 0, 1,
                                         &f->seeded));
    completion = UMI_STATUS_INVALID_STATE;
  } else
    return 2;
  if (!strcmp(name, "buttons-save")) {
    gtk_editable_set_text(GTK_EDITABLE(native_find(f->editor, "authorized-form-reason")), reason);
    g_signal_emit_by_name(native_find(f->editor, "authorized-form-save"), "clicked");
  } else
    CHECK(UmiAuthorizedFormEditorGtk4Save(f->editor, reason, 0, submit) == admission);
  if (admission == UMI_STATUS_OK)
    CHECK(native_wait(f) == completion);
  else
    CHECK(!UmiAuthorizedFormEditorGtk4Busy(f->editor));
  OK(UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state));
  bool saved = admission == UMI_STATUS_OK && completion == UMI_STATUS_OK;
  CHECK(state.record_revision == (saved ? (create ? 1U : 2U) : (create ? 0U : 1U)));
  CHECK(state.can_close == saved);
  if (saved) {
    OK(native_read(f, create ? "example.new" : "example.stored"));
    CHECK(f->loaded->form.answers[0].value.decimal_value == 8.0);
    CHECK(!strcmp(f->loaded->record.amendment.actor, "example.editor"));
    CHECK(!strcmp(f->loaded->record.amendment.source, "Fictional native host"));
  } else {
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(native_find(f->editor, "parcel.mass.value"))),
                  !strcmp(name, "invalid-value") ? "3kg" : "8"));
    if (!strcmp(name, "stale-head")) {
      OK(native_read(f, "example.stored"));
      CHECK(f->loaded->record.revision == 2);
      CHECK(f->loaded->form.answers[0].value.decimal_value == 6.0);
    }
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
