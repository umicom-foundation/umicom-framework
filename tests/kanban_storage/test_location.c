/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_storage/test_location.c
 *
 * PURPOSE:
 *   Verify deterministic project naming and side-effect-free board location admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/kanban/location.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
#ifdef _WIN32
#define ROOT "C:/Projects/editor"
#define OTHER "C:/Projects/other"
/* The suggested name hashes the normalized native root. Retain the earlier
 * forward-slash vector for review; Windows normalization uses backslashes. */
#if 0
#define EXPECTED "project-d36450143b792cfed64ac31fd20c253dba75053c68bfa2dcbf047d093dcf9dd9.umikb"
#endif
#define EXPECTED "project-f4e46cb0db980e47b055f4f919bd47673a0e935d9096349eda86b2ab82008b8a.umikb"
#else
#define ROOT "/projects/editor"
#define OTHER "/projects/other"
#define EXPECTED "project-86451499628507b0af353d28baf9a84eba85047b7573810668403cf442c4abb4.umikb"
#endif
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  UmiKanbanProjectLocation value, sentinel;
  memset(&value, 0x51, sizeof value);
  sentinel = value;
  UmiStatus status = UMI_STATUS_OK;
  if (!strcmp(name, "stable") || !strcmp(name, "distinct") || !strcmp(name, "trailing") ||
      !strcmp(name, "independent") || !strcmp(name, "filename") || !strcmp(name, "unicode")) {
    const char *root = !strcmp(name, "unicode") ? ROOT "-\xc3\xa9" : ROOT;
    CHECK(UmiKanbanProjectLocationCreate(root, &value) == UMI_STATUS_OK);
    /* Compare canonical output rather than input separator spelling. */
#if 0
    CHECK(!strcmp(value.project_root, root));
#endif
    char canonical[UMI_PATH_CAPACITY];
    CHECK(umi_path_normalise(root, canonical, sizeof canonical) == UMI_STATUS_OK);
    CHECK(!strcmp(value.project_root, canonical));
    CHECK(strlen(value.filename) == 78U && !strncmp(value.filename, "project-", 8U));
    CHECK(!strcmp(value.filename + 72U, ".umikb"));
    if (!strcmp(name, "stable"))
      CHECK(!strcmp(value.filename, EXPECTED));
    if (!strcmp(name, "filename"))
      CHECK(UmiKanbanLocationValidate(ROOT, value.filename) == UMI_STATUS_OK);
    if (!strcmp(name, "distinct")) {
      UmiKanbanProjectLocation other;
      CHECK(UmiKanbanProjectLocationCreate(OTHER, &other) == UMI_STATUS_OK);
      CHECK(strcmp(value.filename, other.filename) != 0);
    }
    if (!strcmp(name, "trailing")) {
      UmiKanbanProjectLocation other;
      CHECK(UmiKanbanProjectLocationCreate(ROOT "/", &other) == UMI_STATUS_OK);
      CHECK(!strcmp(value.filename, other.filename) &&
            !strcmp(value.project_root, other.project_root));
    }
    if (!strcmp(name, "independent")) {
      char mutable_root[UMI_PATH_CAPACITY];
      strcpy(mutable_root, ROOT);
      CHECK(UmiKanbanProjectLocationCreate(mutable_root, &value) == UMI_STATUS_OK);
      mutable_root[3] = 'x';
      /* The independently owned root retains its normalized representation. */
#if 0
      CHECK(!strcmp(value.project_root, ROOT));
#endif
      CHECK(umi_path_equal(value.project_root, ROOT));
    }
    return 0;
  }
  if (!strcmp(name, "arguments")) {
    CHECK(UmiKanbanProjectLocationCreate(ROOT, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    status = UmiKanbanProjectLocationCreate(NULL, &value);
  } else if (!strcmp(name, "relative"))
    status = UmiKanbanProjectLocationCreate("project", &value);
  else if (!strcmp(name, "parent"))
    status = UmiKanbanProjectLocationCreate(ROOT "/../other", &value);
  else if (!strcmp(name, "dot"))
    status = UmiKanbanProjectLocationCreate(ROOT "/.", &value);
  else if (!strcmp(name, "empty"))
    status = UmiKanbanProjectLocationCreate("", &value);
  else if (!strcmp(name, "long")) {
    char root[UMI_PATH_CAPACITY + 1U];
    memset(root, 'x', sizeof root);
    root[sizeof root - 1U] = '\0';
    status = UmiKanbanProjectLocationCreate(root, &value);
    CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
  } else {
    const char *leaf = NULL;
    if (!strcmp(name, "suffix"))
      leaf = "board.txt";
    else if (!strcmp(name, "empty-leaf"))
      leaf = "";
    else if (!strcmp(name, "suffix-only"))
      leaf = ".umikb";
    else if (!strcmp(name, "upper-suffix"))
      leaf = "board.UMIKB";
    else if (!strcmp(name, "parent-leaf"))
      leaf = "../board.umikb";
    else if (!strcmp(name, "slash"))
      leaf = "child/board.umikb";
    else if (!strcmp(name, "backslash"))
      leaf = "child\\board.umikb";
    else if (!strcmp(name, "device"))
      leaf = "CON.umikb";
    else if (!strcmp(name, "null-leaf"))
      leaf = NULL;
    else if (!strcmp(name, "null-root")) {
      status = UmiKanbanLocationValidate(NULL, "board.umikb");
      goto refusal;
    } else if (!strcmp(name, "long-leaf")) {
      char leaf_value[UMI_KANBAN_FILENAME_CAPACITY + 1U];
      memset(leaf_value, 'x', sizeof leaf_value);
      leaf_value[sizeof leaf_value - 1U] = '\0';
      status = UmiKanbanLocationValidate(ROOT, leaf_value);
      CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
      goto refusal;
    } else
      return 2;
    status = UmiKanbanLocationValidate(ROOT, leaf);
  }
refusal:
  CHECK(status != UMI_STATUS_OK && memcmp(&value, &sentinel, sizeof value) == 0);
  return 0;
}
