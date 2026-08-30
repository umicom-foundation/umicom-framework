/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_command_catalogue.c
 *
 * PURPOSE:
 *   Verify unique stable Database workbench commands and mutation metadata.
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

#include "umicom/data/workbench/command_catalogue.h"

int main(void)
{
    size_t index;
    size_t comparison;
    assert(umi_data_workbench_command_count() == 34U);
    for (index = 0U; index < umi_data_workbench_command_count(); ++index) {
        const UmiDataWorkbenchCommand *command =
            umi_data_workbench_command_at(index);
        assert(command != NULL);
        for (comparison = index + 1U;
             comparison < umi_data_workbench_command_count(); ++comparison) {
            assert(strcmp(command->command_id,
                          umi_data_workbench_command_at(comparison)->command_id) !=
                   0);
        }
    }
    assert(umi_data_workbench_command_find(
               "data.workbench.schema.compare") != NULL);
    return 0;
}
