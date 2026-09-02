/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_commands_application.c
 *
 * PURPOSE:
 *   Verify all built-in Application command definitions are valid and unique
 *   within their command group.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_workbench/commands/application.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiDeveloperWorkbenchCommandDefinition *items =
        umi_developer_workbench_application_commands();
    const size_t count = umi_developer_workbench_application_command_count();
    size_t index;
    size_t other;

    assert(items != NULL);
    assert(count == 3U);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        assert(umi_developer_workbench_command_definition_validate(
            &items[index]) == UMI_STATUS_OK);

        /* Visit each bounded item once so every record receives the same rule. */
        for (other = index + 1U; other < count; ++other) {
            assert(strcmp(items[index].command_id,
                          items[other].command_id) != 0);
        }
    }

    return 0;
}
