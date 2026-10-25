/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_editor_config_evidence.c
 *
 * PURPOSE:
 *   Verify configuration scope and evidence comparisons detect added, removed and changed sources.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "editor_config_adoption_fixture.h"
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char
      *name = argv[1],
      *names[] = {
          "same",           "scope",    "copy-ownership",   "changed-value",  "changed-comment",
          "closer-added",   "removed",  "different-target", "different-root", "normalized-root",
          "same-root-stop", "arguments"};
  int known = 0;
  for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(name, names[i]))
      known = 1;
  if (!known)
    return 2;
  AdoptionFixture f = {0};
  AdoptionStartFixture(&f, "root=true\n[*]\nindent_size=2\n");
  AdoptionInspect(&f);
  UmiEditorConfigInspectionScope scope;
  CHECK(UmiEditorConfigInspectionCopyScope(f.inspection, &scope) == UMI_STATUS_OK);
  CHECK(umi_path_equal(scope.project_root, f.root) && !strcmp(scope.relative_file, "src/main.c"));
  if (!strcmp(name, "arguments")) {
    int equal = 19;
    CHECK(UmiEditorConfigInspectionEqual(NULL, f.inspection, &equal) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          equal == 19);
    CHECK(UmiEditorConfigInspectionEqual(f.inspection, NULL, &equal) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          equal == 19);
    CHECK(UmiEditorConfigInspectionEqual(f.inspection, f.inspection, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorConfigInspectionCopyScope(NULL, &scope) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorConfigInspectionCopyScope(f.inspection, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "copy-ownership")) {
    UmiEditorConfigInspectionDestroy(f.inspection);
    f.inspection = NULL;
    CHECK(umi_path_equal(scope.project_root, f.root) && !strcmp(scope.relative_file, "src/main.c"));
  } else {
    int expected = 1;
    if (!strcmp(name, "changed-value")) {
      AdoptionSave(&f, ".editorconfig", "root=true\n[*]\nindent_size=4\n");
      expected = 0;
    }
    if (!strcmp(name, "changed-comment")) {
      AdoptionSave(&f, ".editorconfig", "# changed\nroot=true\n[*]\nindent_size=2\n");
      expected = 0;
    }
    if (!strcmp(name, "closer-added")) {
      AdoptionSave(&f, "src/.editorconfig", "[*]\nindent_size=2\n");
      expected = 0;
    }
    if (!strcmp(name, "removed")) {
      CHECK(UmiRootedFileRemove(f.root, ".editorconfig") == UMI_STATUS_OK);
      expected = 0;
    }
    if (!strcmp(name, "different-target")) {
      CHECK(UmiEditorConfigInspectionLoad(f.root, "src/other.c", NULL, &f.fresh, NULL) ==
            UMI_STATUS_OK);
      expected = 0;
    } else if (!strcmp(name, "different-root")) {
      char root[UMI_PATH_CAPACITY];
      FixtureDirectory(root);
      CHECK(UmiEditorConfigInspectionLoad(root, "main.c", NULL, &f.fresh, NULL) == UMI_STATUS_OK);
      expected = 0;
    } else if (!strcmp(name, "normalized-root")) {
      char root[UMI_PATH_CAPACITY];
      CHECK(umi_path_join(f.root, ".", root, sizeof root) == UMI_STATUS_OK);
      CHECK(UmiEditorConfigInspectionLoad(root, "src/main.c", NULL, &f.fresh, NULL) ==
            UMI_STATUS_OK);
    } else if (!strcmp(name, "same-root-stop")) {
      AdoptionSave(&f, "src/.editorconfig", "root=true\n[*]\nindent_size=2\n");
      UmiEditorConfigInspectionDestroy(f.inspection);
      f.inspection = NULL;
      AdoptionInspect(&f);
      AdoptionSave(&f, ".editorconfig", "this ancestor is ignored by root=true");
      AdoptionReinspect(&f);
    } else
      AdoptionReinspect(&f);
    int equal = -1;
    CHECK(UmiEditorConfigInspectionEqual(f.inspection, f.fresh, &equal) == UMI_STATUS_OK &&
          equal == expected);
    equal = -1;
    CHECK(UmiEditorConfigInspectionEqual(f.fresh, f.inspection, &equal) == UMI_STATUS_OK &&
          equal == expected);
  }
  AdoptionStopFixture(&f);
  return 0;
}
