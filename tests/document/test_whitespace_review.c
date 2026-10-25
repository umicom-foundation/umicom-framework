/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_whitespace_review.c
 *
 * PURPOSE:
 *   Exercise reviewed whitespace identity, pending typing, source preservation and Undo/Redo.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/whitespace.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  const char *names[] = {"trim",           "expand",       "compact",       "selection",
                         "caret-move",     "inactive",     "pending-draft", "undo-redo",
                         "unchanged",      "stale-text",   "stale-aba",     "readonly-create",
                         "readonly-apply", "foreign-view", "foreign-owner", "closed",
                         "declined",       "repeat",       "arguments",     "owned-text"};
  int known = 0;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(name, names[i]))
      known = 1;
  CHECK(known);
  const char *source = "a \nb\t\n", *expected = "a\nb\n";
  UmiEditorWhitespaceOptions options = {UMI_EDITOR_WHITESPACE_TRIM_TRAILING, 4U, 0};
  if (!strcmp(name, "expand")) {
    source = "\tx";
    expected = "    x";
    options.operation = UMI_EDITOR_WHITESPACE_EXPAND_LEADING_TABS;
  }
  if (!strcmp(name, "compact")) {
    source = "    x";
    expected = "\tx";
    options.operation = UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES;
  }
  if (!strcmp(name, "unchanged")) {
    source = "a\n";
    expected = source;
  }
  ReplacementFixture f = {0};
  CHECK(Start(&f, source) == 0);
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
  if (!strcmp(name, "selection")) {
    view.cursor_offset = 3U;
    view.selection_length = 1U;
    options.selected_lines = 1;
    expected = "a \nb\n";
  }
  if (!strcmp(name, "readonly-create"))
    view.read_only = 1;
  if (!strcmp(name, "foreign-view"))
    strcpy(view.document_id, "foreign");
  CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  if (!strcmp(name, "pending-draft")) {
    source = "pending \n";
    expected = "pending\n";
    CHECK(Draft(&f, source) == UMI_STATUS_OK);
  }
  UmiDocumentWhitespaceReview *review = NULL;
  UmiStatus status = UmiDocumentWhitespaceReviewCreate(f.documents, f.id, &options, &review);
  if (!strcmp(name, "readonly-create") || !strcmp(name, "foreign-view")) {
    CHECK(status == (!strcmp(name, "readonly-create") ? UMI_STATUS_PERMISSION_DENIED
                                                      : UMI_STATUS_INVALID_STATE) &&
          review == NULL);
    Stop(&f);
    return 0;
  }
  CHECK(status == UMI_STATUS_OK && review != NULL);
  const char *before, *after;
  size_t n, m;
  CHECK(UmiDocumentWhitespaceReviewTexts(review, &before, &n, &after, &m) == UMI_STATUS_OK);
  CHECK(n == strlen(source) && m == strlen(expected) && !strcmp(before, source) &&
        !strcmp(after, expected));
  UmiDocumentWhitespaceSummary summary;
  CHECK(UmiDocumentWhitespaceReviewInspect(review, &summary) == UMI_STATUS_OK &&
        summary.document_id == f.id && !summary.applied);
  if (!strcmp(name, "arguments")) {
    UmiDocumentWhitespaceReview *missing = (void *)1;
    CHECK(UmiDocumentWhitespaceReviewCreate(NULL, f.id, &options, &missing) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          missing == NULL);
    CHECK(UmiDocumentWhitespaceReviewCheck(NULL, review) == UMI_STATUS_INVALID_ARGUMENT);
    UmiDocumentWhitespaceReviewDestroy(review);
    Stop(&f);
    return 0;
  }
  if (!strcmp(name, "owned-text")) {
    CHECK(Draft(&f, "different") == UMI_STATUS_OK);
    CHECK(!strcmp(before, source) && !strcmp(after, expected));
    UmiDocumentWhitespaceReviewDestroy(review);
    Stop(&f);
    return 0;
  }
  UmiStatus wanted = UMI_STATUS_OK;
  if (!strcmp(name, "stale-text") || !strcmp(name, "stale-aba")) {
    CHECK(Draft(&f, "different") == UMI_STATUS_OK);
    if (!strcmp(name, "stale-aba"))
      CHECK(Draft(&f, source) == UMI_STATUS_OK);
    wanted = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(name, "readonly-apply")) {
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    view.read_only = 1;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    wanted = UMI_STATUS_PERMISSION_DENIED;
  }
  if (!strcmp(name, "caret-move")) {
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    view.cursor_offset = strlen(source);
    view.selection_length = 0U;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  }
  UmiDocumentId peer = 0U;
  if (!strcmp(name, "inactive")) {
    char other[UMI_UI_ID_CAPACITY];
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_new(f.documents, "peer.c", other, sizeof other) ==
          UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &snapshot) == UMI_STATUS_OK);
    peer = snapshot.document_id;
  }
  if (!strcmp(name, "closed")) {
    CHECK(UmiDocumentCoordinatorClose(f.documents, f.id, 1) == UMI_STATUS_OK);
    wanted = UMI_STATUS_NOT_FOUND;
  }
  if (!strcmp(name, "declined"))
    wanted = UMI_STATUS_PERMISSION_DENIED;
  ReplacementFixture other = {0};
  UmiDocumentCoordinator *owner = f.documents;
  if (!strcmp(name, "foreign-owner")) {
    CHECK(Start(&other, "other") == 0);
    owner = other.documents;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  status = UmiDocumentWhitespaceReviewApply(owner, review, strcmp(name, "declined") != 0);
  CHECK(status == wanted);
  if (other.documents)
    Stop(&other);
  CHECK(UmiDocumentWhitespaceReviewInspect(review, &summary) == UMI_STATUS_OK &&
        summary.applied == (wanted == UMI_STATUS_OK));
  if (wanted == UMI_STATUS_OK) {
    CHECK(ExpectText(&f, expected) == 0);
    if (strcmp(source, expected)) {
      CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK &&
            ExpectText(&f, source) == 0);
      CHECK(UmiDocumentCoordinatorRedo(f.documents, f.id) == UMI_STATUS_OK &&
            ExpectText(&f, expected) == 0);
    }
    CHECK(UmiDocumentWhitespaceReviewApply(f.documents, review, 1) == UMI_STATUS_ALREADY_EXISTS);
  }
  if (peer) {
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK &&
          active.document_id == peer);
  }
  CHECK(UmiDocumentWhitespaceReviewTexts(review, &before, &n, &after, &m) == UMI_STATUS_OK &&
        !strcmp(before, source) && !strcmp(after, expected));
  UmiDocumentWhitespaceReviewDestroy(review);
  Stop(&f);
  return 0;
}
