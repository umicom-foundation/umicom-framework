/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_editor_config_inspection.c
 *
 * PURPOSE:
 *   Exercise project boundaries, file provenance and cancellation in isolated configuration
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
#include "umicom/document/editor_config_inspection.h"
#include "umicom/platform/local_replace.h"
static void MakeDirectory(const char *path) {
#ifdef _WIN32
  wchar_t wide[UMI_PATH_CAPACITY];
  CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, (int)UMI_PATH_CAPACITY) >
        0);
  CHECK(CreateDirectoryW(wide, NULL));
#else
  CHECK(mkdir(path, 0700) == 0);
#endif
}
static void Save(const char *root, const char *relative, const char *text) {
  char path[UMI_PATH_CAPACITY];
  FixturePath(path, root, relative);
  CHECK(UmiLocalFileReplace(path, text, strlen(text)) == UMI_STATUS_OK);
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1];
  const char *names[] = {"inherit",   "root-stop", "boundary",  "missing",   "missing-directory",
                         "unicode",   "malformed", "oversized", "directory", "traversal",
                         "cancelled", "arguments", "metadata",  "unchanged", "case-selection"};
  int known = 0;
  for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(name, names[i]))
      known = 1;
  if (!known)
    return 2;
  char root[UMI_PATH_CAPACITY], child[UMI_PATH_CAPACITY];
  FixtureDirectory(root);
  FixturePath(child, root, "src");
  MakeDirectory(child);
  UmiEditorConfigInspection *inspection = NULL;
  UmiEditorConfigInspectionDiagnostic diagnostic;
  if (!strcmp(name, "arguments")) {
    CHECK(UmiEditorConfigInspectionLoad(NULL, "a", NULL, &inspection, &diagnostic) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          inspection == NULL);
    CHECK(UmiEditorConfigInspectionLoad(root, "a", NULL, NULL, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorConfigInspectionFileAt(NULL, 0, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  if (!strcmp(name, "traversal")) {
    CHECK(UmiEditorConfigInspectionLoad(root, "../a.c", NULL, &inspection, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          inspection == NULL);
    return 0;
  }
  if (!strcmp(name, "cancelled")) {
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    umi_cancellation_token_request(cancel);
    CHECK(UmiEditorConfigInspectionLoad(root, "src/a.c", cancel, &inspection, NULL) ==
              UMI_STATUS_CANCELLED &&
          inspection == NULL);
    umi_cancellation_token_destroy(cancel);
    return 0;
  }
  if (strcmp(name, "missing") && strcmp(name, "missing-directory"))
    Save(root, ".editorconfig", "root=true\n[*]\nindent_size=4\ncharset=utf-8\n");
  if (!strcmp(name, "inherit") || !strcmp(name, "metadata") || !strcmp(name, "case-selection"))
    Save(root, "src/.editorconfig", "[*.c]\nindent_size=2\n");
  if (!strcmp(name, "root-stop")) {
    Save(root, ".editorconfig", "this parent is deliberately malformed");
    Save(root, "src/.editorconfig", "root=true\n[*]\nindent_size=2\n");
  }
  if (!strcmp(name, "malformed"))
    Save(root, "src/.editorconfig", "[*]\nnot a pair\n");
  if (!strcmp(name, "directory")) {
    char path[UMI_PATH_CAPACITY];
    FixturePath(path, child, ".editorconfig");
    MakeDirectory(path);
  }
  if (!strcmp(name, "oversized")) {
    char path[UMI_PATH_CAPACITY];
    FixturePath(path, child, ".editorconfig");
    size_t n = UMI_EDITOR_CONFIG_MAXIMUM_BYTES + 1U;
    char *text = malloc(n);
    CHECK(text);
    memset(text, 'x', n);
    CHECK(UmiLocalFileReplace(path, text, n) == UMI_STATUS_OK);
    free(text);
  }
  const char *target = !strcmp(name, "unicode")             ? "src/caf\xc3\xa9.c"
                       : !strcmp(name, "missing-directory") ? "absent/a.c"
                       : !strcmp(name, "case-selection")    ? "src/a.h"
                                                            : "src/a.c";
  const char *boundary = !strcmp(name, "boundary") ? child : root;
  UmiStatus status = UmiEditorConfigInspectionLoad(
      boundary, !strcmp(name, "boundary") ? "a.c" : target, NULL, &inspection, &diagnostic);
  if (!strcmp(name, "malformed") || !strcmp(name, "oversized") || !strcmp(name, "directory")) {
    CHECK(status != UMI_STATUS_OK && inspection == NULL);
    CHECK(!strcmp(diagnostic.relative_path, "src/.editorconfig"));
    if (!strcmp(name, "malformed"))
      CHECK(diagnostic.line == 2U);
    return 0;
  }
  CHECK(status == UMI_STATUS_OK && inspection != NULL && diagnostic.status == UMI_STATUS_OK);
  const UmiEditorConfigResult *r = UmiEditorConfigInspectionResult(inspection);
  UmiEditorConfigProperty property;
  if (!strcmp(name, "missing") || !strcmp(name, "missing-directory") || !strcmp(name, "boundary"))
    CHECK(UmiEditorConfigResultCount(r) == 0U &&
          UmiEditorConfigInspectionFileCount(inspection) == 0U);
  else {
    CHECK(UmiEditorConfigResultFind(r, "indent_size", &property) == UMI_STATUS_OK);
    CHECK(!strcmp(property.value, (!strcmp(name, "inherit") || !strcmp(name, "metadata") ||
                                   !strcmp(name, "root-stop"))
                                      ? "2"
                                      : "4"));
  }
  if (!strcmp(name, "metadata")) {
    CHECK(UmiEditorConfigInspectionFileCount(inspection) == 2U);
    UmiEditorConfigInspectionFile file;
    CHECK(UmiEditorConfigInspectionFileAt(inspection, 0U, &file) == UMI_STATUS_OK);
    CHECK(!strcmp(file.relative_path, ".editorconfig") && file.root && strlen(file.sha256) == 64U &&
          file.bytes > 0U);
    char expected[65];
    const char *content = "root=true\n[*]\nindent_size=4\ncharset=utf-8\n";
    CHECK(UmiSha256Buffer(content, strlen(content), expected) == UMI_STATUS_OK &&
          !strcmp(file.sha256, expected));
  }
  if (!strcmp(name, "unchanged")) {
    Save(root, ".editorconfig", "root=true\n[*]\nindent_size=8\n");
    CHECK(UmiEditorConfigResultFind(r, "indent_size", &property) == UMI_STATUS_OK &&
          !strcmp(property.value, "4"));
  }
  UmiEditorConfigInspectionDestroy(inspection);
  return 0;
}
