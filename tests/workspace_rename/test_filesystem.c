/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workspace_rename/test_filesystem.c
 *
 * PURPOSE:
 *   Verify disk content preservation, directory moves and destination refusal through native
 * rename.
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
  const char *cases[] = {
      "file",           "nested",         "directory", "existing-file", "existing-directory",
      "missing-source", "missing-parent", "unicode",   "unchanged",     "arguments",
      "stale",          "untrusted"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(cases[i], test))
      known = 1;
  CHECK(known);
  RenameFixture f = {0};
  CHECK(StartRename(&f) == 0);
  char from[UMI_PATH_CAPACITY], to[UMI_PATH_CAPACITY], folder[UMI_PATH_CAPACITY];
  UmiStatus expected = UMI_STATUS_OK;
  uint64_t revision = f.snapshot.revision;
  if (!strcmp(test, "nested") || !strcmp(test, "directory")) {
    CHECK(umi_path_join(f.root, "src", folder, sizeof folder) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(folder) == UMI_STATUS_OK);
    CHECK(PutRename(&f, "src/old.c", "keep source bytes\n") == 0);
    source = !strcmp(test, "directory") ? "src" : "src/old.c";
    name = !strcmp(test, "directory") ? "renamed" : "new.c";
  } else {
    if (!strcmp(test, "unicode")) {
      source = "caf\xc3\xa9.c";
      name = "caf\xc3\xa9-renamed.c";
    }
    CHECK(PutRename(&f, source, "keep source bytes\n") == 0);
  }
  if (!strcmp(test, "existing-file")) {
    CHECK(PutRename(&f, name, "keep destination\n") == 0);
    expected = UMI_STATUS_ALREADY_EXISTS;
  }
  if (!strcmp(test, "existing-directory")) {
    CHECK(umi_path_join(f.root, name, folder, sizeof folder) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(folder) == UMI_STATUS_OK);
    expected = UMI_STATUS_ALREADY_EXISTS;
  }
  if (!strcmp(test, "missing-source")) {
    source = "missing.c";
    expected = UMI_STATUS_NOT_FOUND;
  }
  if (!strcmp(test, "missing-parent")) {
    source = "missing/old.c";
    expected = UMI_STATUS_NOT_FOUND;
  }
  if (!strcmp(test, "unchanged")) {
    name = source;
    expected = UMI_STATUS_ALREADY_EXISTS;
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
  CHECK(umi_path_join(f.root, source, from, sizeof from) == UMI_STATUS_OK);
  UmiWorkspaceRenameResult result;
  if (!strcmp(test, "arguments")) {
    CHECK(UmiWorkspaceRenameEntry(NULL, 0U, source, name, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWorkspaceRenameEntry(f.graph, 0U, source, name, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  }
  UmiStatus status = UmiWorkspaceRenameEntry(f.graph, revision, source, name, &result);
  if (status == UMI_STATUS_NOT_IMPLEMENTED) {
    CHECK(umi_fs_exists(from));
    CHECK(StopRename(&f) == 0);
    return 77;
  }
  CHECK(status == expected);
  CHECK(result.renamed == (expected == UMI_STATUS_OK));
  if (expected == UMI_STATUS_OK) {
    CHECK(!umi_fs_exists(from) && umi_fs_exists(result.destination));
    if (!strcmp(test, "directory")) {
      CHECK(result.directory);
      CHECK(umi_path_join(result.destination, "old.c", to, sizeof to) == UMI_STATUS_OK);
      CHECK(RenameText(to, "keep source bytes\n") == 0);
    } else
      CHECK(RenameText(result.destination, "keep source bytes\n") == 0);
  } else if (strcmp(test, "missing-source") && strcmp(test, "missing-parent")) {
    CHECK(RenameText(from, "keep source bytes\n") == 0);
    if (!strcmp(test, "existing-file"))
      CHECK(RenameText(result.destination, "keep destination\n") == 0);
  }
  CHECK(StopRename(&f) == 0);
  return 0;
}
