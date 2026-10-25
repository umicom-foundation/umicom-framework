/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_line_edit_commands.c
 * PURPOSE: Check line-command discovery, explicit argument rules and shared Undo behavior.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1], *cases[] = {"delete-line",
                                          "duplicate-line",
                                          "move-line-up",
                                          "move-line-down",
                                          "join-line-with-next",
                                          "trim-trailing-whitespace",
                                          "indent-lines",
                                          "outdent-lines",
                                          "toggle-line-comment",
                                          "metadata",
                                          "prefix",
                                          "argument",
                                          "empty-argument",
                                          "message-bounds",
                                          "read-only",
                                          "no-document",
                                          "mapping",
                                          "mapping-unknown",
                                          "mapping-null"};
  int known = 0;
  for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(name, cases[i]) == 0)
      known = 1;
  CHECK(known);
  static const struct {
    const char *name, *id, *source, *expected;
  } rows[] = {
      {"delete-line", UMI_DOCUMENT_COMMAND_DELETE_LINE, "a\nbb\nccc", "a\nccc"},
      {"duplicate-line", UMI_DOCUMENT_COMMAND_DUPLICATE_LINE, "a\nbb\nccc", "a\nbb\nbb\nccc"},
      {"move-line-up", UMI_DOCUMENT_COMMAND_MOVE_LINE_UP, "a\nbb\nccc", "bb\na\nccc"},
      {"move-line-down", UMI_DOCUMENT_COMMAND_MOVE_LINE_DOWN, "a\nbb\nccc", "a\nccc\nbb"},
      {"join-line-with-next", UMI_DOCUMENT_COMMAND_JOIN_LINE_WITH_NEXT, "a\nbb\nccc", "a\nbb ccc"},
      {"trim-trailing-whitespace", UMI_DOCUMENT_COMMAND_TRIM_TRAILING_WHITESPACE, "a\nbb \nccc\t",
       "a\nbb\nccc"},
      {"indent-lines", UMI_DOCUMENT_COMMAND_INDENT_LINES, "a\nbb\nccc", "a\n    bb\nccc"},
      {"outdent-lines", UMI_DOCUMENT_COMMAND_OUTDENT_LINES, "a\n  bb\nccc", "a\nbb\nccc"},
      {"toggle-line-comment", UMI_DOCUMENT_COMMAND_TOGGLE_LINE_COMMENT, "a\nbb\nccc",
       "a\n// bb\nccc"}};
  if (strncmp(name, "mapping", 7U) == 0) {
    UmiEditorEditCommandKind mapped = UMI_EDITOR_EDIT_COMMAND_INSERT_TEXT;
    if (strcmp(name, "mapping-null") == 0) {
      CHECK(UmiDocumentLineEditCommandFromId(NULL, &mapped) == UMI_STATUS_INVALID_ARGUMENT);
      CHECK(UmiDocumentLineEditCommandFromId(UMI_DOCUMENT_COMMAND_DUPLICATE_LINE, NULL) ==
            UMI_STATUS_INVALID_ARGUMENT);
      CHECK(mapped == UMI_EDITOR_EDIT_COMMAND_INSERT_TEXT);
    } else if (strcmp(name, "mapping-unknown") == 0) {
      const char *unknown[] = {"", "umicom.document.line-edit.",
                               "umicom.document.line-edit.duplicate-line.extra", "edit.undo",
                               "UMICOM.DOCUMENT.LINE-EDIT.DUPLICATE-LINE"};
      for (size_t i = 0U; i < sizeof unknown / sizeof unknown[0]; ++i) {
        CHECK(UmiDocumentLineEditCommandFromId(unknown[i], &mapped) == UMI_STATUS_NOT_FOUND);
        CHECK(mapped == UMI_EDITOR_EDIT_COMMAND_INSERT_TEXT);
      }
    } else {
      const UmiEditorEditCommandKind kinds[] = {UMI_EDITOR_EDIT_COMMAND_DELETE_LINE,
                                                UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE,
                                                UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP,
                                                UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN,
                                                UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT,
                                                UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE,
                                                UMI_EDITOR_EDIT_COMMAND_INDENT_LINES,
                                                UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES,
                                                UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT};
      for (size_t i = 0U; i < sizeof rows / sizeof rows[0]; ++i) {
        CHECK(UmiDocumentLineEditCommandFromId(rows[i].id, &mapped) == UMI_STATUS_OK);
        CHECK(mapped == kinds[i]);
      }
    }
    return 0;
  }
  size_t chosen = 1U;
  for (size_t i = 0U; i < sizeof(rows) / sizeof(rows[0]); ++i)
    if (strcmp(name, rows[i].name) == 0)
      chosen = i;
  ReplacementFixture f = {0};
  CHECK(Start(&f, rows[chosen].source) == 0);
  CHECK(umi_document_commands_register(f.commands, f.documents) == UMI_STATUS_OK);
  CHECK(umi_command_registry_count(f.commands) == UMI_DOCUMENT_COMMAND_COUNT);
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
  view.cursor_offset = 2U;
  view.read_only = strcmp(name, "read-only") == 0;
  CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  if (strcmp(name, "metadata") == 0) {
    for (size_t i = 0U; i < sizeof(rows) / sizeof(rows[0]); ++i) {
      UmiCommandSnapshot snapshot;
      CHECK(umi_command_registry_snapshot(f.commands, rows[i].id, &snapshot) == UMI_STATUS_OK &&
            snapshot.title[0] != '\0' && snapshot.description[0] != '\0' &&
            strcmp(snapshot.category, "Document") == 0);
    }
  }
  if (strcmp(name, "prefix") == 0) {
    /* The catalogue now includes four selection commands. Preserve the
     * original nine-only assertion and verify all registered entries fit. */
#if 0
        UmiCommandSnapshot items[9];
        size_t count = 0U;
        CHECK(umi_command_registry_find_prefix(f.commands, "umicom.document.line-edit.", items, 9U, &count) ==
                  UMI_STATUS_OK &&
              count == 9U);
#endif
    /* Keep the previous selection-only catalogue assertion for review.
     * Contextual commenting adds a distinct command without changing literals. */
#if 0
        UmiCommandSnapshot items[13];
        size_t count = 0U;
        CHECK(umi_command_registry_find_prefix(f.commands, "umicom.document.line-edit.", items, 13U, &count) ==
                  UMI_STATUS_OK && count == 13U);
#endif
    UmiCommandSnapshot items[14];
    size_t count = 0U;
    CHECK(umi_command_registry_find_prefix(f.commands, "umicom.document.line-edit.", items, 14U,
                                           &count) == UMI_STATUS_OK &&
          count == 14U);
    CHECK(umi_command_registry_contains(f.commands, UMI_DOCUMENT_COMMAND_TOGGLE_LANGUAGE_COMMENT));
    for (size_t i = 0U; i < sizeof rows / sizeof rows[0]; ++i)
      CHECK(umi_command_registry_contains(f.commands, rows[i].id));
  }
  const char *argument = strcmp(name, "argument") == 0         ? "other.c"
                         : strcmp(name, "empty-argument") == 0 ? ""
                                                               : NULL;
  UmiStatus wanted = strcmp(name, "argument") == 0    ? UMI_STATUS_INVALID_ARGUMENT
                     : strcmp(name, "read-only") == 0 ? UMI_STATUS_PERMISSION_DENIED
                                                      : UMI_STATUS_OK;
  if (strcmp(name, "no-document") == 0) {
    CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
    wanted = UMI_STATUS_NOT_FOUND;
  }
  char message[180];
  memset(message, '~', sizeof(message));
  size_t capacity = strcmp(name, "message-bounds") == 0 ? 4U : sizeof(message);
  CHECK(umi_command_registry_execute(f.commands, rows[chosen].id, argument, message, capacity) ==
        wanted);
  CHECK(memchr(message, '\0', capacity) != NULL);
  if (capacity == 4U)
    CHECK(message[4] == '~');
  if (strcmp(name, "no-document") != 0) {
    CHECK(ExpectText(&f, wanted == UMI_STATUS_OK ? rows[chosen].expected : rows[chosen].source) ==
          0);
    if (wanted == UMI_STATUS_OK)
      CHECK(umi_document_coordinator_undo(f.documents) == UMI_STATUS_OK &&
            ExpectText(&f, rows[chosen].source) == 0);
  }
  Stop(&f);
  return 0;
}
