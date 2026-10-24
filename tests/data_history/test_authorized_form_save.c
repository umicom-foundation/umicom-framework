/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Preserve draft edits and reject unauthorized or incompatible amendments. */
#include "authorized_form_fixture.h"
static int exercise(AuthorizedFormFixture *f, const char *name) {
  UmiStructuredFormSchema schema = f->values->schema;
  uint64_t expected_revision = 0;
  UmiStatus expected = UMI_STATUS_OK;
  bool seeded = false;
  const char *reason = "Review fictional form";
  if (!strcmp(name, "create") || !strcmp(name, "unanswered-draft") ||
      !strcmp(name, "caller-dirty")) {
  } else if (!strcmp(name, "occupied")) {
    OK(authorized_form_save(f, 0, &f->saved));
    seeded = true;
    expected = UMI_STATUS_INVALID_STATE;
  } else if (!strcmp(name, "foreign-previous")) {
    UmiRecordRevision other;
    OK(access_seed(&f->access, "example.record", "plain", 0, &other));
    seeded = true;
    expected_revision = 1;
    expected = UMI_STATUS_PARSE_ERROR;
  } else if (!strcmp(name, "amend") || !strcmp(name, "stale") || !strcmp(name, "previous-schema") ||
             !strcmp(name, "read-required") || !strcmp(name, "amend-required") ||
             !strcmp(name, "revoked-after-review") || !strcmp(name, "edit-versus-record")) {
    OK(authorized_form_save(f, 0, &f->saved));
    seeded = true;
    expected_revision = 1;
    if (!strcmp(name, "previous-schema")) {
      /* The independently owned schema is not borrowed by a live widget. */
      schema.version = 2;
      OK(umi_structured_form_init(&schema, &f->values->after));
      f->values->before = f->values->after;
      expected = UMI_STATUS_INVALID_STATE;
    } else {
      UmiFormAnswer answer = form_mass(8);
      OK(umi_structured_form_set(&schema, &f->values->before, 2, "parcel.mass", &answer));
    }
    if (!strcmp(name, "stale")) {
      expected_revision = 2;
      expected = UMI_STATUS_INVALID_STATE;
    }
    if (!strcmp(name, "read-required")) {
      OK(access_rule(&f->access, "example.editor", "record.read", "*", UMI_POLICY_DENY));
      expected = UMI_STATUS_PERMISSION_DENIED;
    }
    if (!strcmp(name, "amend-required")) {
      OK(access_rule(&f->access, "example.editor", "record.amend", "*", UMI_POLICY_DENY));
      expected = UMI_STATUS_PERMISSION_DENIED;
    }
    if (!strcmp(name, "revoked-after-review")) {
      OK(umi_session_registry_revoke(f->access.sessions, f->access.session.session_id));
      expected = UMI_STATUS_PERMISSION_DENIED;
    }
  } else if (!strcmp(name, "create-denied")) {
    OK(access_rule(&f->access, "example.editor", "record.create", "*", UMI_POLICY_DENY));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "disabled")) {
    OK(umi_identity_registry_set_enabled(f->access.identities, "example.editor", 0));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "wrong-purpose")) {
    strcpy(f->access.request.purpose, "export");
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "invalid-answer")) {
    strcpy(f->values->before.answers[0].unit_id, "mass.lb");
    expected = UMI_STATUS_INVALID_STATE;
  } else if (!strcmp(name, "wrong-input-schema")) {
    schema.version++;
    expected = UMI_STATUS_INVALID_STATE;
  } else if (!strcmp(name, "wrong-mode")) {
    f->access.request.operation = UMI_RECORD_ACCESS_READ_HEAD;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "nested")) {
    OK(umi_data_server_begin(f->access.server));
    expected = UMI_STATUS_BUSY;
  } else if (!strcmp(name, "invalid-reason")) {
    reason = "bad\nreason";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "overflow")) {
    expected_revision = UINT64_MAX;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  } else if (!strcmp(name, "null")) {
    CHECK(umi_authorized_form_record_save(NULL, &f->access.request, &schema, &f->values->before,
                                          reason, 0, 0, &f->saved) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_form_record_save(f->access.records, NULL, &schema, &f->values->before,
                                          reason, 0, 0, &f->saved) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_form_record_save(f->access.records, &f->access.request, NULL,
                                          &f->values->before, reason, 0, 0,
                                          &f->saved) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_form_record_save(f->access.records, &f->access.request, &schema, NULL,
                                          reason, 0, 0, &f->saved) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_form_record_save(f->access.records, &f->access.request, &schema,
                                          &f->values->before, NULL, 0, 0,
                                          &f->saved) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_form_record_save(f->access.records, &f->access.request, &schema,
                                          &f->values->before, reason, 0, 0,
                                          NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  } else
    return 2;
  if (strcmp(name, "wrong-mode"))
    f->access.request.operation =
        expected_revision ? UMI_RECORD_ACCESS_AMEND : UMI_RECORD_ACCESS_CREATE;
  /* Draft storage and submission validation have different contracts. */
  if (!strcmp(name, "unanswered-draft")) {
    UmiApplicationExperienceValidationReport report;
    OK(umi_structured_form_validate(&schema, &f->values->before, 0, &report));
    CHECK(report.error_count == 0);
    OK(umi_structured_form_validate(&schema, &f->values->before, 1, &report));
    CHECK(report.error_count > 0);
  }
  if (!strcmp(name, "caller-dirty")) {
    UmiFormAnswer note = form_text("Unsubmitted draft note");
    OK(umi_structured_form_set(&schema, &f->values->before, 2, "parcel.note", &note));
    CHECK(f->values->before.revision == 3 && f->values->before.saved_revision == 1);
  }
  UmiStructuredForm *before = malloc(sizeof *before);
  CHECK(before != NULL);
  /* Byte-preservation assertions need a byte-identical baseline. Structure
   * assignment does not promise to copy padding; retain the former assertion
   * setup for review and capture the object representation explicitly. */
#if 0
  *before = f->values->before;
#endif
  memcpy(before, &f->values->before, sizeof *before);
  memset(&f->saved, 0xa5, sizeof f->saved);
  CHECK(umi_authorized_form_record_save(f->access.records, &f->access.request, &schema,
                                        &f->values->before, reason, 100, expected_revision,
                                        &f->saved) == expected);
  CHECK(!memcmp(before, &f->values->before, sizeof *before));
  free(before);
  if (expected == UMI_STATUS_OK) {
    CHECK(f->saved.record.revision == expected_revision + 1);
    CHECK(!strcmp(f->saved.record.amendment.actor, "example.editor"));
    CHECK(f->values->before.saved_revision < f->values->before.revision);
    f->access.request.operation = UMI_RECORD_ACCESS_READ_HEAD;
    OK(umi_authorized_form_record_load(f->access.records, &f->access.request, 0, &schema,
                                       f->loaded));
    CHECK(f->loaded->form.saved_revision == f->loaded->form.revision);
    if (!strcmp(name, "edit-versus-record"))
      CHECK(f->loaded->form.revision == 3 && f->loaded->record.revision == 2);
  } else {
    CHECK(access_filled(&f->saved, sizeof f->saved, 0xa5));
    if (!strcmp(name, "nested")) {
      CHECK(umi_data_server_in_transaction(f->access.server));
      OK(umi_data_server_rollback(f->access.server));
    }
    UmiRecordRevision current;
    UmiStatus found = umi_record_history_latest(f->access.server, "example.record", &current);
    CHECK(found == (seeded ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND));
    if (seeded)
      CHECK(current.revision == 1);
  }
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 3 || (strcmp(argv[1], "memory") && strcmp(argv[1], "sqlite")))
    return 2;
  AuthorizedFormFixture f;
  UmiStatus s = authorized_form_new(&f, argv[1]);
  int result = s == UMI_STATUS_OK ? exercise(&f, argv[2]) : 1;
  authorized_form_free(&f);
  return result;
}
