/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_command_history.c
 *
 * PURPOSE:
 *   Verify auditable designer commands, result evidence and bounded
 *   success/failure history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerCommand command;
    UmiWorkbenchDesignerCommandResult result;
    UmiWorkbenchDesignerCommandHistory history;
    const UmiWorkbenchDesignerCommandRecord *record;
    uint64_t revision;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    revision = document.version.revision;
    umi_workbench_designer_command_init(
        &command, UMI_WORKBENCH_DESIGNER_COMMAND_SET_PROPERTY,
        "command.rename-editor");
    TEST_REQUIRE_STATUS(test_copy_text(
        command.target_node_id, sizeof(command.target_node_id), "editor"));
    TEST_REQUIRE_STATUS(test_copy_text(
        command.component_id, sizeof(command.component_id), "title"));
    TEST_REQUIRE_STATUS(test_copy_text(
        command.text_value, sizeof(command.text_value), "Source Editor"));
    command.expected_revision = revision;
    TEST_REQUIRE_STATUS(umi_workbench_designer_command_validate(&command));
    TEST_REQUIRE_STATUS(umi_workbench_designer_command_execute(
        &document, &command, &result));
    TEST_REQUIRE(result.document_changed);
    TEST_REQUIRE(document.version.revision > revision);

    umi_workbench_designer_command_history_init(&history);
    TEST_REQUIRE_STATUS(umi_workbench_designer_command_history_push(
        &history, &command, &result));
    TEST_REQUIRE(umi_workbench_designer_command_history_success_count(
        &history) == 1U);
    record = umi_workbench_designer_command_history_current(&history);
    TEST_REQUIRE(record != NULL);
    TEST_REQUIRE(strcmp(record->command.command_id,
                        "command.rename-editor") == 0);
    return 0;
}
