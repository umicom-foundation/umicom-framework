/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_host_policy.c
 *
 * PURPOSE:
 *   A bad replacement must never partially clear or grant record permissions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A bad replacement must never partially clear or grant record permissions. */
#include "record_host_fixture.h"
static int exercise(HostFixture *f, const char *name) {
  UmiRecordHostState before, after;
  OK(umi_record_host_state(f->host, &before));
  UmiPolicyRule rules[] = {{"example.editor", "record.*", "*", UMI_POLICY_ALLOW},
                           {"example.editor", "record.purpose", "*", UMI_POLICY_ALLOW}};
  UmiStatus expected = UMI_STATUS_OK;
  uint64_t revision = 77;
  size_t count = 2;
  const UmiPolicyRule *input = rules;
  bool permitted = true;
  char too_long[UMI_POLICY_TEXT_CAPACITY + 1];
  memset(too_long, 'p', sizeof too_long - 1);
  too_long[sizeof too_long - 1] = 0;
  if (!strcmp(name, "replace")) {
  } else if (!strcmp(name, "clear")) {
    count = 0;
    input = NULL;
    permitted = false;
  } else if (!strcmp(name, "deny")) {
    rules[0].effect = UMI_POLICY_DENY;
    rules[1].effect = UMI_POLICY_DENY;
    permitted = false;
  } else if (!strcmp(name, "partial-failure")) {
    rules[1].principal = NULL;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "invalid-effect")) {
    rules[1].effect = 99;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "long-principal")) {
    rules[1].principal = too_long;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  } else if (!strcmp(name, "too-many")) {
    count = UMI_POLICY_MAX_RULES + 1;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  } else if (!strcmp(name, "null-rules")) {
    input = NULL;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "retired")) {
    OK(umi_record_host_retire(f->host));
    expected = UMI_STATUS_CANCELLED;
  } else if (!strcmp(name, "last-match")) {
    rules[0].effect = UMI_POLICY_DENY;
    rules[1].capability = "*";
  } else if (!strcmp(name, "null-output")) {
    CHECK(umi_record_host_replace_policy(f->host, rules, 2, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    OK(umi_record_host_state(f->host, &after));
    CHECK(after.policy_revision == before.policy_revision);
    return 0;
  } else
    return 2;
  CHECK(umi_record_host_replace_policy(f->host, input, count, &revision) == expected);
  OK(umi_record_host_state(f->host, &after));
  if (expected == UMI_STATUS_OK) {
    CHECK(revision == before.policy_revision + 1 && after.policy_revision == revision);
    CHECK(after.policy_rules == count);
  } else {
    CHECK(revision == 77 && after.policy_revision == before.policy_revision);
    CHECK(after.policy_rules == before.policy_rules);
  }
  if (strcmp(name, "retired")) {
    UmiAuthorizedRecordRevision saved;
    memset(&saved, 0xa5, sizeof saved);
    CHECK(host_save(f, "Example", 0, &saved) ==
          (permitted ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED));
    if (!permitted)
      CHECK(host_filled(&saved, sizeof saved, 0xa5));
  }
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
