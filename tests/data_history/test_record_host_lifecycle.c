/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Worker references retain storage; retiring refuses subsequent activity. */
#include "record_host_fixture.h"
static int exercise(HostFixture *f, const char *name) {
  UmiRecordHostState state;
  UmiSessionRecord session;
  memset(&session, 0xa5, sizeof session);
  if (!strcmp(name, "initial")) {
    OK(umi_record_host_state(f->host, &state));
    CHECK(!state.retired && state.identities == 1 && state.sessions == 1 &&
          state.policy_rules == 2 && state.policy_revision == 2);
  } else if (!strcmp(name, "retain")) {
    UmiRecordHost *worker = f->host;
    OK(umi_record_host_retain(worker));
    umi_record_host_release(f->host);
    f->host = NULL;
    OK(umi_record_host_state(worker, &state));
    CHECK(state.identities == 1);
    umi_record_host_release(worker);
  } else if (!strcmp(name, "retire")) {
    OK(umi_record_host_retire(f->host));
    OK(umi_record_host_retire(f->host));
    OK(umi_record_host_state(f->host, &state));
    CHECK(state.retired);
    CHECK(umi_record_host_issue_session(f->host, "example.editor", 101, 100, &session) ==
          UMI_STATUS_CANCELLED);
    CHECK(host_filled(&session, sizeof session, 0xa5));
  } else if (!strcmp(name, "retired-register")) {
    UmiIdentityRecord identity = {.identity_id = "new", .display_name = "New", .enabled = 1};
    OK(umi_record_host_retire(f->host));
    CHECK(umi_record_host_register_identity(f->host, &identity) == UMI_STATUS_CANCELLED);
    OK(umi_record_host_state(f->host, &state));
    CHECK(state.identities == 1);
  } else if (!strcmp(name, "retired-enable")) {
    OK(umi_record_host_retire(f->host));
    CHECK(umi_record_host_set_identity_enabled(f->host, "example.editor", 0) ==
          UMI_STATUS_CANCELLED);
  } else if (!strcmp(name, "retired-revoke")) {
    OK(umi_record_host_retire(f->host));
    CHECK(umi_record_host_revoke_session(f->host, f->session.session_id) == UMI_STATUS_CANCELLED);
  } else if (!strcmp(name, "disabled-issuance")) {
    OK(umi_record_host_set_identity_enabled(f->host, "example.editor", 0));
    CHECK(umi_record_host_issue_session(f->host, "example.editor", 101, 100, &session) ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(host_filled(&session, sizeof session, 0xa5));
  } else if (!strcmp(name, "missing-identity")) {
    CHECK(umi_record_host_issue_session(f->host, "missing", 101, 100, &session) ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(host_filled(&session, sizeof session, 0xa5));
  } else if (!strcmp(name, "zero-clock")) {
    CHECK(umi_record_host_issue_session(f->host, "example.editor", 0, 100, &session) ==
          UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "expiry-overflow")) {
    CHECK(umi_record_host_issue_session(f->host, "example.editor", UINT64_MAX, 1, &session) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(host_filled(&session, sizeof session, 0xa5));
  } else if (!strcmp(name, "factory-invalid")) {
    UmiRecordHostOptions options = {
        .backend = 99, .source = "Example", .entity_prefix = "example."};
    UmiRecordHost *result = f->host;
    CHECK(umi_record_host_create(&options, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result == f->host);
    options.backend = UMI_RECORD_HOST_SQLITE;
    CHECK(umi_record_host_create(&options, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result == f->host);
    options.backend = UMI_RECORD_HOST_MEMORY;
    options.database_path = "unwanted.db";
    CHECK(umi_record_host_create(&options, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result == f->host);
  } else if (!strcmp(name, "factory-scope")) {
    UmiRecordHostOptions options = {
        .backend = UMI_RECORD_HOST_MEMORY, .source = "Example", .entity_prefix = "*"};
    UmiRecordHost *result = f->host;
    CHECK(umi_record_host_create(&options, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result == f->host);
  } else if (!strcmp(name, "null")) {
    CHECK(umi_record_host_create(NULL, &f->host) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_host_retain(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_host_retire(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_host_state(f->host, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    umi_record_host_release(NULL);
  } else
    return 2;
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
