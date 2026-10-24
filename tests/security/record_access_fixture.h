/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Fictional identities exercise the same authority against both storage engines. */
#ifndef UMICOM_RECORD_ACCESS_FIXTURE_H
#define UMICOM_RECORD_ACCESS_FIXTURE_H
#include "umicom/data/authorized_records.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
typedef struct AccessFixture {
  UmiIdentityRegistry *identities;
  UmiSessionRegistry *sessions;
  UmiPolicyEngine *policy;
  UmiRecordAccessAuthority *authority;
  UmiDataServer *server;
  UmiAuthorizedRecords *records;
  UmiSessionRecord session;
  UmiRecordAccessRequest request;
} AccessFixture;
static inline UmiStatus access_rule(AccessFixture *f, const char *principal, const char *capability,
                                    const char *resource, UmiPolicyEffect effect) {
  UmiPolicyRule rule = {principal, capability, resource, effect};
  return umi_policy_engine_add(f->policy, &rule);
}
static inline UmiStatus access_allow(AccessFixture *f) {
  UmiStatus s = access_rule(f, "example.editor", "record.*", "example.*", UMI_POLICY_ALLOW);
  if (s == UMI_STATUS_OK)
    s = access_rule(f, "example.editor", "record.purpose", "purpose/review/record/example.*",
                    UMI_POLICY_ALLOW);
  return s;
}
static inline UmiStatus access_new(AccessFixture *f, const char *backend) {
  memset(f, 0, sizeof *f);
  UmiStatus s = umi_identity_registry_create(&f->identities);
  if (s == UMI_STATUS_OK)
    s = umi_session_registry_create(&f->sessions);
  if (s == UMI_STATUS_OK)
    s = umi_policy_engine_create(&f->policy);
  UmiIdentityRecord identity = {.identity_id = "example.editor",
                                .display_name = "Fictional editor",
                                .kind = UMI_IDENTITY_HUMAN,
                                .enabled = 1};
  if (s == UMI_STATUS_OK)
    s = umi_identity_registry_register(f->identities, &identity);
  if (s == UMI_STATUS_OK)
    s = umi_session_registry_issue(f->sessions, identity.identity_id, 100, 1000, &f->session);
  if (s == UMI_STATUS_OK)
    s = umi_record_access_authority_create(f->identities, f->sessions, f->policy,
                                           "Example record host", "example.", &f->authority);
  if (s == UMI_STATUS_OK)
    s = !strcmp(backend, "sqlite") ? umi_data_server_create_sqlite(":memory:", &f->server)
                                   : umi_data_server_create_memory(&f->server);
  if (s == UMI_STATUS_OK)
    s = umi_authorized_records_create(f->server, f->authority, &f->records);
  if (s == UMI_STATUS_OK) {
    strcpy(f->request.session_id, f->session.session_id);
    strcpy(f->request.entity_id, "example.record");
    strcpy(f->request.purpose, "review");
    f->request.operation = UMI_RECORD_ACCESS_READ_HEAD;
    f->request.now_nanoseconds = 101;
  }
  return s;
}
static inline void access_free(AccessFixture *f) {
  umi_authorized_records_destroy(f->records);
  umi_data_server_destroy(f->server);
  umi_record_access_authority_destroy(f->authority);
  umi_policy_engine_destroy(f->policy);
  umi_session_registry_destroy(f->sessions);
  umi_identity_registry_destroy(f->identities);
}
static inline UmiStatus access_seed(AccessFixture *f, const char *entity, const char *text,
                                    uint64_t expected, UmiRecordRevision *out) {
  UmiRecordAmendment metadata = {.actor = "example.seed",
                                 .source = "Fictional fixture",
                                 .reason = "Create synthetic fixture",
                                 .recorded_at_nanoseconds = 100};
  (void)snprintf(metadata.entity_id, sizeof metadata.entity_id, "%s", entity);
  return umi_record_document_append(f->server, &metadata, text, strlen(text), expected, out);
}
static inline bool access_filled(const void *buffer, size_t length, unsigned char value) {
  const unsigned char *bytes = buffer;
  for (size_t i = 0; i < length; ++i)
    if (bytes[i] != value)
      return false;
  return true;
}
#endif
