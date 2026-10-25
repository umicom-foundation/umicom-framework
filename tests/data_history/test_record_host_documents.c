/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_host_documents.c
 *
 * PURPOSE:
 *   Storage and current security changes share the owned host boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Storage and current security changes share the owned host boundary. */
#include "record_host_fixture.h"
static int exercise(HostFixture *f, const char *name) {
  UmiAuthorizedRecordRevision first, second;
  OK(host_save(f, "first", 0, &first));
  OK(host_save(f, "second", 1, &second));
  UmiAuthorizedRecordDocument *out = malloc(sizeof *out);
  CHECK(out != NULL);
  memset(out, 0xa5, sizeof *out);
  f->request.operation = UMI_RECORD_ACCESS_READ_HEAD;
  UmiStatus expected = UMI_STATUS_OK, s;
  int mode = 0;
  if (!strcmp(name, "latest")) {
  } else if (!strcmp(name, "history")) {
    mode = 1;
    f->request.operation = UMI_RECORD_ACCESS_READ_HISTORY;
  } else if (!strcmp(name, "reviewed"))
    mode = 2;
  else if (!strcmp(name, "stale-reviewed")) {
    mode = 3;
    expected = UMI_STATUS_INVALID_STATE;
  } else if (!strcmp(name, "disabled")) {
    OK(umi_record_host_set_identity_enabled(f->host, "example.editor", 0));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "revoked")) {
    OK(umi_record_host_revoke_session(f->host, f->session.session_id));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "retired")) {
    OK(umi_record_host_retire(f->host));
    expected = UMI_STATUS_CANCELLED;
  } else if (!strcmp(name, "policy-changed")) {
    uint64_t revision;
    OK(host_policy(f->host, UMI_POLICY_DENY, &revision));
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "wrong-purpose")) {
    strcpy(f->request.purpose, "export");
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "stale-amend")) {
    UmiAuthorizedRecordRevision saved;
    memset(&saved, 0xa5, sizeof saved);
    CHECK(host_save(f, "stale", 1, &saved) == UMI_STATUS_INVALID_STATE);
    CHECK(host_filled(&saved, sizeof saved, 0xa5));
    f->request.operation = UMI_RECORD_ACCESS_READ_HEAD;
  } else if (!strcmp(name, "null")) {
    CHECK(umi_record_host_latest(f->host, NULL, out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_host_latest(NULL, &f->request, out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(host_filled(out, sizeof *out, 0xa5));
    free(out);
    return 0;
  } else {
    free(out);
    return 2;
  }
  if (mode == 1)
    s = umi_record_host_load(f->host, &f->request, 1, out);
  else if (mode >= 2)
    s = umi_record_host_open_reviewed(f->host, &f->request,
                                      mode == 2 ? &second.record : &first.record, out);
  else
    s = umi_record_host_latest(f->host, &f->request, out);
  CHECK(s == expected);
  if (s != UMI_STATUS_OK)
    CHECK(host_filled(out, sizeof *out, 0xa5));
  else {
    CHECK(out->document.record.revision == (mode == 1 ? 1U : 2U));
    CHECK(out->document.length == (mode == 1 ? 5U : 6U));
    CHECK(!memcmp(out->document.content, mode == 1 ? "first" : "second", out->document.length));
    CHECK(!strcmp(out->document.record.amendment.actor, "example.editor"));
    CHECK(!strcmp(out->access.source, "Fictional record host"));
  }
  free(out);
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 3)
    return 2;
  HostFixture f;
  UmiStatus s = host_new(&f, argv[1], true);
  int result = s == UMI_STATUS_OK ? exercise(&f, argv[2]) : 1;
  umi_record_host_release(f.host);
  return result;
}
