/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Session clock boundaries and failed outputs are part of the public contract. */
#include "umicom/security/session.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
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
  const char *cases[] = {"basic",         "before-issued",   "rollback",
                         "equal-now",     "expires",         "after-expiry",
                         "revoked",       "overflow",        "max-boundary",
                         "zero-lifetime", "empty-principal", "unclosed-principal",
                         "empty-handle",  "unclosed-handle", "revoke-bounds",
                         "missing",       "cleanup",         "outputs",
                         "null",          "distinct",        "principal-capacity"};
  if (argc != 2)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i]))
      known = true;
  if (!known)
    return 2;
  int result = 0;
  const char *name = argv[1];
  UmiSessionRegistry *registry = NULL;
  UmiSessionRecord first, second, out, before;
  memset(&out, 0x6f, sizeof out);
  before = out;
  OK(umi_session_registry_create(&registry));
  if (!strcmp(name, "overflow") || !strcmp(name, "zero-lifetime") ||
      !strcmp(name, "empty-principal") || !strcmp(name, "unclosed-principal")) {
    char unclosed[UMI_IDENTITY_ID_CAPACITY];
    memset(unclosed, 'x', sizeof unclosed);
    const char *principal = !strcmp(name, "empty-principal")      ? ""
                            : !strcmp(name, "unclosed-principal") ? unclosed
                                                                  : "example.author";
    uint64_t now = !strcmp(name, "overflow") ? UINT64_MAX : 100,
             lifetime = !strcmp(name, "zero-lifetime") ? 0 : 10;
    CHECK(umi_session_registry_issue(registry, principal, now, lifetime, &out) != UMI_STATUS_OK);
    CHECK(!memcmp(&out, &before, sizeof out) && umi_session_registry_count(registry) == 0);
    goto cleanup;
  }
  if (!strcmp(name, "principal-capacity")) {
    char principal[UMI_IDENTITY_ID_CAPACITY];
    memset(principal, 'x', sizeof principal - 1);
    principal[sizeof principal - 1] = 0;
    OK(umi_session_registry_issue(registry, principal, 100, 10, &first));
    CHECK(!strcmp(first.principal_id, principal));
    goto cleanup;
  }
  if (!strcmp(name, "max-boundary")) {
    OK(umi_session_registry_issue(registry, "example.author", UINT64_MAX - 1, 1, &first));
    CHECK(first.expires_at_ns == UINT64_MAX);
    OK(umi_session_registry_validate(registry, first.session_id, UINT64_MAX - 1, &out));
    CHECK(umi_session_registry_validate(registry, first.session_id, UINT64_MAX, &out) ==
          UMI_STATUS_PERMISSION_DENIED);
    goto cleanup;
  }
  OK(umi_session_registry_issue(registry, "example.author", 100, 100, &first));
  if (!strcmp(name, "basic")) {
    OK(umi_session_registry_validate(registry, first.session_id, 150, &out));
    CHECK(out.last_seen_ns == 150 && out.expires_at_ns == 200 &&
          !strcmp(out.principal_id, "example.author"));
    goto cleanup;
  }
  if (!strcmp(name, "distinct")) {
    OK(umi_session_registry_issue(registry, "example.author", 100, 100, &second));
    CHECK(strcmp(first.session_id, second.session_id));
    CHECK(umi_session_registry_count(registry) == 2);
    goto cleanup;
  }
  if (!strcmp(name, "before-issued")) {
    CHECK(umi_session_registry_validate(registry, first.session_id, 99, &out) ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(!memcmp(&out, &before, sizeof out));
    OK(umi_session_registry_at(registry, 0, &second));
    CHECK(second.active && second.last_seen_ns == 100);
    OK(umi_session_registry_validate(registry, first.session_id, 100, &out));
    goto cleanup;
  }
  if (!strcmp(name, "rollback")) {
    OK(umi_session_registry_validate(registry, first.session_id, 150, &second));
    CHECK(umi_session_registry_validate(registry, first.session_id, 149, &out) ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(!memcmp(&out, &before, sizeof out));
    OK(umi_session_registry_at(registry, 0, &second));
    CHECK(second.last_seen_ns == 150 && second.active);
    OK(umi_session_registry_validate(registry, first.session_id, 151, &out));
    goto cleanup;
  }
  if (!strcmp(name, "equal-now")) {
    OK(umi_session_registry_validate(registry, first.session_id, 100, &out));
    OK(umi_session_registry_validate(registry, first.session_id, 100, &out));
    CHECK(out.last_seen_ns == 100);
    goto cleanup;
  }
  if (!strcmp(name, "expires") || !strcmp(name, "after-expiry")) {
    CHECK(umi_session_registry_validate(registry, first.session_id, 200, &out) ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(!memcmp(&out, &before, sizeof out));
    OK(umi_session_registry_at(registry, 0, &second));
    CHECK(!second.active);
    if (!strcmp(name, "after-expiry"))
      CHECK(umi_session_registry_validate(registry, first.session_id, 150, &out) ==
            UMI_STATUS_PERMISSION_DENIED);
    goto cleanup;
  }
  if (!strcmp(name, "revoked")) {
    OK(umi_session_registry_revoke(registry, first.session_id));
    CHECK(umi_session_registry_validate(registry, first.session_id, 150, &out) ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(!memcmp(&out, &before, sizeof out));
    goto cleanup;
  }
  if (!strcmp(name, "empty-handle") || !strcmp(name, "unclosed-handle") ||
      !strcmp(name, "revoke-bounds")) {
    char handle[UMI_SESSION_ID_CAPACITY];
    memset(handle, 'x', sizeof handle);
    const char *value = !strcmp(name, "empty-handle") ? "" : handle;
    CHECK(umi_session_registry_validate(registry, value, 150, &out) != UMI_STATUS_OK);
    CHECK(!memcmp(&out, &before, sizeof out));
    if (!strcmp(name, "revoke-bounds"))
      CHECK(umi_session_registry_revoke(registry, handle) != UMI_STATUS_OK);
    OK(umi_session_registry_validate(registry, first.session_id, 150, &out));
    goto cleanup;
  }
  if (!strcmp(name, "missing")) {
    CHECK(umi_session_registry_validate(registry, "missing", 150, &out) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_session_registry_revoke(registry, "missing") == UMI_STATUS_NOT_FOUND);
    CHECK(!memcmp(&out, &before, sizeof out));
    goto cleanup;
  }
  if (!strcmp(name, "cleanup")) {
    OK(umi_session_registry_issue(registry, "example.service", 100, 200, &second));
    CHECK(umi_session_registry_cleanup(registry, 200) == 1);
    CHECK(umi_session_registry_count(registry) == 1);
    OK(umi_session_registry_validate(registry, second.session_id, 201, &out));
    goto cleanup;
  }
  if (!strcmp(name, "outputs")) {
    CHECK(umi_session_registry_at(registry, 1, &out) == UMI_STATUS_NOT_FOUND);
    CHECK(!memcmp(&out, &before, sizeof out));
    CHECK(umi_session_registry_issue(registry, "example", UINT64_MAX, 1, &out) != UMI_STATUS_OK);
    CHECK(!memcmp(&out, &before, sizeof out));
    goto cleanup;
  }
  CHECK(umi_session_registry_issue(NULL, "example", 1, 1, &out) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_session_registry_issue(registry, NULL, 1, 1, &out) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_session_registry_issue(registry, "example", 1, 1, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_session_registry_validate(NULL, first.session_id, 150, &out) ==
        UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_session_registry_validate(registry, NULL, 150, &out) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_session_registry_validate(registry, first.session_id, 150, NULL) ==
        UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_session_registry_revoke(NULL, first.session_id) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_session_registry_revoke(registry, NULL) == UMI_STATUS_INVALID_ARGUMENT);
cleanup:
  umi_session_registry_destroy(registry);
  return result;
}
