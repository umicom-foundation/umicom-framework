/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workspace_rename/test_preview.c
 *
 * PURPOSE:
 *   Check rename authorization, portable names and registered project boundaries without changing
 * entries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *test = argv[1], *source = "old.c", *name = "new.c";
  UmiStatus expected = UMI_STATUS_OK;
  RenameFixture f = {0};
  CHECK(StartRename(&f) == 0);
  uint64_t revision = f.snapshot.revision;
  UmiWorkspaceRenameResult result;
  int known = 0;
  const char *cases[] = {"basic",           "nested",         "backslash",     "same",
                         "empty-source",    "empty-name",     "parent-source", "parent-name",
                         "absolute-source", "absolute-name",  "new-folder",    "metadata-source",
                         "metadata-name",   "device",         "colon",         "trailing-dot",
                         "trailing-space",  "stale",          "untrusted",     "closed",
                         "project-root",    "project-parent", "arguments",     "capacity",
                         "new-folder-backslash", "nested-new-folder", "fixture-isolation"};
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(cases[i], test) == 0)
      known = 1;
  CHECK(known);
  if (!strcmp(test, "fixture-isolation")) {
    /* Two live fixtures in one case must not adopt or remove one another. */
    RenameFixture second = {0};
    CHECK(PutRename(&f, "retained.txt", "Earlier evidence") == 0);
    CHECK(StartRename(&second) == 0);
    CHECK(strcmp(f.root, second.root) != 0);
    CHECK(StopRename(&second) == 0);
    char retained[UMI_PATH_CAPACITY];
    CHECK(umi_path_join(f.root, "retained.txt", retained, sizeof retained) == UMI_STATUS_OK);
    CHECK(RenameText(retained, "Earlier evidence") == 0);
    return StopRename(&f);
  }
  if (!strcmp(test, "nested"))
    source = "src/old.c";
  if (!strcmp(test, "backslash"))
    source = "src\\old.c";
  if (!strcmp(test, "same")) {
    name = source;
    expected = UMI_STATUS_ALREADY_EXISTS;
  }
  if (!strcmp(test, "empty-source")) {
    source = "";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "empty-name")) {
    name = "";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "parent-source")) {
    source = "../old.c";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "parent-name")) {
    name = "../new.c";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "absolute-source")) {
    source = f.root;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "absolute-name")) {
    name = f.root;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "new-folder")) {
    name = "other/new.c";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "new-folder-backslash")) {
    name = "other\\new.c";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "nested-new-folder")) {
    source = "src/old.c";
    name = "other/new.c";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "metadata-source")) {
    source = ".git/config";
    expected = UMI_STATUS_PERMISSION_DENIED;
  }
  if (!strcmp(test, "metadata-name")) {
    name = ".git";
    expected = UMI_STATUS_PERMISSION_DENIED;
  }
  if (!strcmp(test, "device")) {
    name = "NUL.txt";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "colon")) {
    name = "file:stream";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "trailing-dot")) {
    name = "new.";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "trailing-space")) {
    name = "new ";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(test, "stale")) {
    ++revision;
    expected = UMI_STATUS_BUSY;
  }
  if (!strcmp(test, "untrusted")) {
    CHECK(umi_workspace_graph_set_trusted(f.graph, 0) == UMI_STATUS_OK);
    revision = 0U;
    expected = UMI_STATUS_PERMISSION_DENIED;
  }
  if (!strcmp(test, "closed")) {
    CHECK(umi_workspace_graph_close(f.graph) == UMI_STATUS_OK);
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(test, "project-root") || !strcmp(test, "project-parent")) {
    char project[UMI_PATH_CAPACITY];
    CHECK(umi_path_join(f.root, "src/child", project, sizeof project) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(project) == UMI_STATUS_OK);
    CHECK(umi_workspace_graph_add_project(f.graph, "nested", "Nested", project, UMI_PROJECT_GENERIC,
                                          NULL) == UMI_STATUS_OK);
    source = !strcmp(test, "project-root") ? "src/child" : "src";
    revision = 0U;
    expected = UMI_STATUS_PERMISSION_DENIED;
  }
  char oversized[UMI_PATH_CAPACITY + 1U];
  if (!strcmp(test, "capacity")) {
    memset(oversized, 'a', sizeof oversized - 1U);
    oversized[sizeof oversized - 1U] = '\0';
    source = oversized;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  }
  if (!strcmp(test, "arguments")) {
    CHECK(UmiWorkspaceRenamePreview(NULL, 0U, source, name, &result) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWorkspaceRenamePreview(f.graph, 0U, NULL, name, &result) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWorkspaceRenamePreview(f.graph, 0U, source, NULL, &result) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWorkspaceRenamePreview(f.graph, 0U, source, name, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
  }
  memset(&result, 0x7f, sizeof result);
  CHECK(UmiWorkspaceRenamePreview(f.graph, revision, source, name, &result) == expected);
  CHECK(!result.renamed && !result.directory);
  if (!strcmp(test, "new-folder") || !strcmp(test, "new-folder-backslash") ||
      !strcmp(test, "nested-new-folder")) {
    /* Execution uses the same basename guard and must refuse before native I/O. */
    CHECK(UmiWorkspaceRenameEntry(f.graph, revision, source, name, &result) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!result.renamed && result.source[0] == '\0' && result.destination[0] == '\0');
  }
  if (expected != UMI_STATUS_OK)
    CHECK(result.source[0] == '\0' && result.destination[0] == '\0');
  else {
    CHECK(umi_path_is_within(f.root, result.source) &&
          umi_path_is_within(f.root, result.destination));
    CHECK(!umi_fs_exists(result.destination));
  }
  CHECK(StopRename(&f) == 0);
  return 0;
}
