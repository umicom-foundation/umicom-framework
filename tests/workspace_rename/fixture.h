/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workspace_rename/fixture.h
 *
 * PURPOSE:
 *   Own isolated filesystem fixtures for workspace rename qualification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_WORKSPACE_RENAME_FIXTURE_H
#define UMICOM_TEST_WORKSPACE_RENAME_FIXTURE_H
#include "umicom/platform/filesystem.h"
#include "umicom/platform/workspace_rename.h"
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
typedef struct RenameFixture {
  UmiWorkspaceGraph *graph;
  UmiWorkspaceGraphSnapshot snapshot;
  char root[UMI_PATH_CAPACITY];
} RenameFixture;
static inline int StartRename(RenameFixture *f) {
  char cwd[UMI_PATH_CAPACITY];
  CHECK(umi_fs_current_directory(cwd, sizeof cwd) == UMI_STATUS_OK);
  CHECK(umi_path_join(cwd, "rename-fixture", f->root, sizeof f->root) == UMI_STATUS_OK);
  CHECK(!umi_fs_exists(f->root));
  CHECK(umi_fs_make_directories(f->root) == UMI_STATUS_OK);
  CHECK(umi_workspace_graph_create(&f->graph) == UMI_STATUS_OK);
  CHECK(umi_workspace_graph_open(f->graph, f->root, 1) == UMI_STATUS_OK);
  CHECK(umi_workspace_graph_snapshot(f->graph, &f->snapshot) == UMI_STATUS_OK);
  return 0;
}
static inline int PutRename(RenameFixture *f, const char *name, const char *text) {
  char file[UMI_PATH_CAPACITY];
  CHECK(umi_path_join(f->root, name, file, sizeof file) == UMI_STATUS_OK);
  CHECK(umi_fs_write_text(file, text) == UMI_STATUS_OK);
  return 0;
}
static inline int RenameText(const char *file, const char *text) {
  char *read = NULL;
  CHECK(umi_fs_read_text(file, &read, NULL) == UMI_STATUS_OK);
  int same = strcmp(read, text) == 0;
  umi_fs_free_text(read);
  CHECK(same);
  return 0;
}
static inline int StopRename(RenameFixture *f) {
  umi_workspace_graph_destroy(f->graph);
  CHECK(umi_fs_remove_tree(f->root) == UMI_STATUS_OK);
  return 0;
}
#endif
