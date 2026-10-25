/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_storage/test_binding_path.c
 *
 * PURPOSE:
 *   Keep deterministic private settings paths independent of current directories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "binding_fixture.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  char value[UMI_PATH_CAPACITY], previous[UMI_PATH_CAPACITY];
  memset(value, 0x39, sizeof value);
  memcpy(previous, value, sizeof value);
  const char *settings = SETTINGS, *project = PROJECT;
  size_t capacity = sizeof value;
  if (!strcmp(name, "relative-settings"))
    settings = "private";
  else if (!strcmp(name, "relative-project"))
    project = "project";
  else if (!strcmp(name, "short"))
    capacity = 2U;
  else if (strcmp(name, "deterministic") && strcmp(name, "distinct") &&
           strcmp(name, "private-name"))
    return 2;
  UmiStatus status = UmiKanbanBindingStoragePath(settings, project, value, capacity);
  if (!strcmp(name, "relative-settings") || !strcmp(name, "relative-project") ||
      !strcmp(name, "short")) {
    CHECK(status != UMI_STATUS_OK && !memcmp(value, previous, sizeof value));
    return 0;
  }
  CHECK(status == UMI_STATUS_OK);
  CHECK(strstr(value, "board-") && strstr(value, ".umilocation"));
  CHECK(strstr(value, "editor") == NULL);
  char another[UMI_PATH_CAPACITY];
  CHECK(UmiKanbanBindingStoragePath(settings,
                                    !strcmp(name, "distinct") ? PROJECT "-other" : PROJECT "/",
                                    another, sizeof another) == UMI_STATUS_OK);
  CHECK((strcmp(value, another) != 0) == !strcmp(name, "distinct"));
  return 0;
}
