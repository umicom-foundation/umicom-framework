/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/security/test_identity_boundaries.c
 *
 * PURPOSE:
 *   Reject unterminated identity metadata before any lookup or stored mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reject unterminated identity metadata before any lookup or stored mutation. */
#include "umicom/security/identity.h"
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
  const char *cases[] = {"unterminated-id", "unterminated-display",
                         "max-id",          "max-display",
                         "empty-id",        "empty-display",
                         "lookup-unclosed", "enable-unclosed",
                         "lookup-empty",    "enable-empty",
                         "lookup-missing",  "enable-missing",
                         "enable-revision", "duplicate",
                         "outputs",         "null"};
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
  UmiIdentityRegistry *registry = NULL;
  UmiIdentityRecord in = {.identity_id = "example.author",
                          .display_name = "Example author",
                          .kind = UMI_IDENTITY_HUMAN,
                          .enabled = 1},
                    out, before;
  memset(&out, 0x6a, sizeof out);
  before = out;
  OK(umi_identity_registry_create(&registry));
  if (!strcmp(name, "unterminated-id"))
    memset(in.identity_id, 'x', sizeof in.identity_id);
  if (!strcmp(name, "unterminated-display"))
    memset(in.display_name, 'x', sizeof in.display_name);
  if (!strcmp(name, "empty-id"))
    in.identity_id[0] = 0;
  if (!strcmp(name, "max-id")) {
    memset(in.identity_id, 'x', sizeof in.identity_id - 1);
    in.identity_id[sizeof in.identity_id - 1] = 0;
  }
  if (!strcmp(name, "max-display")) {
    memset(in.display_name, 'x', sizeof in.display_name - 1);
    in.display_name[sizeof in.display_name - 1] = 0;
  }
  if (!strcmp(name, "empty-display"))
    in.display_name[0] = 0;
  if (!strcmp(name, "unterminated-id") || !strcmp(name, "unterminated-display") ||
      !strcmp(name, "empty-id")) {
    CHECK(umi_identity_registry_register(registry, &in) != UMI_STATUS_OK);
    CHECK(!umi_identity_registry_count(registry));
    goto cleanup;
  }
  OK(umi_identity_registry_register(registry, &in));
  UmiIdentityRecord initial;
  OK(umi_identity_registry_get(registry, in.identity_id, &initial));
  if (!strcmp(name, "max-id") || !strcmp(name, "max-display") || !strcmp(name, "empty-display")) {
    CHECK(!strcmp(in.identity_id, initial.identity_id) &&
          !strcmp(in.display_name, initial.display_name));
    goto cleanup;
  }
  if (!strcmp(name, "duplicate")) {
    CHECK(umi_identity_registry_register(registry, &in) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_identity_registry_count(registry) == 1);
    goto cleanup;
  }
  if (!strcmp(name, "enable-revision")) {
    OK(umi_identity_registry_set_enabled(registry, in.identity_id, 0));
    OK(umi_identity_registry_get(registry, in.identity_id, &out));
    CHECK(!out.enabled && out.revision > initial.revision);
    goto cleanup;
  }
  if (!strcmp(name, "lookup-unclosed") || !strcmp(name, "lookup-empty") ||
      !strcmp(name, "lookup-missing")) {
    char unclosed[UMI_IDENTITY_ID_CAPACITY];
    memset(unclosed, 'x', sizeof unclosed);
    const char *id = !strcmp(name, "lookup-unclosed") ? unclosed
                     : !strcmp(name, "lookup-empty")  ? ""
                                                      : "missing";
    CHECK(umi_identity_registry_get(registry, id, &out) != UMI_STATUS_OK);
    CHECK(!memcmp(&out, &before, sizeof out));
    goto cleanup;
  }
  if (!strcmp(name, "enable-unclosed") || !strcmp(name, "enable-empty") ||
      !strcmp(name, "enable-missing")) {
    char unclosed[UMI_IDENTITY_ID_CAPACITY];
    memset(unclosed, 'x', sizeof unclosed);
    const char *id = !strcmp(name, "enable-unclosed") ? unclosed
                     : !strcmp(name, "enable-empty")  ? ""
                                                      : "missing";
    CHECK(umi_identity_registry_set_enabled(registry, id, 0) != UMI_STATUS_OK);
    OK(umi_identity_registry_get(registry, in.identity_id, &out));
    CHECK(!memcmp(&out, &initial, sizeof out));
    goto cleanup;
  }
  if (!strcmp(name, "outputs")) {
    CHECK(umi_identity_registry_at(registry, 1, &out) == UMI_STATUS_NOT_FOUND);
    CHECK(!memcmp(&out, &before, sizeof out));
    goto cleanup;
  }
  CHECK(umi_identity_registry_register(registry, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_identity_registry_get(NULL, in.identity_id, &out) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_identity_registry_get(registry, NULL, &out) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_identity_registry_get(registry, in.identity_id, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_identity_registry_set_enabled(registry, NULL, 0) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_identity_registry_set_enabled(NULL, in.identity_id, 0) == UMI_STATUS_INVALID_ARGUMENT);
cleanup:
  umi_identity_registry_destroy(registry);
  return result;
}
