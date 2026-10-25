/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_language_override_history.c
 *
 * PURPOSE:
 *   Check language policy preserves pending drafts, selections, read-only state, Undo and full text
 * beyond previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/language_comment.h"
#include "umicom/document/language_override.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1], *cases[] = {"pending", "selection",     "readonly",         "history",
                                          "undo",    "redo",          "comment",          "large",
                                          "clean",   "text-revision", "policy-after-sync"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    known |= !strcmp(name, cases[i]);
  CHECK(known);
  ReplacementFixture f = {0};
  CHECK(Start(&f, "alpha\nbeta") == 0);
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  char *large = NULL;
  const char *expected = "alpha\nbeta";
  if (!strcmp(name, "pending") || !strcmp(name, "policy-after-sync")) {
    expected = "pending draft";
    CHECK(Draft(&f, expected) == UMI_STATUS_OK);
  }
  if (!strcmp(name, "large")) {
    large = malloc(65537U);
    CHECK(large);
    memset(large, 'a', 65536U);
    large[65536U] = '\0';
    expected = large;
    CHECK(Draft(&f, large) == UMI_STATUS_OK);
  }
  UmiUiDocumentViewSnapshot v;
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &v) == UMI_STATUS_OK);
  if (!strcmp(name, "selection")) {
    v.cursor_offset = 2U;
    v.selection_length = 3U;
  }
  if (!strcmp(name, "readonly"))
    v.read_only = 1;
  if (!strcmp(name, "clean"))
    v.dirty = 0;
  CHECK(umi_ui_document_view_model_upsert(views, &v) == UMI_STATUS_OK);
  UmiUiDocumentTextInfo text_before, text_after;
  CHECK(UmiUiDocumentViewModelTextInfo(views, f.viewId, &text_before) == UMI_STATUS_OK);
  UmiDocumentWorkingCopySnapshot before, after;
  CHECK(umi_document_coordinator_active_snapshot(f.documents, &before) == UMI_STATUS_OK);
  UmiDocumentLanguageReview review;
  CHECK(UmiDocumentCoordinatorInspectLanguage(f.documents, f.id, &review) == UMI_STATUS_OK);
  CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &review, "python3", &review) ==
        UMI_STATUS_OK);
  CHECK(umi_document_coordinator_active_snapshot(f.documents, &after) == UMI_STATUS_OK);
  CHECK(before.revision == after.revision && before.dirty == after.dirty &&
        before.undo_count == after.undo_count && before.redo_count == after.redo_count);
  UmiUiDocumentViewSnapshot changed;
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &changed) == UMI_STATUS_OK);
  CHECK(changed.cursor_offset == v.cursor_offset &&
        changed.selection_length == v.selection_length && changed.read_only == v.read_only &&
        changed.dirty == v.dirty);
  CHECK(UmiUiDocumentViewModelTextInfo(views, f.viewId, &text_after) == UMI_STATUS_OK &&
        text_before.text_revision == text_after.text_revision);
  CHECK(ExpectText(&f, expected) == 0);
  if (!strcmp(name, "undo") || !strcmp(name, "redo") || !strcmp(name, "comment")) {
    CHECK(UmiDocumentCoordinatorToggleLanguageComment(f.documents, f.id) == UMI_STATUS_OK);
    CHECK(ExpectText(&f, "# alpha\nbeta") == 0);
    if (strcmp(name, "comment")) {
      CHECK(umi_document_coordinator_undo(f.documents) == UMI_STATUS_OK);
      CHECK(ExpectText(&f, expected) == 0);
    }
    if (!strcmp(name, "redo")) {
      CHECK(umi_document_coordinator_redo(f.documents) == UMI_STATUS_OK);
      CHECK(ExpectText(&f, "# alpha\nbeta") == 0);
    }
  }
  if (!strcmp(name, "policy-after-sync"))
    CHECK(umi_document_coordinator_sync_active(f.documents) == UMI_STATUS_OK);
  CHECK(UmiDocumentCoordinatorInspectLanguage(f.documents, f.id, &review) == UMI_STATUS_OK &&
        review.overridden && !strcmp(review.effective.language_id, "python3"));
  free(large);
  Stop(&f);
  return 0;
}
