/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_line_edit_commands.c
 * PURPOSE: Check line-command discovery, explicit argument rules and shared Undo behavior.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
int main(int argc, char **argv)
{
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
                                            "no-document"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    CHECK(known);
    static const struct
    {
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
        {"toggle-line-comment", UMI_DOCUMENT_COMMAND_TOGGLE_LINE_COMMENT, "a\nbb\nccc", "a\n// bb\nccc"}};
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
    if (strcmp(name, "metadata") == 0)
    {
        for (size_t i = 0U; i < sizeof(rows) / sizeof(rows[0]); ++i)
        {
            UmiCommandSnapshot snapshot;
            CHECK(umi_command_registry_snapshot(f.commands, rows[i].id, &snapshot) == UMI_STATUS_OK &&
                  snapshot.title[0] != '\0' && snapshot.description[0] != '\0' &&
                  strcmp(snapshot.category, "Document") == 0);
        }
    }
    if (strcmp(name, "prefix") == 0)
    {
        UmiCommandSnapshot items[9];
        size_t count = 0U;
        CHECK(umi_command_registry_find_prefix(f.commands, "umicom.document.line-edit.", items, 9U, &count) ==
                  UMI_STATUS_OK &&
              count == 9U);
    }
    const char *argument = strcmp(name, "argument") == 0         ? "other.c"
                           : strcmp(name, "empty-argument") == 0 ? ""
                                                                 : NULL;
    UmiStatus wanted = strcmp(name, "argument") == 0    ? UMI_STATUS_INVALID_ARGUMENT
                       : strcmp(name, "read-only") == 0 ? UMI_STATUS_PERMISSION_DENIED
                                                        : UMI_STATUS_OK;
    if (strcmp(name, "no-document") == 0)
    {
        CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
        wanted = UMI_STATUS_NOT_FOUND;
    }
    char message[180];
    memset(message, '~', sizeof(message));
    size_t capacity = strcmp(name, "message-bounds") == 0 ? 4U : sizeof(message);
    CHECK(umi_command_registry_execute(f.commands, rows[chosen].id, argument, message, capacity) == wanted);
    CHECK(memchr(message, '\0', capacity) != NULL);
    if (capacity == 4U)
        CHECK(message[4] == '~');
    if (strcmp(name, "no-document") != 0)
    {
        CHECK(ExpectText(&f, wanted == UMI_STATUS_OK ? rows[chosen].expected : rows[chosen].source) == 0);
        if (wanted == UMI_STATUS_OK)
            CHECK(umi_document_coordinator_undo(f.documents) == UMI_STATUS_OK &&
                  ExpectText(&f, rows[chosen].source) == 0);
    }
    Stop(&f);
    return 0;
}
