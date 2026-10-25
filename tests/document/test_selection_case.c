/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_selection_case.c
 *
 * PURPOSE:
 *   Qualify exact-selection case commands against canonical document identity, live drafts and Undo
 * ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/line_edit.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1],
             *cases[] = {"uppercase", "lowercase", "unicode",  "unchanged", "empty",
                         "readonly",  "inactive",  "closed",   "identity",  "pending",
                         "command",   "argument",  "selection"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(name, cases[i]) == 0)
      known = 1;
  CHECK(known);
  const char *source = "xAbcy", *expected = "xABCy";
  size_t start = 1U, selected = 3U;
  UmiEditorEditCommandKind kind = UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE;
  if (strcmp(name, "lowercase") == 0) {
    expected = "xabcy";
    kind = UMI_EDITOR_EDIT_COMMAND_TRANSFORM_LOWERCASE;
  }
  if (strcmp(name, "unicode") == 0) {
    source = "x\xc3\xa9"
             "by";
    expected = "x\xc3\xa9"
               "By";
  }
  if (strcmp(name, "unchanged") == 0)
    source = expected = "xABCy";
  if (strcmp(name, "empty") == 0)
    selected = 0U;
  ReplacementFixture f = {0};
  CHECK(Start(&f, source) == 0);
  if (strcmp(name, "pending") == 0) {
    source = "xDefy";
    expected = "xDEFy";
    CHECK(Draft(&f, source) == UMI_STATUS_OK);
  }
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
  view.cursor_offset = start;
  view.selection_length = selected;
  view.read_only = strcmp(name, "readonly") == 0;
  if (strcmp(name, "identity") == 0)
    strcpy(view.document_id, "foreign");
  CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  UmiDocumentWorkingCopySnapshot before;
  CHECK(umi_document_coordinator_active_snapshot(f.documents, &before) == UMI_STATUS_OK);
  if (strcmp(name, "closed") == 0)
    CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
  UmiDocumentId other = 0U;
  if (strcmp(name, "inactive") == 0) {
    char id[UMI_UI_ID_CAPACITY];
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_new(f.documents, "other.c", id, sizeof id) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK);
    other = active.document_id;
  }
  UmiStatus status;
  if (strcmp(name, "command") == 0 || strcmp(name, "argument") == 0) {
    char message[256];
    CHECK(umi_document_commands_register(f.commands, f.documents) == UMI_STATUS_OK);
    status = umi_command_registry_execute(f.commands, UMI_DOCUMENT_COMMAND_UPPERCASE_SELECTION,
                                          strcmp(name, "argument") == 0 ? "unexpected" : NULL,
                                          message, sizeof message);
  } else
    status = UmiDocumentCoordinatorEditLinesForDocument(f.documents, f.id, kind, NULL);
  UmiStatus wanted = strcmp(name, "readonly") == 0 ? UMI_STATUS_PERMISSION_DENIED
                     : strcmp(name, "closed") == 0 || strcmp(name, "empty") == 0
                         ? UMI_STATUS_NOT_FOUND
                     : strcmp(name, "identity") == 0 ? UMI_STATUS_INVALID_STATE
                     : strcmp(name, "argument") == 0 ? UMI_STATUS_INVALID_ARGUMENT
                                                     : UMI_STATUS_OK;
  CHECK(status == wanted);
  if (wanted == UMI_STATUS_OK) {
    CHECK(ExpectText(&f, expected) == 0);
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
          view.cursor_offset == start && view.selection_length == selected);
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK);
    if (other)
      CHECK(active.document_id == other);
    if (strcmp(source, expected) != 0) {
      CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK);
      CHECK(ExpectText(&f, source) == 0);
      CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
            view.cursor_offset == start && view.selection_length == selected);
      CHECK(UmiDocumentCoordinatorRedo(f.documents, f.id) == UMI_STATUS_OK);
      CHECK(ExpectText(&f, expected) == 0);
    } else
      CHECK(active.undo_count == before.undo_count);
  } else if (strcmp(name, "closed") != 0) {
    CHECK(ExpectText(&f, source) == 0);
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK &&
          active.undo_count == before.undo_count);
  }
  Stop(&f);
  return 0;
}
