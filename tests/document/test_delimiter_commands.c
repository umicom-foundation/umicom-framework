/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_delimiter_commands.c
 * PURPOSE: Verify command-search registration and shared delimiter profile selection against authoritative drafts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/delimiter_navigation.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    const char *cases[] = {"match",         "contents",        "pair",        "metadata",
                           "prefix",        "argument",        "read-only",   "c-profile",
                           "json-profile",  "unknown-profile", "no-document", "active-invalid",
                           "message-bounds"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture fixture = {0};
    const char *text = "(one)", *language = "plaintext", *argument = NULL;
    size_t caret = 0U, expected_cursor = 4U, expected_selection = 0U;
    UmiStatus expected = UMI_STATUS_OK;
    const char *command = UMI_DOCUMENT_COMMAND_MATCH_DELIMITER;
    if (strcmp(name, "contents") == 0)
    {
        command = UMI_DOCUMENT_COMMAND_SELECT_DELIMITER_CONTENT;
        caret = 2U;
        expected_cursor = 1U;
        expected_selection = 3U;
    }
    if (strcmp(name, "pair") == 0)
    {
        command = UMI_DOCUMENT_COMMAND_SELECT_DELIMITER_PAIR;
        caret = 2U;
        expected_cursor = 0U;
        expected_selection = 5U;
    }
    if (strcmp(name, "c-profile") == 0 || strcmp(name, "unknown-profile") == 0)
    {
        text = "(\"}\")";
        language = strcmp(name, "c-profile") == 0 ? "c" : "cpp";
        if (strcmp(name, "unknown-profile") == 0)
        {
            expected = UMI_STATUS_NOT_FOUND;
            expected_cursor = 0U;
        }
    }
    if (strcmp(name, "json-profile") == 0)
    {
        text = "{\"x\":\"}\"}";
        language = "json";
        expected_cursor = 8U;
    }
    if (strcmp(name, "argument") == 0)
    {
        argument = "other.c";
        expected = UMI_STATUS_INVALID_ARGUMENT;
        expected_cursor = caret;
    }
    CHECK(Start(&fixture, text) == 0);
    CHECK(umi_document_commands_register(fixture.commands, fixture.documents) == UMI_STATUS_OK);
    CHECK(umi_command_registry_count(fixture.commands) == UMI_DOCUMENT_COMMAND_COUNT);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(fixture.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK);
    view.cursor_offset = caret;
    view.read_only = strcmp(name, "read-only") == 0;
    (void)snprintf(view.language_id, sizeof(view.language_id), "%s", language);
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    if (strcmp(name, "metadata") == 0)
    {
        UmiCommandSnapshot snapshot;
        CHECK(umi_command_registry_snapshot(fixture.commands, command, &snapshot) == UMI_STATUS_OK);
        CHECK(strcmp(snapshot.title, "Matching Bracket") == 0 && strcmp(snapshot.category, "Document") == 0 &&
              snapshot.description[0] != '\0');
    }
    if (strcmp(name, "prefix") == 0)
    {
        UmiCommandSnapshot items[3];
        size_t count = 0U;
        CHECK(umi_command_registry_find_prefix(fixture.commands, "umicom.document.select-delimiter-", items,
                                               3U, &count) == UMI_STATUS_OK &&
              count == 2U);
        CHECK(strcmp(items[0].command_id, items[1].command_id) != 0);
    }
    if (strcmp(name, "active-invalid") == 0)
    {
        CHECK(UmiDocumentCoordinatorNavigateActiveDelimiter(
                  fixture.documents, (UmiDocumentDelimiterAction)8) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorNavigateActiveDelimiter(NULL, UMI_DOCUMENT_DELIMITER_MATCH) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    char message[128];
    memset(message, '~', sizeof(message));
    size_t capacity = strcmp(name, "message-bounds") == 0 ? 4U : sizeof(message);
    if (strcmp(name, "no-document") == 0)
    {
        CHECK(umi_document_coordinator_close_active(fixture.documents, 1) == UMI_STATUS_OK);
        CHECK(umi_command_registry_execute(fixture.commands, command, NULL, message, capacity) ==
              UMI_STATUS_NOT_FOUND);
        Stop(&fixture);
        return 0;
    }
    UmiDocumentWorkingCopySnapshot before, after;
    CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &before) == UMI_STATUS_OK);
    CHECK(umi_command_registry_execute(fixture.commands, command, argument, message, capacity) == expected);
    CHECK(memchr(message, '\0', capacity) != NULL);
    if (capacity == 4U)
        CHECK(message[4] == '~');
    CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK &&
          view.cursor_offset == expected_cursor && view.selection_length == expected_selection);
    CHECK(ExpectText(&fixture, text) == 0);
    CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &after) == UMI_STATUS_OK);
    CHECK(before.revision == after.revision && before.undo_count == after.undo_count &&
          before.dirty == after.dirty);
    if (strcmp(name, "match") == 0)
    {
        CHECK(umi_command_registry_execute(fixture.commands, command, "", message, sizeof(message)) ==
              UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK &&
              view.cursor_offset == caret);
    }
    Stop(&fixture);
    return 0;
}
