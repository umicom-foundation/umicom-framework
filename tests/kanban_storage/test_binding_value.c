/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_storage/test_binding_value.c
 *
 * PURPOSE:
 *   Reject invalid board bindings without changing caller-owned state.
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
  UmiKanbanProjectBinding value, sentinel;
  memset(&value, 0x36, sizeof value);
  sentinel = value;
  const char *root = PROJECT, *directory = BOARDS, *filename = "private.umikb";
  if (!strcmp(name, "root-relative"))
    root = "project";
  else if (!strcmp(name, "directory-relative"))
    directory = "boards";
  else if (!strcmp(name, "root-parent"))
    root = PROJECT "/../other";
  else if (!strcmp(name, "directory-parent"))
    directory = BOARDS "/../other";
  else if (!strcmp(name, "filename-parent"))
    filename = "../private.umikb";
  else if (!strcmp(name, "filename-extension"))
    filename = "private.txt";
  else if (!strcmp(name, "root-null"))
    root = NULL;
  else if (!strcmp(name, "directory-null"))
    directory = NULL;
  else if (!strcmp(name, "filename-null"))
    filename = NULL;
  else if (!strcmp(name, "normalised")) {
    root = PROJECT "/";
    directory = BOARDS "/";
  } else if (!strcmp(name, "unicode")) {
    root = PROJECT "-\xc3\xa9";
    directory = BOARDS "-\xc3\xa9";
  } else if (strcmp(name, "valid") && strcmp(name, "alias") && strcmp(name, "unterminated") &&
             strcmp(name, "noncanonical") && strcmp(name, "output-null"))
    return 2;
  if (!strcmp(name, "output-null")) {
    CHECK(UmiKanbanProjectBindingCreate(root, directory, filename, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  UmiStatus status = UmiKanbanProjectBindingCreate(root, directory, filename, &value);
  bool refused = strstr(name, "relative") || strstr(name, "parent") || strstr(name, "extension") ||
                 strstr(name, "null");
  if (refused) {
    CHECK(status != UMI_STATUS_OK);
    CHECK(!memcmp(&value, &sentinel, sizeof value));
    return 0;
  }
  CHECK(status == UMI_STATUS_OK);
  CHECK(UmiKanbanProjectBindingValidate(&value) == UMI_STATUS_OK);
  CHECK(!strcmp(value.filename, "private.umikb"));
  if (!strcmp(name, "normalised")) {
    CHECK(!strcmp(value.project_root, PROJECT) && !strcmp(value.directory, BOARDS));
  } else if (!strcmp(name, "alias")) {
    UmiKanbanProjectBinding copy = value;
    CHECK(UmiKanbanProjectBindingCreate(value.project_root, value.directory, value.filename,
                                        &value) == UMI_STATUS_OK);
    CHECK(BindingEqual(&value, &copy));
  } else if (!strcmp(name, "unterminated")) {
    memset(value.filename, 'x', sizeof value.filename);
    CHECK(UmiKanbanProjectBindingValidate(&value) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "noncanonical")) {
    strcat(value.project_root, "/");
    CHECK(UmiKanbanProjectBindingValidate(&value) == UMI_STATUS_INVALID_ARGUMENT);
  }
  return 0;
}
