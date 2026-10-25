/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_indentation.c
 *
 * PURPOSE:
 *   Verify document indentation preferences preserve explicit overrides, selections, foreign
 * projections and Undo.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/indentation.h"
#include "umicom/document/line_edit.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1],
             *cases[] = {"defaults",         "policy",    "invalid",  "arguments",     "tab-spaces",
                         "tab-literal",      "selection", "outdent",  "mixed-outdent", "override",
                         "readonly",         "inactive",  "identity", "pending",       "command",
                         "command-argument", "closed"};
  int known = 0;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  CHECK(known);
  const char *source = "ab\nc", *expected = "ab  \nc";
  size_t cursor = 2U, selection = 0U;
  UmiEditorIndentation policy = {4U, 1};
  if (!strcmp(name, "tab-literal")) {
    policy.insert_spaces = 0;
    expected = "ab\t\nc";
  }
  if (!strcmp(name, "selection")) {
    cursor = 0U;
    selection = 4U;
    policy.tab_width = 2U;
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
    expected = "ab\nc";
    policy.insert_spaces = 0;
  }
  if (!strcmp(name, "override")) {
    cursor = 0U;
    expected = " ab\nc";
    policy.tab_width = 8U;
  }
  ReplacementFixture f = {0};
  CHECK(Start(&f, source) == 0);
  UmiEditorIndentation got = {0};
  CHECK(UmiDocumentCoordinatorGetIndentation(f.documents, &got) == UMI_STATUS_OK &&
        got.tab_width == 4U && got.insert_spaces == 1);
  if (!strcmp(name, "defaults")) {
    Stop(&f);
    return 0;
  }
  if (!strcmp(name, "invalid")) {
    UmiEditorIndentation bad = {0U, 1};
    CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, &bad) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorGetIndentation(f.documents, &got) == UMI_STATUS_OK &&
          got.tab_width == 4U);
    Stop(&f);
    return 0;
  }
  if (!strcmp(name, "arguments")) {
    CHECK(UmiDocumentCoordinatorGetIndentation(NULL, &got) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorGetIndentation(f.documents, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorInsertIndentation(f.documents, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorInsertIndentation(NULL, f.id) == UMI_STATUS_INVALID_ARGUMENT);
    Stop(&f);
    return 0;
  }
  CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, &policy) == UMI_STATUS_OK);
  if (!strcmp(name, "policy")) {
    policy.tab_width = 8U;
    CHECK(UmiDocumentCoordinatorGetIndentation(f.documents, &got) == UMI_STATUS_OK &&
          got.tab_width == 4U);
    CHECK(ExpectText(&f, source) == 0);
    Stop(&f);
    return 0;
  }
  if (!strcmp(name, "pending")) {
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
  if (!strcmp(name, "identity"))
    strcpy(view.document_id, "foreign");
  CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  UmiDocumentId other = 0U;
  if (!strcmp(name, "inactive")) {
    char id[UMI_UI_ID_CAPACITY];
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_new(f.documents, "other.c", id, sizeof id) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK);
    other = active.document_id;
  }
  if (!strcmp(name, "closed"))
    CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
  UmiStatus status;
  if (!strcmp(name, "command") || !strcmp(name, "command-argument")) {
    char message[256];
    CHECK(umi_document_commands_register(f.commands, f.documents) == UMI_STATUS_OK);
    status = umi_command_registry_execute(f.commands, UMI_DOCUMENT_COMMAND_INSERT_INDENTATION,
                                          !strcmp(name, "command-argument") ? "unexpected" : NULL,
                                          message, sizeof message);
  } else if (!strcmp(name, "override")) {
    UmiDocumentLineEditOptions explicitOptions = {" ", NULL};
    status = UmiDocumentCoordinatorEditLinesForDocument(
        f.documents, f.id, UMI_EDITOR_EDIT_COMMAND_INDENT_LINES, &explicitOptions);
  } else if (!strcmp(name, "outdent") || !strcmp(name, "mixed-outdent"))
    status = UmiDocumentCoordinatorEditLinesForDocument(
        f.documents, f.id, UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES, NULL);
  else
    status = UmiDocumentCoordinatorInsertIndentation(f.documents, f.id);
  UmiStatus wanted = !strcmp(name, "readonly")           ? UMI_STATUS_PERMISSION_DENIED
                     : !strcmp(name, "identity")         ? UMI_STATUS_INVALID_STATE
                     : !strcmp(name, "command-argument") ? UMI_STATUS_INVALID_ARGUMENT
                     : !strcmp(name, "closed")           ? UMI_STATUS_NOT_FOUND
                                                         : UMI_STATUS_OK;
  CHECK(status == wanted);
  if (wanted == UMI_STATUS_OK) {
    CHECK(ExpectText(&f, expected) == 0);
    if (other) {
      UmiDocumentWorkingCopySnapshot active;
      CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK &&
            active.document_id == other);
    }
    CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK &&
          ExpectText(&f, source) == 0);
    CHECK(UmiDocumentCoordinatorRedo(f.documents, f.id) == UMI_STATUS_OK &&
          ExpectText(&f, expected) == 0);
  } else if (strcmp(name, "closed"))
    CHECK(ExpectText(&f, source) == 0);
  Stop(&f);
  return 0;
}
