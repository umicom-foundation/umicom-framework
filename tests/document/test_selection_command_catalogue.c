/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_selection_command_catalogue.c
 *
 * PURPOSE:
 *   Check the active document catalogue exposes selection commands instead of merely preserving
 * disabled declarations.
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
  const char *name = argv[1];
  CHECK(strcmp(name, "count") == 0 || strcmp(name, "resolve") == 0 || strcmp(name, "sort") == 0 ||
        strcmp(name, "case") == 0 || strcmp(name, "unknown") == 0);
  ReplacementFixture f = {0};
  CHECK(Start(&f, "b\na\n") == 0);
  CHECK(umi_document_commands_register(f.commands, f.documents) == UMI_STATUS_OK);
  const char *ids[] = {
      UMI_DOCUMENT_COMMAND_SORT_LINES_ASCENDING, UMI_DOCUMENT_COMMAND_SORT_LINES_DESCENDING,
      UMI_DOCUMENT_COMMAND_UPPERCASE_SELECTION, UMI_DOCUMENT_COMMAND_LOWERCASE_SELECTION};
  const UmiEditorEditCommandKind kinds[] = {
      UMI_EDITOR_EDIT_COMMAND_SORT_LINES_ASCENDING, UMI_EDITOR_EDIT_COMMAND_SORT_LINES_DESCENDING,
      UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE, UMI_EDITOR_EDIT_COMMAND_TRANSFORM_LOWERCASE};
  if (strcmp(name, "count") == 0) {
    CHECK(umi_command_registry_count(f.commands) == UMI_DOCUMENT_COMMAND_COUNT);
    for (size_t i = 0U; i < sizeof ids / sizeof ids[0]; ++i) {
      UmiCommandSnapshot snapshot;
      CHECK(umi_command_registry_snapshot(f.commands, ids[i], &snapshot) == UMI_STATUS_OK);
      CHECK(snapshot.title[0] != '\0');
    }
  }
  if (strcmp(name, "resolve") == 0) {
    for (size_t i = 0U; i < sizeof ids / sizeof ids[0]; ++i) {
      UmiEditorEditCommandKind kind = UMI_EDITOR_EDIT_COMMAND_DELETE_LINE;
      CHECK(UmiDocumentLineEditCommandFromId(ids[i], &kind) == UMI_STATUS_OK && kind == kinds[i]);
    }
  }
  if (strcmp(name, "unknown") == 0) {
    UmiEditorEditCommandKind kind = UMI_EDITOR_EDIT_COMMAND_DELETE_LINE;
    CHECK(UmiDocumentLineEditCommandFromId("umicom.document.line-edit.uppercase-selection.extra",
                                           &kind) == UMI_STATUS_NOT_FOUND &&
          kind == UMI_EDITOR_EDIT_COMMAND_DELETE_LINE);
    CHECK(UmiDocumentLineEditCommandFromId(NULL, &kind) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentLineEditCommandFromId(ids[0], NULL) == UMI_STATUS_INVALID_ARGUMENT);
  }
  if (strcmp(name, "sort") == 0 || strcmp(name, "case") == 0) {
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    view.cursor_offset = 0U;
    view.selection_length = 4U;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    char message[256];
    const char *id = strcmp(name, "sort") == 0 ? ids[0] : ids[2];
    CHECK(umi_command_registry_execute(f.commands, id, NULL, message, sizeof message) ==
          UMI_STATUS_OK);
    CHECK(ExpectText(&f, strcmp(name, "sort") == 0 ? "a\nb\n" : "B\nA\n") == 0);
    CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK);
    CHECK(ExpectText(&f, "b\na\n") == 0);
  }
  Stop(&f);
  return 0;
}
