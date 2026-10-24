/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Owned-host tests expose no underlying Data Server or mutable security handles. */
#ifndef UMICOM_RECORD_HOST_FIXTURE_H
#define UMICOM_RECORD_HOST_FIXTURE_H
#include "umicom/data/record_host.h"
#include "umicom/platform/threading.h"
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
typedef struct HostFixture {
  UmiRecordHost *host;
  UmiRecordAccessRequest request;
  UmiSessionRecord session;
} HostFixture;
static inline UmiStatus host_policy(UmiRecordHost *host, UmiPolicyEffect effect,
                                    uint64_t *revision) {
  UmiPolicyRule rules[] = {
      {"example.editor", "record.*", "example.*", effect},
      {"example.editor", "record.purpose", "purpose/review/record/example.*", effect}};
  return umi_record_host_replace_policy(host, rules, 2, revision);
}
static inline UmiStatus host_new(HostFixture *f, const char *backend, bool allow) {
  memset(f, 0, sizeof *f);
  UmiRecordHostOptions options = {.backend = !strcmp(backend, "sqlite") ? UMI_RECORD_HOST_SQLITE
                                                                        : UMI_RECORD_HOST_MEMORY,
                                  .database_path = !strcmp(backend, "sqlite") ? ":memory:" : NULL,
                                  .source = "Fictional record host",
                                  .entity_prefix = "example."};
  UmiStatus s = umi_record_host_create(&options, &f->host);
  UmiIdentityRecord identity = {.identity_id = "example.editor",
                                .display_name = "Fictional editor",
                                .kind = UMI_IDENTITY_HUMAN,
                                .enabled = 1};
  if (s == UMI_STATUS_OK)
    s = umi_record_host_register_identity(f->host, &identity);
  if (s == UMI_STATUS_OK)
    s = umi_record_host_issue_session(f->host, identity.identity_id, 100, 1000, &f->session);
  uint64_t revision;
  if (s == UMI_STATUS_OK && allow)
    s = host_policy(f->host, UMI_POLICY_ALLOW, &revision);
  if (s == UMI_STATUS_OK) {
    strcpy(f->request.session_id, f->session.session_id);
    strcpy(f->request.entity_id, "example.record");
    strcpy(f->request.purpose, "review");
    f->request.now_nanoseconds = 101;
    f->request.operation = UMI_RECORD_ACCESS_CREATE;
  }
  return s;
}
static inline UmiStatus host_save(HostFixture *f, const char *text, uint64_t expected,
                                  UmiAuthorizedRecordRevision *out) {
  f->request.operation = expected ? UMI_RECORD_ACCESS_AMEND : UMI_RECORD_ACCESS_CREATE;
  return umi_record_host_append(f->host, &f->request, "Review fictional record", 100, text,
                                strlen(text), expected, out);
}
static inline bool host_filled(const void *data, size_t size, unsigned char byte) {
  const unsigned char *p = data;
  for (size_t i = 0; i < size; ++i)
    if (p[i] != byte)
      return false;
  return true;
}
#endif
