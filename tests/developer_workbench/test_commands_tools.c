/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_commands_tools.c
 *
 * PURPOSE:
 *   Verify all built-in Tools command definitions are valid and unique
 *   within their command group.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>

#include "umicom/developer_workbench/commands/tools.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiDeveloperWorkbenchCommandDefinition *items =
        umi_developer_workbench_tools_commands();
    const size_t count = umi_developer_workbench_tools_command_count();
    size_t index;
    size_t other;

    assert(items != NULL);
    /* Cancellation extends the catalogue; keep the previous count for review. */
#if 0
    assert(count == 3U);
#endif
    assert(count == 4U);
    size_t scans = 0U, cancellations = 0U;
    for (size_t i = 0U; i < count; ++i) {
        if (strcmp(items[i].command_id, "codeguard.scan") == 0) {
            ++scans;
            assert((items[i].flags & UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE) != 0U);
        }
        if (strcmp(items[i].command_id, "codeguard.cancel") == 0) {
            ++cancellations;
            assert((items[i].flags & UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE) == 0U);
            assert((items[i].flags & UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND) != 0U);
        }
    }
    assert(scans == 1U && cancellations == 1U);

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
