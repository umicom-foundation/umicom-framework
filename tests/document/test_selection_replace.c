/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_selection_replace.c
 *
 * PURPOSE:
 *   Verify selection-limited proposals preserve complete text, pending typing and document edit
 * authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/selection_replace.h"
static UmiStatus Selection(ReplacementFixture *f, size_t start, size_t count, int readonly) {
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
  UmiUiDocumentViewSnapshot view;
  UmiStatus status = umi_ui_document_view_model_find(views, f->viewId, &view);
  if (status == UMI_STATUS_OK) {
    view.cursor_offset = start;
    view.selection_length = count;
    view.read_only = readonly;
    status = umi_ui_document_view_model_upsert(views, &view);
  }
  return status;
}
static int Comparison(UmiDocumentProposal *proposal, const char *before, const char *after) {
  const char *left = NULL, *right = NULL;
  size_t left_bytes = 0U, right_bytes = 0U;
  CHECK(UmiDocumentProposalTexts(proposal, &left, &left_bytes, &right, &right_bytes) ==
        UMI_STATUS_OK);
  CHECK(left_bytes == strlen(before) && right_bytes == strlen(after) && !strcmp(left, before) &&
        !strcmp(right, after));
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1];
  const char *cases[] = {"capture",
                         "apply",
                         "undo",
                         "pending-typing",
                         "delete",
                         "no-match",
                         "whole-word-left",
                         "whole-word-right",
                         "smart-case",
                         "unicode",
                         "crlf",
                         "empty-selection",
                         "read-only",
                         "bad-needle",
                         "bad-replacement",
                         "split-utf8",
                         "split-crlf",
                         "foreign-view",
                         "stale",
                         "closed",
                         "other-owner",
                         "inactive",
                         "caret",
                         "approval",
                         "review-revision",
                         "arguments",
                         "invalid-policy",
                         "owned"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  if (!known)
    return 2;
  const char *source = "old old old", *expected = "old new old", *needle = "old",
             *replacement = "new";
  size_t start = 4U, span = 3U, expected_count = 1U;
  UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SMART, 0, 0, 0U};
  if (!strcmp(name, "delete")) {
    replacement = "";
    expected = "old  old";
  }
  if (!strcmp(name, "no-match")) {
    needle = "absent";
    expected = source;
    expected_count = 0U;
  }
  if (!strcmp(name, "whole-word-left")) {
    source = "xold old";
    start = 1U;
    expected = source;
    expected_count = 0U;
    options.whole_word = 1;
  }
  if (!strcmp(name, "whole-word-right")) {
    source = "oldx old";
    start = 0U;
    expected = source;
    expected_count = 0U;
    options.whole_word = 1;
  }
  if (!strcmp(name, "smart-case")) {
    source = "Old OLD old";
    span = 7U;
    expected = "Old new new";
    expected_count = 2U;
  }
  if (!strcmp(name, "unicode")) {
    source = "\xc3\xa9 old \xc3\xa9";
    start = 3U;
    expected = "\xc3\xa9 new \xc3\xa9";
  }
  if (!strcmp(name, "crlf")) {
    source = "old\r\nold\r\nold";
    start = 5U;
    expected = "old\r\nnew\r\nold";
  }
  if (!strcmp(name, "split-utf8")) {
    source = "\xc3\xa9 old";
    start = 1U;
    span = 1U;
  }
  if (!strcmp(name, "split-crlf")) {
    source = "old\r\nold";
    start = 4U;
    span = 4U;
  }
  if (!strcmp(name, "empty-selection"))
    span = 0U;
  ReplacementFixture f = {0};
  CHECK(Start(&f, source) == 0);
  if (!strcmp(name, "pending-typing")) {
    source = "old old pending";
    expected = "old new pending";
    CHECK(Draft(&f, source) == UMI_STATUS_OK);
  }
  CHECK(Selection(&f, start, span, !strcmp(name, "read-only")) == UMI_STATUS_OK);
  if (!strcmp(name, "foreign-view")) {
    UmiUiDocumentViewSnapshot view;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    snprintf(view.document_id, sizeof view.document_id, "document.foreign");
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  }
  if (!strcmp(name, "bad-needle"))
    needle = "\xc0\xaf";
  if (!strcmp(name, "bad-replacement"))
    replacement = "\xc0\xaf";
  if (!strcmp(name, "invalid-policy"))
    options.whole_word = 7;
  UmiDocumentProposal *proposal = NULL;
  size_t count = 99U;
  if (!strcmp(name, "arguments")) {
    CHECK(UmiDocumentPrepareSelectionReplacement(NULL, f.id, needle, replacement, &options,
                                                 &proposal, &count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentPrepareSelectionReplacement(f.documents, 0U, needle, replacement, &options,
                                                 &proposal, &count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentPrepareSelectionReplacement(f.documents, f.id, "", replacement, &options,
                                                 &proposal, &count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentPrepareSelectionReplacement(f.documents, f.id, needle, NULL, &options,
                                                 &proposal, &count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentPrepareSelectionReplacement(f.documents, f.id, needle, replacement, &options,
                                                 NULL, &count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(proposal == NULL && count == 99U);
    Stop(&f);
    return 0;
  }
  UmiStatus status = UmiDocumentPrepareSelectionReplacement(f.documents, f.id, needle, replacement,
                                                            &options, &proposal, &count);
  if (!strcmp(name, "read-only") || !strcmp(name, "empty-selection") ||
      !strcmp(name, "bad-needle") || !strcmp(name, "bad-replacement") ||
      !strcmp(name, "split-utf8") || !strcmp(name, "split-crlf") || !strcmp(name, "foreign-view") ||
      !strcmp(name, "invalid-policy")) {
    CHECK(status != UMI_STATUS_OK && proposal == NULL && count == 99U);
    CHECK(ExpectText(&f, source) == 0);
    Stop(&f);
    return 0;
  }
  CHECK(status == UMI_STATUS_OK && proposal != NULL && count == expected_count);
  CHECK(Comparison(proposal, source, expected) == 0 && ExpectText(&f, source) == 0);
  UmiDocumentProposalSummary summary;
  CHECK(UmiDocumentProposalInspect(proposal, &summary) == UMI_STATUS_OK);
  CHECK(summary.has_proposal && summary.selection_offset == start &&
        summary.selection_bytes == span);
  if (!strcmp(name, "stale") || !strcmp(name, "owned")) {
    CHECK(Draft(&f, "later contents") == UMI_STATUS_OK);
    CHECK(Comparison(proposal, source, expected) == 0);
    CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(ExpectText(&f, "later contents") == 0);
  } else if (!strcmp(name, "closed")) {
    CHECK(UmiDocumentCoordinatorClose(f.documents, f.id, 1) == UMI_STATUS_OK);
    CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) ==
          UMI_STATUS_NOT_FOUND);
  } else if (!strcmp(name, "other-owner")) {
    ReplacementFixture other = {0};
    CHECK(Start(&other, source) == 0);
    CHECK(UmiDocumentProposalApply(other.documents, proposal, summary.revision, 1) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(ExpectText(&other, source) == 0);
    Stop(&other);
  } else if (!strcmp(name, "approval")) {
    CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 0) ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(ExpectText(&f, source) == 0);
  } else if (!strcmp(name, "review-revision")) {
    CHECK(UmiDocumentProposalSetText(proposal, summary.revision, "different", 9U) == UMI_STATUS_OK);
    CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) == UMI_STATUS_BUSY);
    CHECK(ExpectText(&f, source) == 0);
  } else if (strcmp(name, "capture")) {
    char other_view[UMI_UI_ID_CAPACITY] = {0};
    if (!strcmp(name, "inactive"))
      CHECK(umi_document_coordinator_new(f.documents, "other.c", other_view, sizeof other_view) ==
            UMI_STATUS_OK);
    if (!strcmp(name, "caret"))
      CHECK(Selection(&f, 0U, 1U, 0) == UMI_STATUS_OK);
    CHECK(UmiDocumentProposalApply(f.documents, proposal, summary.revision, 1) == UMI_STATUS_OK);
    CHECK(ExpectText(&f, expected) == 0);
    if (!strcmp(name, "inactive")) {
      UmiDocumentWorkingCopySnapshot active;
      CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK &&
            !strcmp(active.view_id, other_view));
    }
    if (!strcmp(name, "undo") || !strcmp(name, "pending-typing") || !strcmp(name, "delete")) {
      CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK);
      CHECK(ExpectText(&f, source) == 0);
    }
  }
  UmiDocumentProposalDestroy(proposal);
  Stop(&f);
  return 0;
}
