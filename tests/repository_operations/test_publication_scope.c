/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_publication_scope.c
 *
 * PURPOSE:
 *   Verify explicit publication scope and unchanged registered-child traversal with controlled
 * filesystem and Git discovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
/* The earlier fixture assumed forward-slash output and named its global mode,
 * which shadows the Git index mode inside the included discovery source. It
 * remains here for review. The active fixture below preserves every scenario
 * while using Framework path identity and a fixture-specific selector name. */
#if 0
static const char *mode;
static unsigned git_calls, excluded_git_calls;
static UmiStatus Safe(UmiPublicationState *state, const char *path) {
  (void)state;
  (void)path;
  return UMI_STATUS_OK;
}
static int Exists(const char *path) {
  if (!strcmp(path, "C:/scope/.git") || !strcmp(path, "/scope/.git"))
    return !strcmp(mode, "root-repository");
  return 1;
}
static UmiStatus Git(UmiPublicationState *state, size_t index, const char *const *args,
                     size_t count, int mutation, UmiPublicationCapture *capture) {
  ++git_calls;
  const char *p = state->report->entries[index].path;
  if (!strcmp(p, "C:/scope/external") || !strcmp(p, "/scope/external"))
    ++excluded_git_calls;
  if (mutation || count != 3U || strcmp(args[0], "ls-files"))
    return UMI_STATUS_PERMISSION_DENIED;
  const char *record = "";
  size_t length = 0U;
  static const char child[] = "160000 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 0\texternal";
  if (!strcmp(mode, "registered-child") && strstr(p, "/Applications") &&
      !strstr(p, "/Applications/external")) {
    record = child;
    length = sizeof child;
  }
  capture->bytes = malloc(length + 1U);
  if (!capture->bytes)
    return UMI_STATUS_OUT_OF_MEMORY;
  memcpy(capture->bytes, record, length);
  capture->bytes[length] = 0;
  capture->size = length;
  capture->exit_code = 0;
  return UMI_STATUS_OK;
}
static UmiStatus Walk(const char *root, const UmiDirectoryWalkOptions *options,
                      UmiDirectoryVisitor visit, void *context,
                      const UmiCancellationToken *cancel) {
  (void)cancel;
  if (options->recursive || options->follow_symbolic_links)
    return UMI_STATUS_INVALID_STATE;
  const char *names[] = {"Applications", "external"};
  for (size_t i = 0U; i < 2U; ++i) {
    UmiFileInfo info = {0};
    info.kind = UMI_FILE_KIND_DIRECTORY;
    snprintf(info.name, sizeof info.name, "%s", names[i]);
    snprintf(info.path, sizeof info.path, "%s/%s", root, names[i]);
    UmiStatus status = visit(&info, context);
    if (status != UMI_STATUS_OK)
      return status;
  }
  return UMI_STATUS_OK;
}
#define umi_publication_safe_path Safe
#define umi_fs_exists Exists
#define umi_publication_git Git
#define UmiDirectoryWalkCancellable Walk
#define umi_publication_discover ControlledDiscover
#include "../../src/repository/operations/publication_discovery.c"
#undef umi_publication_safe_path
#undef umi_fs_exists
#undef umi_publication_git
#undef UmiDirectoryWalkCancellable
#undef umi_publication_discover
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  mode = argv[1];
  const char *names[17] = {"external"};
  size_t count = 1U;
  char long_name[65];
  memset(long_name, 'x', 64U);
  long_name[64] = 0;
  char names_storage[5][64];
  UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
  if (!strcmp(mode, "empty")) {
    count = 0U;
    expected = UMI_STATUS_OK;
  } else if (!strcmp(mode, "valid"))
    expected = UMI_STATUS_OK;
  else if (!strcmp(mode, "duplicate")) {
    names[1] = names[0];
    count = 2U;
  } else if (!strcmp(mode, "null-vector")) {
    CHECK(umi_repository_publication_exclusions_validate(NULL, 1U) == expected);
    return 0;
  } else if (!strcmp(mode, "null-name"))
    names[0] = NULL;
  else if (!strcmp(mode, "empty-name"))
    names[0] = "";
  else if (!strcmp(mode, "absolute"))
    names[0] = "C:/external";
  else if (!strcmp(mode, "nested"))
    names[0] = "Applications/external";
  else if (!strcmp(mode, "parent"))
    names[0] = "../external";
  else if (!strcmp(mode, "option-name"))
    names[0] = "--apply";
  else if (!strcmp(mode, "trailing-dot"))
    names[0] = "external.";
  else if (!strcmp(mode, "long-name"))
    names[0] = long_name;
  else if (!strcmp(mode, "excess-count"))
    count = 17U;
  else if (!strcmp(mode, "total-limit")) {
    count = 5U;
    for (size_t i = 0U; i < count; ++i) {
      memset(names_storage[i], (int)('a' + i), 63U);
      names_storage[i][63] = 0;
      names[i] = names_storage[i];
    }
  } else {
    UmiPublicationState *state = calloc(1U, sizeof *state);
    UmiRepositoryPublicationReport *report = calloc(1U, sizeof *report);
    CHECK(state && report);
    UmiRepositoryPublicationRequest request;
#ifdef _WIN32
    const char *root = "C:/scope";
#else
    const char *root = "/scope";
#endif
    umi_repository_publication_request_init(&request, root);
    request.refresh = 0;
    state->request = &request;
    state->report = report;
    snprintf(state->root, sizeof state->root, "%s", root);
    expected = UMI_STATUS_OK;
    if (!strcmp(mode, "size-contract")) {
      UmiRepositoryPublicationOptions options = {0};
      report->count = 71U;
      CHECK(umi_repository_publication_run_scoped_sized(
                NULL, NULL, NULL, &options, sizeof options - 1U, names, 1U, report,
                sizeof *report) == UMI_STATUS_INVALID_ARGUMENT);
      CHECK(report->count == 71U);
      free(state);
      free(report);
      return 0;
    } else if (!strcmp(mode, "unmatched")) {
      names[0] = "absent";
      expected = UMI_STATUS_NOT_FOUND;
    } else if (!strcmp(mode, "name-prefix")) {
      names[0] = "ext";
      expected = UMI_STATUS_NOT_FOUND;
    } else if (!strcmp(mode, "all-excluded")) {
      names[1] = "Applications";
      count = 2U;
      expected = UMI_STATUS_NOT_FOUND;
    } else if (!strcmp(mode, "root-repository"))
      expected = UMI_STATUS_INVALID_ARGUMENT;
    else if (strcmp(mode, "skip-unrelated") && strcmp(mode, "registered-child") &&
             strcmp(mode, "repeat-discovery"))
      return 2;
    state->excluded_names = names;
    state->excluded_count = count;
    CHECK(ControlledDiscover(state) == expected);
    if (!strcmp(mode, "unmatched") || !strcmp(mode, "name-prefix"))
      CHECK(excluded_git_calls == 1U);
    else
      CHECK(excluded_git_calls == 0U);
    if (expected == UMI_STATUS_OK) {
      CHECK(report->count == (!strcmp(mode, "registered-child") ? 2U : 1U));
      if (!strcmp(mode, "registered-child"))
        CHECK(state->edge_count == 1U && strstr(report->entries[1].path, "/Applications/external"));
      if (!strcmp(mode, "repeat-discovery")) {
        report->count = 0U;
        state->excluded_seen = 0xffffffffU;
        CHECK(ControlledDiscover(state) == UMI_STATUS_OK && state->excluded_seen == 1U);
      }
    }
    free(state);
    free(report);
    return 0;
  }
  CHECK(umi_repository_publication_exclusions_validate(names, count) == expected);
  CHECK(git_calls == 0U);
  return 0;
}
#endif

static const char *scope_case;
static unsigned git_calls, excluded_git_calls;

/* Discovery normalises paths for the host. Compare through the same lexical
 * Framework service so Windows separators do not change the controlled tree. */
static int ScopePathEquals(const char *actual, const char *leaf) {
  char expected[UMI_PATH_CAPACITY];
#ifdef _WIN32
  const char *root = "C:/scope";
#else
  const char *root = "/scope";
#endif
  return umi_path_join(root, leaf, expected, sizeof expected) == UMI_STATUS_OK &&
         umi_path_equal(actual, expected);
}
static UmiStatus Safe(UmiPublicationState *state, const char *path) {
  (void)state;
  (void)path;
  return UMI_STATUS_OK;
}
static int Exists(const char *path) {
  if (ScopePathEquals(path, ".git"))
    return !strcmp(scope_case, "root-repository");
  return 1;
}
static UmiStatus Git(UmiPublicationState *state, size_t index, const char *const *args,
                     size_t count, int mutation, UmiPublicationCapture *capture) {
  ++git_calls;
  const char *p = state->report->entries[index].path;
  if (ScopePathEquals(p, "external"))
    ++excluded_git_calls;
  if (mutation || count != 3U || strcmp(args[0], "ls-files"))
    return UMI_STATUS_PERMISSION_DENIED;
  const char *record = "";
  size_t length = 0U;
  static const char child[] = "160000 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 0\texternal";
  if (!strcmp(scope_case, "registered-child") && ScopePathEquals(p, "Applications")) {
    record = child;
    length = sizeof child;
  }
  capture->bytes = malloc(length + 1U);
  if (!capture->bytes)
    return UMI_STATUS_OUT_OF_MEMORY;
  memcpy(capture->bytes, record, length);
  capture->bytes[length] = 0;
  capture->size = length;
  capture->exit_code = 0;
  return UMI_STATUS_OK;
}
static UmiStatus Walk(const char *root, const UmiDirectoryWalkOptions *options,
                      UmiDirectoryVisitor visit, void *context,
                      const UmiCancellationToken *cancel) {
  (void)cancel;
  if (options->recursive || options->follow_symbolic_links)
    return UMI_STATUS_INVALID_STATE;
  const char *names[] = {"Applications", "external"};
  for (size_t i = 0U; i < 2U; ++i) {
    UmiFileInfo info = {0};
    info.kind = UMI_FILE_KIND_DIRECTORY;
    snprintf(info.name, sizeof info.name, "%s", names[i]);
    snprintf(info.path, sizeof info.path, "%s/%s", root, names[i]);
    UmiStatus status = visit(&info, context);
    if (status != UMI_STATUS_OK)
      return status;
  }
  return UMI_STATUS_OK;
}
#define umi_publication_safe_path Safe
#define umi_fs_exists Exists
#define umi_publication_git Git
#define UmiDirectoryWalkCancellable Walk
#define umi_publication_discover ControlledDiscover
#include "../../src/repository/operations/publication_discovery.c"
#undef umi_publication_safe_path
#undef umi_fs_exists
#undef umi_publication_git
#undef UmiDirectoryWalkCancellable
#undef umi_publication_discover
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  scope_case = argv[1];
  const char *names[17] = {"external"};
  size_t count = 1U;
  char long_name[65];
  memset(long_name, 'x', 64U);
  long_name[64] = 0;
  char names_storage[5][64];
  UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
  if (!strcmp(scope_case, "empty")) {
    count = 0U;
    expected = UMI_STATUS_OK;
  } else if (!strcmp(scope_case, "valid"))
    expected = UMI_STATUS_OK;
  else if (!strcmp(scope_case, "duplicate")) {
    names[1] = names[0];
    count = 2U;
  } else if (!strcmp(scope_case, "null-vector")) {
    CHECK(umi_repository_publication_exclusions_validate(NULL, 1U) == expected);
    return 0;
  } else if (!strcmp(scope_case, "null-name"))
    names[0] = NULL;
  else if (!strcmp(scope_case, "empty-name"))
    names[0] = "";
  else if (!strcmp(scope_case, "absolute"))
    names[0] = "C:/external";
  else if (!strcmp(scope_case, "nested"))
    names[0] = "Applications/external";
  else if (!strcmp(scope_case, "parent"))
    names[0] = "../external";
  else if (!strcmp(scope_case, "option-name"))
    names[0] = "--apply";
  else if (!strcmp(scope_case, "trailing-dot"))
    names[0] = "external.";
  else if (!strcmp(scope_case, "long-name"))
    names[0] = long_name;
  else if (!strcmp(scope_case, "excess-count"))
    count = 17U;
  else if (!strcmp(scope_case, "total-limit")) {
    count = 5U;
    for (size_t i = 0U; i < count; ++i) {
      memset(names_storage[i], (int)('a' + i), 63U);
      names_storage[i][63] = 0;
      names[i] = names_storage[i];
    }
  } else {
    UmiPublicationState *state = calloc(1U, sizeof *state);
    UmiRepositoryPublicationReport *report = calloc(1U, sizeof *report);
    CHECK(state && report);
    UmiRepositoryPublicationRequest request;
#ifdef _WIN32
    const char *root = "C:/scope";
#else
    const char *root = "/scope";
#endif
    umi_repository_publication_request_init(&request, root);
    request.refresh = 0;
    state->request = &request;
    state->report = report;
    snprintf(state->root, sizeof state->root, "%s", root);
    expected = UMI_STATUS_OK;
    if (!strcmp(scope_case, "size-contract")) {
      UmiRepositoryPublicationOptions options = {0};
      report->count = 71U;
      CHECK(umi_repository_publication_run_scoped_sized(
                NULL, NULL, NULL, &options, sizeof options - 1U, names, 1U, report,
                sizeof *report) == UMI_STATUS_INVALID_ARGUMENT);
      CHECK(report->count == 71U);
      free(state);
      free(report);
      return 0;
    } else if (!strcmp(scope_case, "unmatched")) {
      names[0] = "absent";
      expected = UMI_STATUS_NOT_FOUND;
    } else if (!strcmp(scope_case, "name-prefix")) {
      names[0] = "ext";
      expected = UMI_STATUS_NOT_FOUND;
    } else if (!strcmp(scope_case, "all-excluded")) {
      names[1] = "Applications";
      count = 2U;
      expected = UMI_STATUS_NOT_FOUND;
    } else if (!strcmp(scope_case, "root-repository"))
      expected = UMI_STATUS_INVALID_ARGUMENT;
    else if (strcmp(scope_case, "skip-unrelated") && strcmp(scope_case, "registered-child") &&
             strcmp(scope_case, "repeat-discovery"))
      return 2;
    state->excluded_names = names;
    state->excluded_count = count;
    CHECK(ControlledDiscover(state) == expected);
    if (!strcmp(scope_case, "unmatched") || !strcmp(scope_case, "name-prefix"))
      CHECK(excluded_git_calls == 1U);
    else
      CHECK(excluded_git_calls == 0U);
    if (expected == UMI_STATUS_OK) {
      CHECK(report->count == (!strcmp(scope_case, "registered-child") ? 2U : 1U));
      if (!strcmp(scope_case, "registered-child"))
        CHECK(state->edge_count == 1U && ScopePathEquals(report->entries[1].path, "Applications/external"));
      if (!strcmp(scope_case, "repeat-discovery")) {
        report->count = 0U;
        state->excluded_seen = 0xffffffffU;
        CHECK(ControlledDiscover(state) == UMI_STATUS_OK && state->excluded_seen == 1U);
      }
    }
    free(state);
    free(report);
    return 0;
  }
  CHECK(umi_repository_publication_exclusions_validate(names, count) == expected);
  CHECK(git_calls == 0U);
  return 0;
}
