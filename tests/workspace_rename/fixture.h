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
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
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
  /* A failed earlier test may leave evidence. Reserve a fresh directory
   * atomically beneath this case; never adopt or remove an existing entry.
   * Retain the old fixed-name setup to explain the rerun collision. */
#if 0
  CHECK(umi_path_join(cwd, "rename-fixture", f->root, sizeof f->root) == UMI_STATUS_OK);
  CHECK(!umi_fs_exists(f->root));
  CHECK(umi_fs_make_directories(f->root) == UMI_STATUS_OK);
#endif
#ifdef _WIN32
  unsigned long process = (unsigned long)GetCurrentProcessId();
#else
  unsigned long process = (unsigned long)getpid();
#endif
  int created = 0;
  for (unsigned attempt = 0U; attempt < 1000U; ++attempt) {
    char leaf[80];
    int length = snprintf(leaf, sizeof leaf, "rename-fixture-%lu-%u", process, attempt);
    CHECK(length > 0 && (size_t)length < sizeof leaf);
    CHECK(umi_path_join(cwd, leaf, f->root, sizeof f->root) == UMI_STATUS_OK);
#ifdef _WIN32
    wchar_t wide[UMI_PATH_CAPACITY];
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, f->root, -1, wide,
                              (int)UMI_PATH_CAPACITY) != 0);
    if (CreateDirectoryW(wide, NULL)) { created = 1; break; }
    CHECK(GetLastError() == ERROR_ALREADY_EXISTS);
#else
    if (mkdir(f->root, 0700) == 0) { created = 1; break; }
    CHECK(errno == EEXIST);
#endif
  }
  CHECK(created);
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
