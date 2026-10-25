/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_language_comment.c
 *
 * PURPOSE:
 *   Check contextual comments, unchanged refusals, selected document identity and existing Undo
 * behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/language_comment.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1], *names[] = {"c",
                                          "python",
                                          "sql",
                                          "ini",
                                          "readonly",
                                          "unknown",
                                          "selection",
                                          "mixed",
                                          "uncomment",
                                          "blank",
                                          "crlf",
                                          "pending",
                                          "undo-redo",
                                          "inactive",
                                          "foreign-view",
                                          "closed",
                                          "arguments",
                                          "registry",
                                          "registry-argument"};
  int known = 0;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    known |= !strcmp(name, names[i]);
  if (!known)
    return 2;
  const char *source = "a\nb", *wanted = "// a\nb", *language = "c";
  if (!strcmp(name, "python")) {
    language = "python3";
    wanted = "# a\nb";
  }
  if (!strcmp(name, "sql")) {
    language = "sql";
    wanted = "-- a\nb";
  }
  if (!strcmp(name, "ini")) {
    language = "ini";
    wanted = "; a\nb";
  }
  if (!strcmp(name, "selection"))
    wanted = "// a\n// b";
  if (!strcmp(name, "mixed")) {
    source = "// a\nb";
    wanted = "// // a\n// b";
  }
  if (!strcmp(name, "uncomment")) {
    source = "// a\n// b";
    wanted = "a\nb";
  }
  if (!strcmp(name, "blank")) {
    source = " \n\t";
    wanted = source;
  }
  if (!strcmp(name, "crlf")) {
    source = "a\r\nb";
    wanted = "// a\r\n// b";
  }
  ReplacementFixture f = {0};
  CHECK(Start(&f, source) == 0);
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
  snprintf(view.language_id, sizeof view.language_id, "%s",
           !strcmp(name, "unknown") ? "json" : language);
  view.cursor_offset = 0U;
  if (!strcmp(name, "selection") || !strcmp(name, "mixed") || !strcmp(name, "uncomment") ||
      !strcmp(name, "crlf"))
    view.selection_length = strlen(source);
  if (!strcmp(name, "readonly"))
    view.read_only = 1;
  if (!strcmp(name, "foreign-view"))
    snprintf(view.document_id, sizeof view.document_id, "document.999");
  CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  if (!strcmp(name, "pending")) {
    CHECK(Draft(&f, "typed") == UMI_STATUS_OK);
    wanted = "// typed";
  }
  if (!strcmp(name, "inactive"))
    CHECK(umi_document_coordinator_new(f.documents, "other.py", NULL, 0U) == UMI_STATUS_OK);
  if (!strcmp(name, "closed"))
    CHECK(UmiDocumentCoordinatorClose(f.documents, f.id, 1) == UMI_STATUS_OK);
  if (!strcmp(name, "arguments")) {
    CHECK(UmiDocumentCoordinatorToggleLanguageComment(NULL, f.id) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorToggleLanguageComment(f.documents, 0U) ==
          UMI_STATUS_INVALID_ARGUMENT);
    Stop(&f);
    return 0;
  }
  UmiStatus status;
  if (!strcmp(name, "registry") || !strcmp(name, "registry-argument")) {
    CHECK(umi_document_commands_register(f.commands, f.documents) == UMI_STATUS_OK);
    CHECK(umi_command_registry_count(f.commands) == UMI_DOCUMENT_COMMAND_COUNT);
    char message[128];
    status = umi_command_registry_execute(f.commands, UMI_DOCUMENT_COMMAND_TOGGLE_LANGUAGE_COMMENT,
                                          !strcmp(name, "registry-argument") ? "no" : NULL, message,
                                          sizeof message);
    CHECK(message[0]);
  } else
    status = UmiDocumentCoordinatorToggleLanguageComment(f.documents, f.id);
  UmiStatus expected = !strcmp(name, "readonly") ? UMI_STATUS_PERMISSION_DENIED
                       : !strcmp(name, "unknown") || !strcmp(name, "closed") ? UMI_STATUS_NOT_FOUND
                       : !strcmp(name, "foreign-view")      ? UMI_STATUS_INVALID_STATE
                       : !strcmp(name, "registry-argument") ? UMI_STATUS_INVALID_ARGUMENT
                                                            : UMI_STATUS_OK;
  CHECK(status == expected);
  if (strcmp(name, "closed"))
    CHECK(ExpectText(&f, status == UMI_STATUS_OK ? wanted : source) == 0);
  if (!strcmp(name, "undo-redo")) {
    CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK);
    CHECK(ExpectText(&f, source) == 0);
    CHECK(UmiDocumentCoordinatorRedo(f.documents, f.id) == UMI_STATUS_OK);
    CHECK(ExpectText(&f, wanted) == 0);
  }
  if (!strcmp(name, "pending")) {
    CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK);
    CHECK(ExpectText(&f, "typed") == 0);
    CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK);
    CHECK(ExpectText(&f, source) == 0);
  }
  if (!strcmp(name, "inactive")) {
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK &&
          active.document_id != f.id);
  }
  Stop(&f);
  return 0;
}
