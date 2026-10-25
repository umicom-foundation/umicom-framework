/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_storage/test_binding_storage.c
 *
 * PURPOSE:
 *   Persist private locations while preserving board files and refusing cross-project redirection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/kanban/binding_storage.h"
#include "umicom/platform/local_replace.h"
#include "umicom/platform/rooted_files.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  char directory[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY];
  FixtureDirectory(directory);
  FixturePath(other, directory, "other-project");
  UmiKanbanProjectBinding binding, restored, sentinel;
  CHECK(UmiKanbanProjectBindingCreate(directory, directory, "plan.umikb", &binding) ==
        UMI_STATUS_OK);
  CHECK(UmiKanbanBindingStoragePath(directory, directory, file, sizeof file) == UMI_STATUS_OK);
  memset(&restored, 0x57, sizeof restored);
  sentinel = restored;
  if (!strcmp(name, "missing")) {
    CHECK(UmiKanbanBindingStorageLoad(directory, directory, &restored) == UMI_STATUS_NOT_FOUND);
    CHECK(!memcmp(&restored, &sentinel, sizeof restored));
    return 0;
  }
  CHECK(UmiKanbanBindingStorageSave(directory, &binding) == UMI_STATUS_OK);
  if (!strcmp(name, "malformed")) {
    CHECK(UmiLocalFileReplace(file, "bad", 3U) == UMI_STATUS_OK);
    CHECK(UmiKanbanBindingStorageLoad(directory, directory, &restored) != UMI_STATUS_OK);
  } else if (!strcmp(name, "wrong-project")) {
    UmiKanbanProjectBinding different;
    CHECK(UmiKanbanProjectBindingCreate(other, directory, "plan.umikb", &different) ==
          UMI_STATUS_OK);
    unsigned char bytes[UMI_KANBAN_BINDING_MAXIMUM_BYTES];
    size_t size = 0U;
    CHECK(UmiKanbanProjectBindingEncode(&different, bytes, sizeof bytes, &size) == UMI_STATUS_OK);
    CHECK(UmiLocalFileReplace(file, bytes, size) == UMI_STATUS_OK);
    CHECK(UmiKanbanBindingStorageLoad(directory, directory, &restored) == UMI_STATUS_INVALID_STATE);
  } else if (!strcmp(name, "missing-parent")) {
    CHECK(UmiKanbanBindingStorageSave(other, &binding) != UMI_STATUS_OK);
  } else if (!strcmp(name, "invalid-preserves")) {
    size_t before_size = 0U, after_size = 0U;
    unsigned char *before = FixtureRead(file, &before_size);
    strcpy(binding.filename, "../escape.umikb");
    CHECK(UmiKanbanBindingStorageSave(directory, &binding) != UMI_STATUS_OK);
    unsigned char *after = FixtureRead(file, &after_size);
    CHECK(before_size == after_size && !memcmp(before, after, before_size));
    free(before);
    free(after);
  } else if (!strcmp(name, "replace") || !strcmp(name, "roundtrip") || !strcmp(name, "no-board")) {
    if (!strcmp(name, "replace")) {
      strcpy(binding.filename, "changed.umikb");
      CHECK(UmiKanbanBindingStorageSave(directory, &binding) == UMI_STATUS_OK);
    }
    CHECK(UmiKanbanBindingStorageLoad(directory, directory, &restored) == UMI_STATUS_OK);
    CHECK(!strcmp(restored.project_root, binding.project_root));
    CHECK(!strcmp(restored.directory, binding.directory) &&
          !strcmp(restored.filename, binding.filename));
    int exists = 1;
    CHECK(UmiRootedFileExists(directory, binding.filename, &exists) == UMI_STATUS_OK && !exists);
    return 0;
  } else
    return 2;
  CHECK(!memcmp(&restored, &sentinel, sizeof restored));
  return 0;
}
