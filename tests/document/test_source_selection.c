/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_source_selection.c
 *
 * PURPOSE:
 *   Verify source selection preserves pending drafts, read-only state and text history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/source_selection.h"
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1],
             *cases[] = {"word",     "line",    "line-ending",        "readonly",  "pending",
                         "inactive", "history", "existing-selection", "no-word",   "empty-line",
                         "unicode",  "closed",  "arguments",          "catalogue", "invalid-kind"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    known |= !strcmp(name, cases[i]);
  if (!known)
    return 2;
  if (!strcmp(name, "catalogue")) {
    CHECK(UmiDocumentSourceSelectionCommandCount() == 3U &&
          UmiDocumentSourceSelectionCommandAt(3U) == NULL);
    for (size_t i = 0U; i < 3U; ++i) {
      const UmiDocumentSourceSelectionCommand *command = UmiDocumentSourceSelectionCommandAt(i);
      UmiEditorSourceSelectionKind kind = (UmiEditorSourceSelectionKind)99;
      CHECK(command && command->id && command->label &&
            UmiDocumentSourceSelectionCommandFromId(command->id, &kind) == UMI_STATUS_OK &&
            kind == command->kind);
    }
    UmiEditorSourceSelectionKind kind = (UmiEditorSourceSelectionKind)99;
    CHECK(UmiDocumentSourceSelectionCommandFromId("missing", &kind) == UMI_STATUS_NOT_FOUND &&
          kind == (UmiEditorSourceSelectionKind)99);
    CHECK(UmiDocumentSourceSelectionCommandFromId(NULL, &kind) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentSourceSelectionCommandFromId("umicom.document.select-source-word", NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  const char *source = !strcmp(name, "unicode")      ? "\xc3\xa9"
                                                       "clair beta"
                       : !strcmp(name, "no-word")    ? "  "
                       : !strcmp(name, "empty-line") ? ""
                                                     : "alpha beta\r\nnext";
  ReplacementFixture f = {0};
  CHECK(Start(&f, source) == 0);
  if (!strcmp(name, "pending")) {
    source = "typed draft\r\nnext";
    CHECK(Draft(&f, source) == UMI_STATUS_OK);
  }
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
  view.cursor_offset = !strcmp(name, "empty-line") ? 0U : !strcmp(name, "unicode") ? 2U : 1U;
  view.selection_length = !strcmp(name, "existing-selection") ? 8U : 0U;
  view.read_only = !strcmp(name, "readonly");
  CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  UmiDocumentWorkingCopySnapshot before, after;
  CHECK(umi_document_coordinator_active_snapshot(f.documents, &before) == UMI_STATUS_OK);
  if (!strcmp(name, "arguments")) {
    CHECK(UmiDocumentCoordinatorSelectSourceUnit(NULL, f.id, UMI_EDITOR_SELECT_SOURCE_WORD) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorSelectSourceUnit(f.documents, 0U, UMI_EDITOR_SELECT_SOURCE_WORD) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorSelectActiveSourceUnit(NULL, UMI_EDITOR_SELECT_SOURCE_WORD) ==
          UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "closed")) {
    CHECK(UmiDocumentCoordinatorClose(f.documents, f.id, 1) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorSelectSourceUnit(
              f.documents, f.id, UMI_EDITOR_SELECT_SOURCE_WORD) == UMI_STATUS_NOT_FOUND);
  } else {
    UmiEditorSourceSelectionKind kind =
        !strcmp(name, "line") || !strcmp(name, "empty-line") ? UMI_EDITOR_SELECT_LINE_CONTENT
        : !strcmp(name, "line-ending")                       ? UMI_EDITOR_SELECT_LINE_WITH_ENDING
                                                             : UMI_EDITOR_SELECT_SOURCE_WORD;
    if (!strcmp(name, "invalid-kind"))
      kind = (UmiEditorSourceSelectionKind)99;
    if (!strcmp(name, "inactive"))
      CHECK(umi_document_coordinator_new(f.documents, "other.c", NULL, 0U) == UMI_STATUS_OK);
    UmiStatus status = UmiDocumentCoordinatorSelectSourceUnit(f.documents, f.id, kind);
    if (!strcmp(name, "no-word") || !strcmp(name, "invalid-kind")) {
      CHECK(status ==
            (!strcmp(name, "no-word") ? UMI_STATUS_NOT_FOUND : UMI_STATUS_INVALID_ARGUMENT));
      CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
            view.cursor_offset == 1U && view.selection_length == 0U);
    } else {
      CHECK(status == UMI_STATUS_OK);
      CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
      size_t expected = !strcmp(name, "line")          ? 10U
                        : !strcmp(name, "line-ending") ? 12U
                        : !strcmp(name, "unicode")     ? 7U
                        : !strcmp(name, "empty-line")  ? 0U
                                                       : 5U;
      CHECK(view.cursor_offset == 0U && view.selection_length == expected);
      CHECK(umi_document_coordinator_active_snapshot(f.documents, &after) == UMI_STATUS_OK &&
            after.document_id == f.id);
      CHECK(after.undo_count == before.undo_count && after.revision == before.revision &&
            after.dirty == before.dirty);
      if (!strcmp(name, "history")) {
        CHECK(UmiDocumentCoordinatorSelectActiveSourceUnit(f.documents, kind) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &after) == UMI_STATUS_OK &&
              after.undo_count == before.undo_count);
      }
    }
    CHECK(ExpectText(&f, source) == 0);
  }
  Stop(&f);
  return 0;
}
