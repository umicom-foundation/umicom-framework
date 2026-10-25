/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_copy_output.c
 *
 * PURPOSE:
 *   Exercise exclusive source-copy creation and retained data in isolated native filesystem
 * fixtures.
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
#include "../../src/document/copy_internal.h"
#include "../build_log/fixture.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1],
             *cases[] = {"new",     "exists", "source", "missing-parent", "relative", "dot",
                         "unicode", "repeat", "empty",  "symlink",        "directory"};
  bool known = false;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(name, cases[i]) == 0)
      known = true;
  CHECK(known);
  char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], second[UMI_PATH_CAPACITY];
  FixtureDirectory(root);
  FixturePath(path, root, strcmp(name, "unicode") == 0 ? "caf\xc3\xa9.c" : "copy.c");
  UmiDocumentCopyInfo info = {.document_id = 1U, .encoding = UMI_DOCUMENT_ENCODING_UTF8};
  strcpy(info.display_name, "draft.c");
  if (strcmp(name, "source") == 0)
    strcpy(info.source_path, path);
  const char *text = strcmp(name, "empty") == 0 ? "" : "draft\n";
  UmiDocumentCopy *copy = NULL;
  CHECK(DocumentCopyCreate(&info, text, strlen(text), &copy) == UMI_STATUS_OK);
  UmiDocumentCopyResult result = {0};
  UmiStatus status;
  if (strcmp(name, "exists") == 0) {
    CHECK(UmiDocumentCopyWriteNew(copy, path, &result) == UMI_STATUS_OK);
    status = UmiDocumentCopyWriteNew(copy, path, &result);
    CHECK(status == UMI_STATUS_ALREADY_EXISTS && !result.created && result.bytes_written == 0U);
  } else if (strcmp(name, "source") == 0 || strcmp(name, "relative") == 0 ||
             strcmp(name, "dot") == 0) {
    const char *target = path;
    if (strcmp(name, "relative") == 0)
      target = "relative.c";
    if (strcmp(name, "dot") == 0) {
      FixturePath(second, root, "./copy.c");
      target = second;
    }
    CHECK(UmiDocumentCopyWriteNew(copy, target, &result) == UMI_STATUS_INVALID_ARGUMENT &&
          !result.created);
    UmiDocumentCopyDestroy(copy);
    return 0;
  } else if (strcmp(name, "missing-parent") == 0) {
    FixturePath(second, root, "absent/copy.c");
    CHECK(UmiDocumentCopyWriteNew(copy, second, &result) != UMI_STATUS_OK && !result.created);
    UmiDocumentCopyDestroy(copy);
    return 0;
  } else if (strcmp(name, "directory") == 0) {
    CHECK(UmiDocumentCopyWriteNew(copy, root, &result) != UMI_STATUS_OK && !result.created);
    UmiDocumentCopyDestroy(copy);
    return 0;
  } else if (strcmp(name, "symlink") == 0) {
    CHECK(UmiDocumentCopyWriteNew(copy, path, &result) == UMI_STATUS_OK);
    FixturePath(second, root, "alias.c");
#ifdef _WIN32
    if (!FixtureSymbolicLink(second, path, 0)) {
      UmiDocumentCopyDestroy(copy);
      return 77;
    }
#else
    CHECK(symlink(path, second) == 0);
#endif
    CHECK(UmiDocumentCopyWriteNew(copy, second, &result) != UMI_STATUS_OK && !result.created);
  } else {
    CHECK(UmiDocumentCopyWriteNew(copy, path, &result) == UMI_STATUS_OK && result.created &&
          result.status == UMI_STATUS_OK && result.bytes_written == strlen(text));
    if (strcmp(name, "repeat") == 0) {
      FixturePath(second, root, "another.c");
      CHECK(UmiDocumentCopyWriteNew(copy, second, &result) == UMI_STATUS_OK);
    }
  }
  size_t length = 0U;
  unsigned char *bytes = FixtureRead(path, &length);
  CHECK(length == strlen(text) && memcmp(bytes, text, length) == 0);
  free(bytes);
  UmiDocumentCopyDestroy(copy);
  return 0;
}
