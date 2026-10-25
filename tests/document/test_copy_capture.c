/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_copy_capture.c
 *
 * PURPOSE:
 *   Verify that copies capture visible text and preserve live editor identity, selection and
 * history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/copy.h"

int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  const char *cases[] = {"arguments",  "missing",     "dirty",     "readonly", "selection",
                         "after-edit", "after-owner", "other-tab", "metadata"};
  bool known = false;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(name, cases[i]) == 0)
      known = true;
  CHECK(known);
  ReplacementFixture f = {0};
  CHECK(Start(&f, "original\n") == 0);
  UmiDocumentCopy *copy = NULL;
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
  if (strcmp(name, "arguments") == 0) {
    copy = (UmiDocumentCopy *)(uintptr_t)1U;
    CHECK(UmiDocumentCoordinatorCaptureCopy(NULL, f.id, &copy) == UMI_STATUS_INVALID_ARGUMENT &&
          copy == NULL);
    CHECK(UmiDocumentCoordinatorCaptureCopy(f.documents, 0U, &copy) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorCaptureCopy(f.documents, f.id, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCopyRead(NULL, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    Stop(&f);
    return 0;
  }
  if (strcmp(name, "missing") == 0) {
    CHECK(UmiDocumentCoordinatorCaptureCopy(f.documents, UINT64_MAX, &copy) ==
              UMI_STATUS_NOT_FOUND &&
          copy == NULL);
    Stop(&f);
    return 0;
  }
  const char *expected = "original\n";
  if (strcmp(name, "dirty") == 0) {
    CHECK(Draft(&f, "unsaved\n") == UMI_STATUS_OK);
    expected = "unsaved\n";
  }
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
  if (strcmp(name, "readonly") == 0)
    view.read_only = 1;
  if (strcmp(name, "selection") == 0) {
    view.cursor_offset = 2U;
    view.selection_length = 3U;
  }
  CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  if (strcmp(name, "other-tab") == 0) {
    char other[UMI_UI_ID_CAPACITY];
    CHECK(umi_document_coordinator_new(f.documents, "other.c", other, sizeof other) ==
          UMI_STATUS_OK);
  }
  UmiDocumentSnapshot before, after;
  UmiDocumentWorkingCopySnapshot activeBefore, activeAfter;
  CHECK(umi_document_store_snapshot(f.store, f.id, &before) == UMI_STATUS_OK);
  CHECK(umi_document_coordinator_active_snapshot(f.documents, &activeBefore) == UMI_STATUS_OK);
  CHECK(UmiDocumentCoordinatorCaptureCopy(f.documents, f.id, &copy) == UMI_STATUS_OK);
  CHECK(umi_document_store_snapshot(f.store, f.id, &after) == UMI_STATUS_OK);
  CHECK(before.revision == after.revision && before.saved_revision == after.saved_revision &&
        before.dirty == after.dirty);
  CHECK(strcmp(before.path, after.path) == 0 && before.has_path == after.has_path);
  CHECK(umi_document_coordinator_active_snapshot(f.documents, &activeAfter) == UMI_STATUS_OK);
  CHECK(activeBefore.document_id == activeAfter.document_id &&
        activeBefore.undo_count == activeAfter.undo_count &&
        activeBefore.redo_count == activeAfter.redo_count &&
        activeBefore.dirty == activeAfter.dirty);
  UmiUiDocumentViewSnapshot final;
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &final) == UMI_STATUS_OK);
  CHECK(final.cursor_offset == view.cursor_offset &&
        final.selection_length == view.selection_length && final.read_only == view.read_only);
  UmiDocumentCopyInfo info;
  CHECK(UmiDocumentCopyInspect(copy, &info) == UMI_STATUS_OK);
  CHECK(info.document_id == f.id && info.text_bytes == strlen(expected) &&
        info.source_path[0] == '\0');
  CHECK(strcmp(info.display_name, "review.c") == 0);
  /* The exported buffer has a larger name capacity than the store. Its unused
   * tail must not contain adjacent store metadata or stack bytes. */
  for (size_t i = strlen(info.display_name) + 1U; i < sizeof info.display_name; ++i)
    CHECK(info.display_name[i] == '\0');
  if (strcmp(name, "after-edit") == 0)
    CHECK(Draft(&f, "newer draft\n") == UMI_STATUS_OK);
  if (strcmp(name, "after-owner") == 0) {
    Stop(&f);
    memset(&f, 0, sizeof f);
  }
  const unsigned char *bytes = NULL;
  size_t count = 0U;
  CHECK(UmiDocumentCopyRead(copy, &bytes, &count) == UMI_STATUS_OK);
  CHECK(count == strlen(expected) && memcmp(bytes, expected, count) == 0);
  UmiDocumentCopyDestroy(copy);
  if (f.documents != NULL)
    Stop(&f);
  return 0;
}
