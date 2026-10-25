/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_indentation_storage.c
 *
 * PURPOSE:
 *   Exercise editor preference replacement and bounded loads in exclusively reserved local
 * fixtures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/document/indentation_storage.h"
#include "umicom/platform/local_replace.h"
#include "umicom/platform/path.h"
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1], *cases[] = {"round-trip", "replace",    "invalid-save",  "malformed",
                                          "oversized",  "missing",    "directory",     "unicode",
                                          "path",       "path-short", "path-relative", "arguments"};
  int known = 0;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  if (!known)
    return 2;
  char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
  FixtureDirectory(root);
  FixturePath(path, root, !strcmp(name, "unicode") ? "caf\xc3\xa9.txt" : "preferences.txt");
  UmiEditorIndentation input = {2, 0}, out = {8, 1};
  if (!strcmp(name, "path") || !strcmp(name, "path-short") || !strcmp(name, "path-relative")) {
    strcpy(path, "retained");
    UmiStatus status = UmiDocumentIndentationStoragePath(
        "Studio", !strcmp(name, "path-relative") ? "relative" : root, path,
        !strcmp(name, "path-short") ? 2U : sizeof path);
    if (!strcmp(name, "path"))
      CHECK(status == UMI_STATUS_OK && umi_path_is_within(root, path) &&
            strstr(path, "editor-indentation.txt"));
    else
      CHECK(status != UMI_STATUS_OK && !strcmp(path, "retained"));
    return 0;
  }
  if (!strcmp(name, "arguments")) {
    CHECK(UmiDocumentIndentationStoragePath("Studio", root, NULL, 0U) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentIndentationStorageLoad(path, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentIndentationStorageSave(path, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentIndentationStorageLoad("relative", &out) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  if (!strcmp(name, "missing")) {
    CHECK(UmiDocumentIndentationStorageLoad(path, &out) == UMI_STATUS_NOT_FOUND);
    CHECK(out.tab_width == 8U && out.insert_spaces == 1);
    return 0;
  }
  if (!strcmp(name, "directory")) {
    CHECK(UmiDocumentIndentationStorageLoad(root, &out) != UMI_STATUS_OK);
    CHECK(UmiDocumentIndentationStorageSave(root, &input) != UMI_STATUS_OK);
    CHECK(out.tab_width == 8U && out.insert_spaces == 1);
    return 0;
  }
  CHECK(UmiDocumentIndentationStorageSave(path, &input) == UMI_STATUS_OK);
  if (!strcmp(name, "replace")) {
    input = (UmiEditorIndentation){16, 1};
    CHECK(UmiDocumentIndentationStorageSave(path, &input) == UMI_STATUS_OK);
  }
  if (!strcmp(name, "invalid-save")) {
    UmiEditorIndentation invalid = {0, 0};
    CHECK(UmiDocumentIndentationStorageSave(path, &invalid) == UMI_STATUS_INVALID_ARGUMENT);
  }
  if (!strcmp(name, "malformed"))
    CHECK(UmiLocalFileReplace(path, "bad", 3U) == UMI_STATUS_OK);
  if (!strcmp(name, "oversized")) {
    char huge[129];
    memset(huge, 'x', sizeof huge);
    CHECK(UmiLocalFileReplace(path, huge, sizeof huge) == UMI_STATUS_OK);
  }
  UmiStatus status = UmiDocumentIndentationStorageLoad(path, &out);
  if (!strcmp(name, "malformed") || !strcmp(name, "oversized"))
    CHECK(status != UMI_STATUS_OK && out.tab_width == 8U && out.insert_spaces == 1);
  else
    CHECK(status == UMI_STATUS_OK && out.tab_width == input.tab_width &&
          out.insert_spaces == input.insert_spaces);
  return 0;
}
