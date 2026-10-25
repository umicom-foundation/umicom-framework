/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workspace_rename/test_links.c
 *
 * PURPOSE:
 *   Refuse linked source entries and parent folders while preserving linked targets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "fixture.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
static int Link(const char *alias, const char *target, int directory) {
#ifdef _WIN32
  wchar_t a[UMI_PATH_CAPACITY], t[UMI_PATH_CAPACITY];
  CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, alias, -1, a, (int)UMI_PATH_CAPACITY) >
        0);
  CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, target, -1, t, (int)UMI_PATH_CAPACITY) >
        0);
  DWORD flags = directory ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0U;
  if (CreateSymbolicLinkW(a, t, flags | 2U) || CreateSymbolicLinkW(a, t, flags))
    return 0;
  DWORD error = GetLastError();
  return error == ERROR_PRIVILEGE_NOT_HELD || error == ERROR_NOT_SUPPORTED ||
                 error == ERROR_INVALID_FUNCTION
             ? 77
             : 1;
#else
  (void)directory;
  return symlink(target, alias) == 0 ? 0 : 1;
#endif
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  int parent = !strcmp(argv[1], "parent");
  CHECK(parent || !strcmp(argv[1], "source"));
  RenameFixture f = {0};
  CHECK(StartRename(&f) == 0);
  char target[UMI_PATH_CAPACITY], alias[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY];
  CHECK(umi_path_join(f.root, "real", target, sizeof target) == UMI_STATUS_OK);
  CHECK(umi_fs_make_directories(target) == UMI_STATUS_OK);
  CHECK(PutRename(&f, "real/old.c", "target bytes") == 0);
  CHECK(umi_path_join(target, "old.c", file, sizeof file) == UMI_STATUS_OK);
  CHECK(umi_path_join(f.root, "alias", alias, sizeof alias) == UMI_STATUS_OK);
  int linked = Link(alias, parent ? target : file, parent);
  if (linked == 77) {
    CHECK(StopRename(&f) == 0);
    return 77;
  }
  CHECK(linked == 0);
  UmiWorkspaceRenameResult result;
  UmiStatus status =
      UmiWorkspaceRenameEntry(f.graph, 0U, parent ? "alias/old.c" : "alias", "new.c", &result);
  if (status == UMI_STATUS_NOT_IMPLEMENTED) {
    CHECK(StopRename(&f) == 0);
    return 77;
  }
  CHECK(status == UMI_STATUS_PERMISSION_DENIED && !result.renamed);
  CHECK(RenameText(file, "target bytes") == 0);
  CHECK(StopRename(&f) == 0);
  return 0;
}
