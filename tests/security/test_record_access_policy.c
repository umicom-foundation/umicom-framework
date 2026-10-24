/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Permissions must be current for both the operation and its declared purpose. */
#include "record_access_fixture.h"
static int exercise(AccessFixture *f, const char *name) {
  UmiRecordAccessReceipt receipt;
  memset(&receipt, 0xa5, sizeof receipt);
  UmiStatus expected = UMI_STATUS_PERMISSION_DENIED;
  if (!strcmp(name, "allow")) {
    OK(access_allow(f));
    expected = UMI_STATUS_OK;
  } else if (!strcmp(name, "no-rules")) { /* Absence must not grant access. */
  } else if (!strcmp(name, "operation-only"))
    OK(access_rule(f, "example.editor", "record.read", "example.*", UMI_POLICY_ALLOW));
  else if (!strcmp(name, "purpose-only"))
    OK(access_rule(f, "example.editor", "record.purpose", "purpose/*", UMI_POLICY_ALLOW));
  else {
    OK(access_allow(f));
    if (!strcmp(name, "wrong-purpose"))
      strcpy(f->request.purpose, "export");
    else if (!strcmp(name, "wrong-principal")) {
      OK(umi_policy_engine_clear(f->policy));
      OK(access_rule(f, "other", "*", "*", UMI_POLICY_ALLOW));
    } else if (!strcmp(name, "purpose-other-record")) {
      OK(umi_policy_engine_clear(f->policy));
      OK(access_rule(f, "example.editor", "record.read", "example.*", UMI_POLICY_ALLOW));
      OK(access_rule(f, "example.editor", "record.purpose", "purpose/review/record/example.other",
                     UMI_POLICY_ALLOW));
    } else if (!strcmp(name, "explicit-operation-deny"))
      OK(access_rule(f, "example.editor", "record.read", "example.record", UMI_POLICY_DENY));
    else if (!strcmp(name, "explicit-purpose-deny"))
      OK(access_rule(f, "example.editor", "record.purpose", "purpose/*", UMI_POLICY_DENY));
    else if (!strcmp(name, "last-matching-rule")) {
      OK(access_rule(f, "example.editor", "record.read", "example.*", UMI_POLICY_DENY));
      OK(access_rule(f, "example.editor", "record.read", "example.record", UMI_POLICY_ALLOW));
      expected = UMI_STATUS_OK;
    } else if (!strcmp(name, "unknown-session"))
      strcpy(f->request.session_id, "unknown");
    else if (!strcmp(name, "revoked"))
      OK(umi_session_registry_revoke(f->sessions, f->session.session_id));
    else if (!strcmp(name, "expired"))
      f->request.now_nanoseconds = 1100;
    else if (!strcmp(name, "before-issued"))
      f->request.now_nanoseconds = 99;
    else if (!strcmp(name, "clock-rollback")) {
      OK(umi_record_access_authorize(f->authority, &f->request, &receipt));
      memset(&receipt, 0xa5, sizeof receipt);
      f->request.now_nanoseconds = 100;
    } else if (!strcmp(name, "disabled"))
      OK(umi_identity_registry_set_enabled(f->identities, "example.editor", 0));
    else if (!strcmp(name, "reenabled")) {
      OK(umi_identity_registry_set_enabled(f->identities, "example.editor", 0));
      CHECK(umi_record_access_authorize(f->authority, &f->request, &receipt) == expected);
      OK(umi_identity_registry_set_enabled(f->identities, "example.editor", 1));
      expected = UMI_STATUS_OK;
    } else if (!strcmp(name, "missing-identity")) {
      UmiSessionRecord unknown;
      OK(umi_session_registry_issue(f->sessions, "missing", 100, 1000, &unknown));
      strcpy(f->request.session_id, unknown.session_id);
    } else if (!strcmp(name, "current-revision")) {
      OK(umi_identity_registry_set_enabled(f->identities, "example.editor", 1));
      expected = UMI_STATUS_OK;
    } else if (!strcmp(name, "history-capability")) {
      OK(umi_policy_engine_clear(f->policy));
      OK(access_rule(f, "example.editor", "record.read", "example.*", UMI_POLICY_ALLOW));
      OK(access_rule(f, "example.editor", "record.purpose", "purpose/*", UMI_POLICY_ALLOW));
      f->request.operation = UMI_RECORD_ACCESS_READ_HISTORY;
    } else if (!strcmp(name, "create-capability")) {
      f->request.operation = UMI_RECORD_ACCESS_CREATE;
      expected = UMI_STATUS_OK;
    } else if (!strcmp(name, "amend-capability")) {
      f->request.operation = UMI_RECORD_ACCESS_AMEND;
      expected = UMI_STATUS_OK;
    } else if (!strcmp(name, "clear-policy"))
      OK(umi_policy_engine_clear(f->policy));
    else if (!strcmp(name, "receipt-is-not-authority")) {
      OK(umi_record_access_authorize(f->authority, &f->request, &receipt));
      OK(umi_session_registry_revoke(f->sessions, f->session.session_id));
      memset(&receipt, 0xa5, sizeof receipt);
    } else
      return 2;
  }
  CHECK(umi_record_access_authorize(f->authority, &f->request, &receipt) == expected);
  if (expected != UMI_STATUS_OK)
    CHECK(access_filled(&receipt, sizeof receipt, 0xa5));
  else {
    UmiIdentityRecord current;
    OK(umi_identity_registry_get(f->identities, "example.editor", &current));
    CHECK(receipt.identity_revision == current.revision);
    CHECK(!strcmp(receipt.principal_id, current.identity_id));
    CHECK(receipt.session_expires_at_nanoseconds == 1100);
  }
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  AccessFixture f;
  UmiStatus s = access_new(&f, "memory");
  int result = s == UMI_STATUS_OK ? exercise(&f, argv[1]) : 1;
  access_free(&f);
  return result;
}
