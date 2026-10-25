/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_indentation_override_edit.c
 *
 * PURPOSE:
 *   Verify named document overrides drive Tab and line operations through existing Undo history.
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
#include "umicom/document/line_edit.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1],
             *names[] = {"tab-spaces",     "tab-literal",    "selection", "outdent",
                         "mixed-outdent",  "explicit-token", "inactive",  "clear-inherits",
                         "default-change", "pending-draft",  "readonly",  "foreign-view",
                         "undo-redo"};
  int known = 0;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(name, names[i]))
      known = 1;
  CHECK(known);
  const char *source = "ab\nc", *expected = "ab  \nc";
  size_t cursor = 2U, selection = 0U;
  UmiEditorIndentation policy = {2U, 1}, global = {8U, 1};
  if (!strcmp(name, "tab-literal")) {
    policy.insert_spaces = 0;
    expected = "ab\t\nc";
  }
  if (!strcmp(name, "selection")) {
    cursor = 0U;
    selection = 4U;
    expected = "  ab\n  c";
  }
  if (!strcmp(name, "outdent")) {
    source = "\tab\nc";
    cursor = 1U;
    expected = "ab\nc";
    policy.insert_spaces = 0;
  }
  if (!strcmp(name, "mixed-outdent")) {
    source = "   ab\nc";
    cursor = 3U;
    expected = " ab\nc";
    policy.insert_spaces = 0;
  }
  if (!strcmp(name, "explicit-token")) {
    cursor = 0U;
    expected = " ab\nc";
  }
  if (!strcmp(name, "clear-inherits"))
    expected = "ab      \nc";
  ReplacementFixture f = {0};
  CHECK(Start(&f, source) == 0);
  CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, &global) == UMI_STATUS_OK);
  UmiDocumentIndentationReview review;
  CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &review) == UMI_STATUS_OK);
  CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &review, &policy, &review) ==
        UMI_STATUS_OK);
  if (!strcmp(name, "clear-inherits"))
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &review, NULL, NULL) ==
          UMI_STATUS_OK);
  if (!strcmp(name, "default-change")) {
    global.tab_width = 16U;
    CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, &global) == UMI_STATUS_OK);
  }
  if (!strcmp(name, "pending-draft")) {
    source = "xy\nc";
    expected = "xy  \nc";
    CHECK(Draft(&f, source) == UMI_STATUS_OK);
  }
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
  view.cursor_offset = cursor;
  view.selection_length = selection;
  view.read_only = !strcmp(name, "readonly");
  if (!strcmp(name, "foreign-view"))
    strcpy(view.document_id, "foreign");
  CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  UmiDocumentId active = 0U;
  if (!strcmp(name, "inactive")) {
    char other[UMI_UI_ID_CAPACITY];
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_new(f.documents, "peer.c", other, sizeof other) ==
          UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &snapshot) == UMI_STATUS_OK);
    active = snapshot.document_id;
  }
  UmiStatus status;
  if (!strcmp(name, "outdent") || !strcmp(name, "mixed-outdent"))
    status = UmiDocumentCoordinatorEditLinesForDocument(
        f.documents, f.id, UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES, NULL);
  else if (!strcmp(name, "explicit-token")) {
    UmiDocumentLineEditOptions explicitOptions = {" ", NULL};
    status = UmiDocumentCoordinatorEditLinesForDocument(
        f.documents, f.id, UMI_EDITOR_EDIT_COMMAND_INDENT_LINES, &explicitOptions);
  } else
    status = UmiDocumentCoordinatorInsertIndentation(f.documents, f.id);
  UmiStatus wanted = !strcmp(name, "readonly")       ? UMI_STATUS_PERMISSION_DENIED
                     : !strcmp(name, "foreign-view") ? UMI_STATUS_INVALID_STATE
                                                     : UMI_STATUS_OK;
  CHECK(status == wanted);
  CHECK(ExpectText(&f, wanted == UMI_STATUS_OK ? expected : source) == 0);
  if (wanted == UMI_STATUS_OK) {
    CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK &&
          ExpectText(&f, source) == 0);
    CHECK(UmiDocumentCoordinatorRedo(f.documents, f.id) == UMI_STATUS_OK &&
          ExpectText(&f, expected) == 0);
  }
  if (active) {
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &snapshot) == UMI_STATUS_OK &&
          snapshot.document_id == active);
  }
  Stop(&f);
  return 0;
}
