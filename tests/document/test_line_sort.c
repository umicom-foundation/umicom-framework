/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_line_sort.c
 *
 * PURPOSE:
 *   Verify line ordering uses document identity, preserves inactive tabs and records one reversible
 * edit.
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
             *cases[] = {"ascending", "descending", "partial", "unchanged", "empty-selection",
                         "readonly",  "other-tab",  "closed",  "identity",  "identity-unchanged",
                         "pending",   "command",    "argument"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(name, cases[i]) == 0)
      known = 1;
  CHECK(known);
  const char *source = "b\na\n", *expected = "a\nb\n";
  size_t start = 0U, selected = 4U;
  UmiEditorEditCommandKind kind = UMI_EDITOR_EDIT_COMMAND_SORT_LINES_ASCENDING;
  if (strcmp(name, "descending") == 0) {
    source = "a\nb\n";
    expected = "b\na\n";
    kind = UMI_EDITOR_EDIT_COMMAND_SORT_LINES_DESCENDING;
  }
  if (strcmp(name, "partial") == 0) {
    source = "z\nbee\nant\ny\n";
    expected = "z\nant\nbee\ny\n";
    start = 3U;
    selected = 5U;
  }
  if (strcmp(name, "unchanged") == 0 || strcmp(name, "identity-unchanged") == 0) {
    source = expected = "a\nb\n";
  }
  if (strcmp(name, "empty-selection") == 0)
    selected = 0U;
  ReplacementFixture f = {0};
  CHECK(Start(&f, source) == 0);
  if (strcmp(name, "pending") == 0) {
    source = "d\nc\n";
    expected = "c\nd\n";
    CHECK(Draft(&f, source) == UMI_STATUS_OK);
  }
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
  view.cursor_offset = start;
  view.selection_length = selected;
  view.read_only = strcmp(name, "readonly") == 0;
  if (strcmp(name, "identity") == 0 || strcmp(name, "identity-unchanged") == 0)
    strcpy(view.document_id, "foreign");
  CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  UmiDocumentWorkingCopySnapshot before;
  CHECK(umi_document_coordinator_active_snapshot(f.documents, &before) == UMI_STATUS_OK);
  if (strcmp(name, "closed") == 0)
    CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
  UmiDocumentId otherId = 0U;
  if (strcmp(name, "other-tab") == 0) {
    char other[UMI_UI_ID_CAPACITY];
    CHECK(umi_document_coordinator_new(f.documents, "other.c", other, sizeof other) ==
          UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK);
    otherId = active.document_id;
  }
  UmiStatus status;
  if (strcmp(name, "command") == 0 || strcmp(name, "argument") == 0) {
    CHECK(umi_document_commands_register(f.commands, f.documents) == UMI_STATUS_OK);
    char message[256];
    status = umi_command_registry_execute(f.commands, UMI_DOCUMENT_COMMAND_SORT_LINES_ASCENDING,
                                          strcmp(name, "argument") == 0 ? "unexpected" : NULL,
                                          message, sizeof message);
  } else
    status = UmiDocumentCoordinatorEditLinesForDocument(f.documents, f.id, kind, NULL);
  UmiStatus wanted = strcmp(name, "readonly") == 0 ? UMI_STATUS_PERMISSION_DENIED
                     : strcmp(name, "closed") == 0 || strcmp(name, "empty-selection") == 0
                         ? UMI_STATUS_NOT_FOUND
                     : strcmp(name, "argument") == 0 ? UMI_STATUS_INVALID_ARGUMENT
                     : (strcmp(name, "identity") == 0 || strcmp(name, "identity-unchanged") == 0)
                         ? UMI_STATUS_INVALID_STATE
                         : UMI_STATUS_OK;
  CHECK(status == wanted);
  if (wanted == UMI_STATUS_INVALID_STATE) {
    char *stored = NULL;
    size_t bytes = 0U;
    CHECK(umi_document_store_copy_text(f.store, f.id, &stored, &bytes) == UMI_STATUS_OK);
    CHECK(bytes == strlen(source) && memcmp(stored, source, bytes) == 0);
    umi_document_store_free_text(stored);
    CHECK(ExpectText(&f, source) == 0);
  }
  if (wanted == UMI_STATUS_OK) {
    CHECK(ExpectText(&f, expected) == 0);
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK);
    if (otherId != 0U)
      CHECK(active.document_id == otherId);
    if (strcmp(name, "unchanged") != 0) {
      CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK);
      CHECK(ExpectText(&f, source) == 0);
      UmiUiDocumentViewSnapshot restored;
      CHECK(umi_ui_document_view_model_find(views, f.viewId, &restored) == UMI_STATUS_OK);
      CHECK(restored.cursor_offset == start && restored.selection_length == selected);
      CHECK(UmiDocumentCoordinatorRedo(f.documents, f.id) == UMI_STATUS_OK);
      CHECK(ExpectText(&f, expected) == 0);
    } else
      CHECK(active.undo_count == before.undo_count);
  }
  Stop(&f);
  return 0;
}
