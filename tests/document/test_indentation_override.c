/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_indentation_override.c
 *
 * PURPOSE:
 *   Verify captured policy revisions, isolation, no-op semantics and non-destructive overrides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/indentation_override.h"
static int SameHistory(const UmiDocumentWorkingCopySnapshot *a,
                       const UmiDocumentWorkingCopySnapshot *b) {
  return a->revision == b->revision && a->dirty == b->dirty && a->undo_count == b->undo_count &&
         a->redo_count == b->redo_count;
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1], *names[] = {"inherit",
                                          "override",
                                          "clear",
                                          "same-as-default",
                                          "default-change",
                                          "default-aba",
                                          "override-aba",
                                          "noop-default",
                                          "noop-override",
                                          "noop-clear",
                                          "stale-noop",
                                          "other-document",
                                          "closed",
                                          "foreign-owner",
                                          "foreign-view",
                                          "tampered-values",
                                          "invalid-width",
                                          "invalid-boolean",
                                          "arguments",
                                          "failed-output",
                                          "alias",
                                          "readonly",
                                          "text-does-not-stale",
                                          "history-unchanged",
                                          "close-reopen"};
  int known = 0;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(name, names[i]))
      known = 1;
  CHECK(known);
  ReplacementFixture f = {0};
  CHECK(Start(&f, "ab\nc") == 0);
  UmiDocumentIndentationReview initial, current, unchanged = {0};
  UmiEditorIndentation two = {2U, 1}, eight = {8U, 0}, four = {4U, 1};
  CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &initial) == UMI_STATUS_OK);
  CHECK(initial.owner == f.documents && initial.document_id == f.id && !initial.overridden &&
        initial.effective.tab_width == 4U && initial.effective.insert_spaces);
  if (!strcmp(name, "inherit")) {
    Stop(&f);
    return 0;
  }
  UmiDocumentWorkingCopySnapshot before, after;
  CHECK(umi_document_coordinator_active_snapshot(f.documents, &before) == UMI_STATUS_OK);
  if (!strcmp(name, "arguments")) {
    CHECK(UmiDocumentCoordinatorInspectIndentation(NULL, f.id, &current) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, 0U, &current) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorCheckIndentation(NULL, &initial) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorCheckIndentation(f.documents, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorApplyIndentation(NULL, &initial, &two, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, NULL, &two, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "failed-output")) {
    unchanged.document_id = 987U;
    current = unchanged;
    CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, UINT64_MAX, &current) ==
          UMI_STATUS_NOT_FOUND);
    CHECK(current.document_id == unchanged.document_id && current.owner == NULL);
    UmiDocumentIndentationReview altered = initial;
    ++altered.defaults_revision;
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &altered, &two, &current) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(current.document_id == unchanged.document_id && current.owner == NULL);
  } else if (!strcmp(name, "invalid-width") || !strcmp(name, "invalid-boolean")) {
    UmiEditorIndentation bad = !strcmp(name, "invalid-width") ? (UmiEditorIndentation){17U, 1}
                                                              : (UmiEditorIndentation){4U, 2};
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &initial, &bad, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorCheckIndentation(f.documents, &initial) == UMI_STATUS_OK);
  } else if (!strcmp(name, "foreign-owner")) {
    ReplacementFixture other = {0};
    CHECK(Start(&other, "other") == 0);
    CHECK(UmiDocumentCoordinatorApplyIndentation(other.documents, &initial, &two, NULL) ==
          UMI_STATUS_INVALID_STATE);
    Stop(&other);
  } else if (!strcmp(name, "foreign-view")) {
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    strcpy(view.document_id, "foreign");
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &current) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &initial, &two, NULL) ==
          UMI_STATUS_INVALID_STATE);
  } else if (!strcmp(name, "tampered-values")) {
    current = initial;
    current.effective.tab_width = 8U;
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &current, &two, NULL) ==
          UMI_STATUS_INVALID_STATE);
    current = initial;
    current.defaults.insert_spaces = 0;
    CHECK(UmiDocumentCoordinatorCheckIndentation(f.documents, &current) ==
          UMI_STATUS_INVALID_STATE);
    current = initial;
    current.overridden = 1;
    CHECK(UmiDocumentCoordinatorCheckIndentation(f.documents, &current) ==
          UMI_STATUS_INVALID_STATE);
  } else if (!strcmp(name, "closed") || !strcmp(name, "close-reopen")) {
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &initial, &two, &current) ==
          UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorClose(f.documents, f.id, 1) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorCheckIndentation(f.documents, &current) == UMI_STATUS_NOT_FOUND);
    if (!strcmp(name, "close-reopen")) {
      char view[UMI_UI_ID_CAPACITY];
      UmiDocumentWorkingCopySnapshot reopened;
      CHECK(umi_document_coordinator_new(f.documents, "review.c", view, sizeof view) ==
            UMI_STATUS_OK);
      CHECK(umi_document_coordinator_active_snapshot(f.documents, &reopened) == UMI_STATUS_OK);
      CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, reopened.document_id, &current) ==
            UMI_STATUS_OK);
      CHECK(!current.overridden && current.effective.tab_width == 4U);
    }
    Stop(&f);
    return 0;
  } else if (!strcmp(name, "noop-default")) {
    CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, &four) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorCheckIndentation(f.documents, &initial) == UMI_STATUS_OK);
  } else if (!strcmp(name, "noop-clear")) {
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &initial, NULL, &current) ==
          UMI_STATUS_OK);
    CHECK(current.override_revision == initial.override_revision && !current.overridden);
  } else if (!strcmp(name, "default-aba")) {
    CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, &eight) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, &four) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &initial, &two, NULL) ==
          UMI_STATUS_INVALID_STATE);
  } else {
    if (!strcmp(name, "readonly")) {
      UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
      UmiUiDocumentViewSnapshot view;
      CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
      view.read_only = 1;
      CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    }
    if (!strcmp(name, "text-does-not-stale")) {
      CHECK(Draft(&f, "new draft") == UMI_STATUS_OK);
      CHECK(UmiDocumentCoordinatorCheckIndentation(f.documents, &initial) == UMI_STATUS_OK);
    }
    UmiEditorIndentation candidate = !strcmp(name, "same-as-default") ? four : two;
    current = initial;
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &current, &candidate,
                                                 !strcmp(name, "alias") ? &current : &unchanged) ==
          UMI_STATUS_OK);
    if (strcmp(name, "alias"))
      current = unchanged;
    CHECK(current.overridden && current.effective.tab_width == candidate.tab_width &&
          current.override_revision == initial.override_revision + 1U);
    CHECK(UmiDocumentCoordinatorCheckIndentation(f.documents, &initial) ==
          UMI_STATUS_INVALID_STATE);
    if (!strcmp(name, "clear")) {
      CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &current, NULL, &current) ==
            UMI_STATUS_OK);
      CHECK(!current.overridden && current.effective.tab_width == 4U);
    } else if (!strcmp(name, "noop-override")) {
      unchanged = current;
      CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &current, &two, &current) ==
            UMI_STATUS_OK);
      CHECK(current.override_revision == unchanged.override_revision);
    } else if (!strcmp(name, "stale-noop")) {
      CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &initial, &two, NULL) ==
            UMI_STATUS_INVALID_STATE);
    } else if (!strcmp(name, "override-aba")) {
      unchanged = current;
      CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &current, &eight, &current) ==
            UMI_STATUS_OK);
      CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &current, &two, &current) ==
            UMI_STATUS_OK);
      CHECK(UmiDocumentCoordinatorCheckIndentation(f.documents, &unchanged) ==
            UMI_STATUS_INVALID_STATE);
    } else if (!strcmp(name, "default-change") || !strcmp(name, "same-as-default")) {
      CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, &eight) == UMI_STATUS_OK);
      CHECK(UmiDocumentCoordinatorCheckIndentation(f.documents, &current) ==
            UMI_STATUS_INVALID_STATE);
      CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &current) == UMI_STATUS_OK);
      CHECK(current.effective.tab_width == candidate.tab_width && current.defaults.tab_width == 8U);
      CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &current, NULL, &current) ==
            UMI_STATUS_OK);
      CHECK(!current.overridden && current.effective.tab_width == 8U &&
            !current.effective.insert_spaces);
    } else if (!strcmp(name, "other-document")) {
      char view[UMI_UI_ID_CAPACITY];
      UmiDocumentWorkingCopySnapshot other;
      UmiDocumentIndentationReview peer;
      CHECK(umi_document_coordinator_new(f.documents, "peer.c", view, sizeof view) ==
            UMI_STATUS_OK);
      CHECK(umi_document_coordinator_active_snapshot(f.documents, &other) == UMI_STATUS_OK);
      CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, other.document_id, &peer) ==
            UMI_STATUS_OK);
      CHECK(!peer.overridden && peer.effective.tab_width == 4U);
      CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &peer, &eight, NULL) ==
            UMI_STATUS_OK);
      CHECK(UmiDocumentCoordinatorCheckIndentation(f.documents, &current) == UMI_STATUS_OK);
    }
  }
  if (!strcmp(name, "history-unchanged")) {
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &after) == UMI_STATUS_OK);
    CHECK(SameHistory(&before, &after));
  }
  if (strcmp(name, "text-does-not-stale"))
    CHECK(ExpectText(&f, "ab\nc") == 0);
  Stop(&f);
  return 0;
}
