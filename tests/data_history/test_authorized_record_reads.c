/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_authorized_record_reads.c
 *
 * PURPOSE:
 *   A denied or stale read must reveal neither document bytes nor a new receipt.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A denied or stale read must reveal neither document bytes nor a new receipt. */
#include "../security/record_access_fixture.h"
static int exercise(AccessFixture *f, const char *name) {
  UmiRecordRevision first, second;
  OK(access_seed(f, "example.record", "first", 0, &first));
  OK(access_seed(f, "example.record", "second", 1, &second));
  UmiAuthorizedRecordDocument *out = malloc(sizeof *out);
  CHECK(out != NULL);
  memset(out, 0xa5, sizeof *out);
  UmiStatus expected = UMI_STATUS_OK, status;
  unsigned mode = 0;
  uint64_t revision = 1;
  UmiRecordRevision reviewed = second;
  if (!strcmp(name, "latest")) {
  } else if (!strcmp(name, "history")) {
    mode = 1;
    f->request.operation = UMI_RECORD_ACCESS_READ_HISTORY;
  } else if (!strcmp(name, "reviewed"))
    mode = 2;
  else if (!strcmp(name, "reviewed-stale")) {
    mode = 2;
    reviewed = first;
    expected = UMI_STATUS_INVALID_STATE;
  } else if (!strcmp(name, "reviewed-other-entity")) {
    mode = 2;
    strcpy(reviewed.amendment.entity_id, "example.other");
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "reviewed-unclosed")) {
    mode = 2;
    memset(reviewed.amendment.entity_id, 'x', sizeof reviewed.amendment.entity_id);
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "reviewed-tampered")) {
    mode = 2;
    strcpy(reviewed.amendment.actor, "forged");
    expected = UMI_STATUS_INVALID_STATE;
  } else if (!strcmp(name, "wrong-latest-operation")) {
    f->request.operation = UMI_RECORD_ACCESS_READ_HISTORY;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "wrong-history-operation")) {
    mode = 1;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "zero-revision")) {
    mode = 1;
    revision = 0;
    f->request.operation = UMI_RECORD_ACCESS_READ_HISTORY;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "missing-revision")) {
    mode = 1;
    revision = 3;
    f->request.operation = UMI_RECORD_ACCESS_READ_HISTORY;
    expected = UMI_STATUS_NOT_FOUND;
  } else if (!strcmp(name, "missing-entity")) {
    strcpy(f->request.entity_id, "example.absent");
    expected = UMI_STATUS_NOT_FOUND;
  } else if (!strcmp(name, "permission-denied")) {
    OK(umi_policy_engine_clear(f->policy));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "history-denied")) {
    mode = 1;
    f->request.operation = UMI_RECORD_ACCESS_READ_HISTORY;
    OK(access_rule(f, "example.editor", "record.history", "*", UMI_POLICY_DENY));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "revoked")) {
    OK(umi_session_registry_revoke(f->sessions, f->session.session_id));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "nested")) {
    OK(umi_data_server_begin(f->server));
    OK(umi_data_server_set(f->server, "pending", "retain"));
    expected = UMI_STATUS_BUSY;
  } else if (!strcmp(name, "no-existence-leak")) {
    strcpy(f->request.entity_id, "example.absent");
    OK(umi_policy_engine_clear(f->policy));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "null")) {
    CHECK(umi_authorized_record_latest(NULL, &f->request, out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_record_latest(f->records, NULL, out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_record_latest(f->records, &f->request, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_authorized_record_open_reviewed(f->records, &f->request, NULL, out) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(access_filled(out, sizeof *out, 0xa5));
    free(out);
    return 0;
  } else {
    free(out);
    return 2;
  }
  if (mode == 1)
    status = umi_authorized_record_load(f->records, &f->request, revision, out);
  else if (mode == 2)
    status = umi_authorized_record_open_reviewed(f->records, &f->request, &reviewed, out);
  else
    status = umi_authorized_record_latest(f->records, &f->request, out);
  CHECK(status == expected);
  if (status != UMI_STATUS_OK)
    CHECK(access_filled(out, sizeof *out, 0xa5));
  else {
    CHECK(out->document.record.revision == (mode == 1 ? 1U : 2U));
    CHECK(out->document.length == (mode == 1 ? 5U : 6U));
    CHECK(!memcmp(out->document.content, mode == 1 ? "first" : "second", out->document.length));
    CHECK(!strcmp(out->access.principal_id, "example.editor"));
    CHECK(!strcmp(out->document.record.amendment.actor, "example.seed"));
    CHECK(out->access.operation == f->request.operation);
  }
  if (!strcmp(name, "nested")) {
    CHECK(umi_data_server_in_transaction(f->server));
    OK(umi_data_server_commit(f->server));
    char value[16];
    OK(umi_data_server_get(f->server, "pending", value, sizeof value));
    CHECK(!strcmp(value, "retain"));
  }
  free(out);
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 3 || (strcmp(argv[1], "memory") && strcmp(argv[1], "sqlite")))
    return 2;
  AccessFixture f;
  UmiStatus s = access_new(&f, argv[1]);
  if (s == UMI_STATUS_OK)
    s = access_allow(&f);
  int result = s == UMI_STATUS_OK ? exercise(&f, argv[2]) : 1;
  access_free(&f);
  return result;
}
