/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_commands_run.c
 *
 * PURPOSE:
 *   Verify all built-in Run command definitions are valid and unique
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

#include "umicom/developer_workbench/commands/run.h"

int main(void)
{
    const UmiDeveloperWorkbenchCommandDefinition *items =
        umi_developer_workbench_run_commands();
    const size_t count = umi_developer_workbench_run_command_count();
    size_t index;
    size_t other;

    assert(items != NULL);
    assert(count == 4U);

    for (index = 0U; index < count; ++index) {
        assert(umi_developer_workbench_command_definition_validate(
            &items[index]) == UMI_STATUS_OK);

        for (other = index + 1U; other < count; ++other) {
            assert(strcmp(items[index].command_id,
                          items[other].command_id) != 0);
        }
    }

    return 0;
}
