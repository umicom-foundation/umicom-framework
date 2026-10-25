/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_authorized_form_load.c
 *
 * PURPOSE:
 *   Publish typed answers, record identity and access receipt as one result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Publish typed answers, record identity and access receipt as one result. */
#include "authorized_form_fixture.h"
static int exercise(AuthorizedFormFixture *f, const char *name) {
  OK(authorized_form_save(f, 0, &f->saved));
  UmiRecordRevision first = f->saved.record, reviewed = first;
  UmiStructuredFormSchema schema = f->values->schema;
  uint64_t revision = 0;
  bool exact = false;
  UmiStatus expected = UMI_STATUS_OK;
  f->access.request.operation = UMI_RECORD_ACCESS_READ_HEAD;
  if (!strcmp(name, "latest")) {
  } else if (!strcmp(name, "history")) {
    revision = 1;
    f->access.request.operation = UMI_RECORD_ACCESS_READ_HISTORY;
  } else if (!strcmp(name, "reviewed"))
    exact = true;
  else if (!strcmp(name, "schema-version")) {
    schema.version++;
    expected = UMI_STATUS_INVALID_STATE;
  } else if (!strcmp(name, "foreign-payload")) {
    UmiRecordRevision other;
    OK(access_seed(&f->access, "example.foreign", "plain", 0, &other));
    strcpy(f->access.request.entity_id, "example.foreign");
    expected = UMI_STATUS_PARSE_ERROR;
  } else if (!strcmp(name, "denied-before-decode")) {
    UmiRecordRevision other;
    OK(access_seed(&f->access, "example.foreign", "plain", 0, &other));
    strcpy(f->access.request.entity_id, "example.foreign");
    OK(umi_policy_engine_clear(f->access.policy));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "history-denied")) {
    revision = 1;
    f->access.request.operation = UMI_RECORD_ACCESS_READ_HISTORY;
    OK(access_rule(&f->access, "example.editor", "record.history", "*", UMI_POLICY_DENY));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "reviewed-stale") || !strcmp(name, "latest-after-amend") ||
             !strcmp(name, "history-after-amend")) {
    UmiFormAnswer changed = form_mass(8);
    OK(umi_structured_form_set(&f->values->schema, &f->values->before, 2, "parcel.mass", &changed));
    OK(authorized_form_save(f, 1, &f->saved));
    f->access.request.operation = UMI_RECORD_ACCESS_READ_HEAD;
    if (!strcmp(name, "reviewed-stale")) {
      exact = true;
      expected = UMI_STATUS_INVALID_STATE;
    }
    if (!strcmp(name, "history-after-amend")) {
      revision = 1;
      f->access.request.operation = UMI_RECORD_ACCESS_READ_HISTORY;
    }
  } else if (!strcmp(name, "reviewed-other")) {
    exact = true;
    strcpy(reviewed.amendment.entity_id, "example.other");
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "reviewed-tampered")) {
    exact = true;
    strcpy(reviewed.amendment.reason, "Different reason");
    expected = UMI_STATUS_INVALID_STATE;
  } else if (!strcmp(name, "disabled")) {
    OK(umi_identity_registry_set_enabled(f->access.identities, "example.editor", 0));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "revoked")) {
    OK(umi_session_registry_revoke(f->access.sessions, f->access.session.session_id));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "wrong-purpose")) {
    strcpy(f->access.request.purpose, "export");
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "wrong-operation")) {
    f->access.request.operation = UMI_RECORD_ACCESS_AMEND;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "missing")) {
    strcpy(f->access.request.entity_id, "example.absent");
    expected = UMI_STATUS_NOT_FOUND;
  } else if (!strcmp(name, "nested")) {
    OK(umi_data_server_begin(f->access.server));
    expected = UMI_STATUS_BUSY;
  } else if (!strcmp(name, "null-schema")) {
    memset(f->loaded, 0xa5, sizeof *f->loaded);
    CHECK(umi_authorized_form_record_load(f->access.records, &f->access.request, 0, NULL,
                                          f->loaded) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(access_filled(f->loaded, sizeof *f->loaded, 0xa5));
    return 0;
  } else if (!strcmp(name, "null")) {
    CHECK(umi_authorized_form_record_load(NULL, &f->access.request, 0, &schema, f->loaded) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_form_record_load(f->access.records, NULL, 0, &schema, f->loaded) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_form_record_load(f->access.records, &f->access.request, 0, &schema,
                                          NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_form_record_open_reviewed(f->access.records, &f->access.request, NULL,
                                                   &schema,
                                                   f->loaded) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  } else
    return 2;
  memset(f->loaded, 0xa5, sizeof *f->loaded);
  UmiStatus s = exact ? umi_authorized_form_record_open_reviewed(
                            f->access.records, &f->access.request, &reviewed, &schema, f->loaded)
                      : umi_authorized_form_record_load(f->access.records, &f->access.request,
                                                        revision, &schema, f->loaded);
  CHECK(s == expected);
  if (s != UMI_STATUS_OK)
    CHECK(access_filled(f->loaded, sizeof *f->loaded, 0xa5));
  else {
    bool amended = !strcmp(name, "latest-after-amend");
    CHECK(f->loaded->record.revision == (amended ? 2U : 1U));
    CHECK(f->loaded->form.answers[0].value.decimal_value == (amended ? 8.0 : 3.5));
    CHECK(f->loaded->form.revision == f->loaded->form.saved_revision);
    CHECK(!strcmp(f->loaded->record.amendment.actor, "example.editor"));
    CHECK(!strcmp(f->loaded->access.purpose, "review"));
  }
  if (!strcmp(name, "nested")) {
    CHECK(umi_data_server_in_transaction(f->access.server));
    OK(umi_data_server_rollback(f->access.server));
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
