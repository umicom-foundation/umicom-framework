/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/security/test_record_access_request.c
 *
 * PURPOSE:
 *   Reject ambiguous resources before session lookup or policy evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reject ambiguous resources before session lookup or policy evaluation. */
#include "record_access_fixture.h"
static int exercise(AccessFixture *f, const char *name) {
  UmiRecordAccessRequest r = f->request;
  UmiRecordAccessReceipt receipt;
  memset(&receipt, 0xa5, sizeof receipt);
  UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
  if (!strcmp(name, "valid"))
    expected = UMI_STATUS_OK;
  else if (!strcmp(name, "null")) {
    CHECK(umi_record_access_request_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_access_authorize(NULL, &r, &receipt) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_access_authorize(f->authority, &r, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  } else if (!strcmp(name, "operation-zero"))
    r.operation = 0;
  else if (!strcmp(name, "operation-unknown"))
    r.operation = 99;
  else if (!strcmp(name, "time-zero"))
    r.now_nanoseconds = 0;
  else if (!strcmp(name, "session-empty"))
    r.session_id[0] = 0;
  else if (!strcmp(name, "session-unclosed"))
    memset(r.session_id, 'x', sizeof r.session_id);
  else if (!strcmp(name, "session-control"))
    r.session_id[0] = '\n';
  else if (!strcmp(name, "entity-empty"))
    r.entity_id[0] = 0;
  else if (!strcmp(name, "entity-unclosed"))
    memset(r.entity_id, 'e', sizeof r.entity_id);
  else if (!strcmp(name, "entity-wildcard"))
    strcpy(r.entity_id, "example.*");
  else if (!strcmp(name, "entity-path"))
    strcpy(r.entity_id, "example.record/other");
  else if (!strcmp(name, "entity-unicode"))
    strcpy(r.entity_id, "example.\xc3\xa9");
  else if (!strcmp(name, "purpose-empty"))
    r.purpose[0] = 0;
  else if (!strcmp(name, "purpose-unclosed"))
    memset(r.purpose, 'p', sizeof r.purpose);
  else if (!strcmp(name, "purpose-wildcard"))
    strcpy(r.purpose, "*");
  else if (!strcmp(name, "purpose-path"))
    strcpy(r.purpose, "review/record/other");
  else if (!strcmp(name, "namespace")) {
    strcpy(r.entity_id, "other.record");
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "namespace-boundary")) {
    strcpy(r.entity_id, "examplex.record");
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "namespace-root")) {
    strcpy(r.entity_id, "example.");
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (!strcmp(name, "maximum-identifiers")) {
    memset(r.entity_id + 8, 'e', sizeof r.entity_id - 9);
    r.entity_id[63] = 0;
    memset(r.purpose, 'p', sizeof r.purpose - 1);
    r.purpose[63] = 0;
    OK(access_rule(f, "example.editor", "record.purpose", "purpose/*", UMI_POLICY_ALLOW));
    expected = UMI_STATUS_OK;
  } else if (!strcmp(name, "factory-prefix")) {
    UmiRecordAccessAuthority *result = f->authority;
    const char *bad[] = {"", "example", "example/*", "example/\n"};
    for (size_t i = 0; i < sizeof bad / sizeof bad[0]; ++i) {
      CHECK(umi_record_access_authority_create(f->identities, f->sessions, f->policy, "Source",
                                               bad[i], &result) == UMI_STATUS_INVALID_ARGUMENT);
      CHECK(result == f->authority);
    }
    return 0;
  } else if (!strcmp(name, "factory-source")) {
    UmiRecordAccessAuthority *result = f->authority;
    char source[160];
    memset(source, 'x', sizeof source);
    const char *bad[] = {"", "Source\n", source};
    for (size_t i = 0; i < sizeof bad / sizeof bad[0]; ++i) {
      CHECK(umi_record_access_authority_create(f->identities, f->sessions, f->policy, bad[i],
                                               "example.", &result) == UMI_STATUS_INVALID_ARGUMENT);
      CHECK(result == f->authority);
    }
    return 0;
  } else if (!strcmp(name, "factory-copy")) {
    char source[] = "Copied source", prefix[] = "example.";
    UmiRecordAccessAuthority *authority = NULL;
    OK(umi_record_access_authority_create(f->identities, f->sessions, f->policy, source, prefix,
                                          &authority));
    source[0] = 'X';
    prefix[0] = 'X';
    OK(umi_record_access_authorize(authority, &r, &receipt));
    CHECK(!strcmp(receipt.source, "Copied source"));
    umi_record_access_authority_destroy(authority);
    return 0;
  } else
    return 2;
  CHECK(umi_record_access_authorize(f->authority, &r, &receipt) == expected);
  if (expected != UMI_STATUS_OK)
    CHECK(access_filled(&receipt, sizeof receipt, 0xa5));
  else {
    CHECK(!strcmp(receipt.entity_id, r.entity_id));
    CHECK(!strcmp(receipt.purpose, r.purpose));
    CHECK(receipt.checked_at_nanoseconds == r.now_nanoseconds);
  }
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  AccessFixture f;
  UmiStatus s = access_new(&f, "memory");
  if (s == UMI_STATUS_OK)
    s = access_allow(&f);
  int result = s == UMI_STATUS_OK ? exercise(&f, argv[1]) : 1;
  access_free(&f);
  return result;
}
