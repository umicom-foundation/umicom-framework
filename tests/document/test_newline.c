/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_newline.c
 *
 * PURPOSE:
 *   Verify newline commands replace the intended draft selection with reversible history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/commands.h"
#include "umicom/document/newline.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  const char *cases[] = {"indent",           "plain",     "crlf",     "selection",
                         "unicode",          "readonly",  "identity", "inactive",
                         "closed",           "pending",   "command",  "plain-command",
                         "command-argument", "arguments", "bare-cr",  "cursor"};
  int known = 0;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  CHECK(known);
  const char *source = "  ab", *expected = "  ab\n  ";
  size_t cursor = 4U, selection = 0U;
  int inherit = 1;
  if (!strcmp(name, "plain") || !strcmp(name, "plain-command")) {
    inherit = 0;
    expected = "  ab\n";
  }
  if (!strcmp(name, "crlf")) {
    source = "  ab\r\nx";
    expected = "  ab\r\n  \r\nx";
  }
  if (!strcmp(name, "selection")) {
    source = "  abcdef";
    cursor = 3U;
    selection = 3U;
    expected = "  a\n  f";
  }
  if (!strcmp(name, "unicode")) {
    source = "  \xc3\xa9";
    expected = "  \xc3\xa9\n  ";
  }
  if (!strcmp(name, "bare-cr")) {
    source = "  a\rb";
    expected = source;
    cursor = 3U;
  }
  ReplacementFixture f = {0};
  CHECK(Start(&f, source) == 0);
  if (!strcmp(name, "arguments")) {
    CHECK(UmiDocumentCoordinatorInsertNewline(NULL, f.id, 1) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorInsertNewline(f.documents, 0U, 1) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorInsertNewline(f.documents, f.id, 2) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(ExpectText(&f, source) == 0);
    Stop(&f);
    return 0;
  }
  if (!strcmp(name, "pending")) {
    source = "  cd";
    expected = "  cd\n  ";
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
  if (!strcmp(name, "command") || !strcmp(name, "plain-command") ||
      !strcmp(name, "command-argument")) {
    char message[256];
    CHECK(umi_document_commands_register(f.commands, f.documents) == UMI_STATUS_OK);
    status = umi_command_registry_execute(
        f.commands,
        inherit ? UMI_DOCUMENT_COMMAND_INSERT_NEWLINE : UMI_DOCUMENT_COMMAND_INSERT_PLAIN_NEWLINE,
        !strcmp(name, "command-argument") ? "unexpected" : NULL, message, sizeof message);
  } else
    status = UmiDocumentCoordinatorInsertNewline(f.documents, f.id, inherit);
  UmiStatus wanted = !strcmp(name, "readonly")           ? UMI_STATUS_PERMISSION_DENIED
                     : !strcmp(name, "identity")         ? UMI_STATUS_INVALID_STATE
                     : !strcmp(name, "closed")           ? UMI_STATUS_NOT_FOUND
                     : !strcmp(name, "command-argument") ? UMI_STATUS_INVALID_ARGUMENT
                     : !strcmp(name, "bare-cr")          ? UMI_STATUS_NOT_IMPLEMENTED
                                                         : UMI_STATUS_OK;
  CHECK(status == wanted);
  if (wanted == UMI_STATUS_OK) {
    CHECK(ExpectText(&f, expected) == 0);
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
          view.selection_length == 0U);
    if (!strcmp(name, "cursor"))
      CHECK(view.cursor_offset == 7U);
    if (other) {
      UmiDocumentWorkingCopySnapshot active;
      CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK &&
            active.document_id == other);
    }
    CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK &&
          ExpectText(&f, source) == 0);
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
          view.cursor_offset == cursor && view.selection_length == selection);
    CHECK(UmiDocumentCoordinatorRedo(f.documents, f.id) == UMI_STATUS_OK &&
          ExpectText(&f, expected) == 0);
  } else if (strcmp(name, "closed"))
    CHECK(ExpectText(&f, source) == 0);
  Stop(&f);
  return 0;
}
