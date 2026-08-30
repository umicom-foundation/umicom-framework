/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_workbench/test_command_catalogue.c
 *
 * PURPOSE:
 *   Verify stable unique compare, merge, staging and review commands.
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

#include "umicom/vcs/workbench/command_catalogue.h"

int main(void)
{
    size_t index;
    size_t comparison;
    assert(umi_vcs_workbench_command_count() == 26U);
    for (index = 0U; index < umi_vcs_workbench_command_count(); ++index) {
        const UmiVcsWorkbenchCommand *command =
            umi_vcs_workbench_command_at(index);
        assert(command != NULL);
        assert(command->struct_size == (uint32_t)sizeof(*command));
        for (comparison = index + 1U;
             comparison < umi_vcs_workbench_command_count(); ++comparison) {
            assert(strcmp(command->command_id,
                          umi_vcs_workbench_command_at(comparison)->command_id) !=
                   0);
        }
    }
    assert(umi_vcs_workbench_command_find(
               "vcs.workbench.merge.finish") != NULL);
    assert(umi_vcs_workbench_command_find("missing") == NULL);
    return 0;
}
