/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/security/test_session_entropy.c
 *
 * PURPOSE:
 *   This target supplies a deterministic entropy fault double to the production
 *   session translation unit. Production links the operating-system provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * This target supplies a deterministic entropy fault double to the production
 * session translation unit. Production links the operating-system provider. */
#include "umicom/platform/secure_random.h"
#include "umicom/security/session.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
static const char *mode;
static unsigned calls;
UmiStatus UmiSecureRandomBytes(void *output, size_t bytes) {
  ++calls;
  if (!output || bytes != 32)
    return UMI_STATUS_INVALID_ARGUMENT;
  memset(output, 0xa7, bytes);
  if (!strcmp(mode, "unavailable") || (!strcmp(mode, "retry-after-failure") && calls == 1))
    return UMI_STATUS_UNAVAILABLE;
  if (!strcmp(mode, "unsupported"))
    return UMI_STATUS_NOT_IMPLEMENTED;
  unsigned value = calls;
  if (!strcmp(mode, "collision"))
    value = 1;
  if (!strcmp(mode, "collision-retry") && calls == 2)
    value = 1;
  unsigned char *data = output;
  data[0] = (unsigned char)value;
  data[1] = (unsigned char)(value >> 8);
  return UMI_STATUS_OK;
}
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
int main(int argc, char **argv) {
  const char *cases[] = {"unavailable", "unsupported",     "retry-after-failure",
                         "collision",   "collision-retry", "opaque",
                         "capacity",    "prevalidation"};
  if (argc != 2)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i]))
      known = true;
  if (!known)
    return 2;
  int result = 0;
  mode = argv[1];
  UmiSessionRegistry *registry = NULL;
  UmiSessionRecord first, out, before;
  memset(&out, 0x7e, sizeof out);
  before = out;
  OK(umi_session_registry_create(&registry));
  if (!strcmp(mode, "prevalidation")) {
    CHECK(umi_session_registry_issue(registry, "example", UINT64_MAX, 1, &out) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(calls == 0 && !memcmp(&out, &before, sizeof out) &&
          umi_session_registry_count(registry) == 0);
    goto cleanup;
  }
  if (!strcmp(mode, "unavailable") || !strcmp(mode, "unsupported") ||
      !strcmp(mode, "retry-after-failure")) {
    UmiStatus expected =
        !strcmp(mode, "unsupported") ? UMI_STATUS_NOT_IMPLEMENTED : UMI_STATUS_UNAVAILABLE;
    CHECK(umi_session_registry_issue(registry, "example", 100, 100, &out) == expected);
    CHECK(calls == 1 && !memcmp(&out, &before, sizeof out) &&
          umi_session_registry_count(registry) == 0);
    if (!strcmp(mode, "retry-after-failure")) {
      OK(umi_session_registry_issue(registry, "example", 100, 100, &out));
      CHECK(calls == 2 && umi_session_registry_count(registry) == 1);
    }
    goto cleanup;
  }
  OK(umi_session_registry_issue(registry, "example", 100, 100, &first));
  if (!strcmp(mode, "collision")) {
    CHECK(umi_session_registry_issue(registry, "example", 100, 100, &out) ==
          UMI_STATUS_UNAVAILABLE);
    CHECK(calls == 5 && !memcmp(&out, &before, sizeof out) &&
          umi_session_registry_count(registry) == 1);
    goto cleanup;
  }
  if (!strcmp(mode, "collision-retry")) {
    OK(umi_session_registry_issue(registry, "example", 100, 100, &out));
    CHECK(calls == 3 && strcmp(out.session_id, first.session_id) &&
          umi_session_registry_count(registry) == 2);
    goto cleanup;
  }
  if (!strcmp(mode, "opaque")) {
    CHECK(strlen(first.session_id) == 72 && !strncmp(first.session_id, "session-", 8));
    for (size_t i = 8; i < 72; ++i)
      CHECK((first.session_id[i] >= '0' && first.session_id[i] <= '9') ||
            (first.session_id[i] >= 'a' && first.session_id[i] <= 'f'));
    CHECK(strcmp(first.session_id, "session-0000000000000001"));
    goto cleanup;
  }
  for (size_t i = 1; i < UMI_SESSION_REGISTRY_MAX; ++i)
    OK(umi_session_registry_issue(registry, "example", 100, 100, &out));
  out = before;
  unsigned before_calls = calls;
  CHECK(umi_session_registry_issue(registry, "example", 100, 100, &out) ==
        UMI_STATUS_CAPACITY_EXCEEDED);
  CHECK(calls == before_calls && !memcmp(&out, &before, sizeof out) &&
        umi_session_registry_count(registry) == UMI_SESSION_REGISTRY_MAX);
  CHECK(umi_session_registry_cleanup(registry, 200) == UMI_SESSION_REGISTRY_MAX);
  OK(umi_session_registry_issue(registry, "example", 201, 100, &out));
  CHECK(umi_session_registry_count(registry) == 1);
cleanup:
  umi_session_registry_destroy(registry);
  return result;
}
