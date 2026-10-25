/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_path_usage.c
 *
 * PURPOSE:
 *   Check clean, dirty and inactive source identities without changing drafts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../workspace_rename/fixture.h"
#include "umicom/document/document.h"
#include "umicom/document/path_usage.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *test = argv[1];
  const char *cases[] = {"source", "destination", "descendant", "sibling",   "inactive",
                         "dirty",  "untitled",    "closed",     "arguments", "normalised"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(test, cases[i]))
      known = 1;
  CHECK(known);
  RenameFixture f = {0};
  CHECK(StartRename(&f) == 0);
  CHECK(PutRename(&f, "source.c", "original") == 0);
  char source[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY], viewId[UMI_UI_ID_CAPACITY];
  CHECK(umi_path_join(f.root, "source.c", source, sizeof source) == UMI_STATUS_OK);
  CHECK(umi_path_join(f.root, "other", other, sizeof other) == UMI_STATUS_OK);
  UmiCommandRegistry *commands = NULL;
  UmiUiWorkbench *workbench = NULL;
  UmiDocumentStore *store = NULL;
  UmiDocumentCoordinator *documents = NULL;
  CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
  CHECK(umi_ui_workbench_create("test.path-usage", commands, &workbench) == UMI_STATUS_OK);
  CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
  UmiDocumentProvider provider = umi_document_local_provider();
  CHECK(umi_document_coordinator_create(store, workbench, &provider, &documents) == UMI_STATUS_OK);
  CHECK(umi_document_coordinator_open(documents, source, viewId, sizeof viewId) == UMI_STATUS_OK);
  if (!strcmp(test, "dirty")) {
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, viewId, &view) == UMI_STATUS_OK);
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "unsaved", 7U) == UMI_STATUS_OK);
  }
  if (!strcmp(test, "untitled") || !strcmp(test, "closed"))
    CHECK(umi_document_coordinator_close_active(documents, 1) == UMI_STATUS_OK);
  if (!strcmp(test, "untitled") || !strcmp(test, "inactive"))
    CHECK(umi_document_coordinator_new(documents, "source.c", viewId, sizeof viewId) ==
          UMI_STATUS_OK);
  size_t count = umi_document_coordinator_count(documents);
  int used = 99;
  const char *from = source, *to = NULL;
  int expected = 1;
  if (!strcmp(test, "destination")) {
    from = other;
    to = source;
  }
  if (!strcmp(test, "descendant"))
    from = f.root;
  if (!strcmp(test, "sibling")) {
    from = other;
    expected = 0;
  }
  if (!strcmp(test, "untitled") || !strcmp(test, "closed"))
    expected = 0;
  char normal[UMI_PATH_CAPACITY];
  if (!strcmp(test, "normalised")) {
    int written = snprintf(normal, sizeof normal, "%s/./source.c", f.root);
    CHECK(written > 0 && (size_t)written < sizeof normal);
    from = normal;
  }
  if (!strcmp(test, "arguments")) {
    CHECK(UmiDocumentCoordinatorPathInUse(NULL, source, NULL, &used) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          used == 0);
    CHECK(UmiDocumentCoordinatorPathInUse(documents, "relative", NULL, &used) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorPathInUse(documents, source, "relative", &used) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorPathInUse(documents, source, NULL, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
  }
  CHECK(UmiDocumentCoordinatorPathInUse(documents, from, to, &used) == UMI_STATUS_OK &&
        used == expected);
  CHECK(umi_document_coordinator_count(documents) == count);
  CHECK(RenameText(source, "original") == 0);
  if (!strcmp(test, "dirty")) {
    char *text = NULL;
    size_t bytes = 0U;
    CHECK(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(workbench), viewId, &text,
                                         &bytes) == UMI_STATUS_OK);
    CHECK(bytes == 7U && !strcmp(text, "unsaved"));
    UmiUiDocumentViewModelFreeText(text);
  }
  umi_document_coordinator_destroy(documents);
  umi_document_store_destroy(store);
  umi_ui_workbench_destroy(workbench);
  umi_command_registry_destroy(commands);
  CHECK(StopRename(&f) == 0);
  return 0;
}
