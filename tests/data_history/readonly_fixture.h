/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Unique, retained SQLite fixtures keep parallel cases independent. */
#ifndef UMICOM_TEST_READONLY_FIXTURE_H
#define UMICOM_TEST_READONLY_FIXTURE_H
#include "umicom/data/data_server.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static inline bool ro_case(const char *name, const char *const *cases, size_t count) {
  for (size_t i = 0; i < count; ++i)
    if (strcmp(name, cases[i]) == 0)
      return true;
  return false;
}
/* An exclusive empty file reserves the generated name. Tests intentionally
 * retain all fixtures under the build tree for inspection after a failure. */
static inline bool ro_path(const char *directory, const char *name, char out[1024], bool reserve) {
  struct timespec now;
  if (timespec_get(&now, TIME_UTC) != TIME_UTC)
    return false;
  for (unsigned attempt = 0; attempt < 100; ++attempt) {
    int n = snprintf(out, 1024, "%s/readonly-%s-%lld-%ld-%u.sqlite", directory, name,
                     (long long)now.tv_sec, now.tv_nsec, attempt);
    if (n < 0 || n >= 1024)
      return false;
    FILE *file = fopen(out, "wbx");
    if (file) {
      if (fclose(file) != 0)
        return false;
      return reserve;
    }
    if (!reserve) { /* Missing-file tests pass a name beneath a unique reserved sibling. */
      continue;
    }
  }
  return false;
}
static inline UmiStatus ro_count(const char *key, const char *value, void *context) {
  if (!key || !value)
    return UMI_STATUS_INVALID_ARGUMENT;
  ++*(size_t *)context;
  return UMI_STATUS_OK;
}
#endif
