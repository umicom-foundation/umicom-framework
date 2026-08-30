/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_command_catalogue.c
 * PURPOSE: Verify stable unique command IDs and external-action trust gates.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/command_catalogue.h"

int main(void)
{
    size_t index;
    size_t external_count = 0U;
    assert(umi_web_workbench_command_count() == 46U);
    for (index = 0U; index < umi_web_workbench_command_count(); ++index) {
        const UmiWebWorkbenchCommandDescriptor *command =
            umi_web_workbench_command_at(index);
        size_t other;
        assert(command != NULL);
        assert(command->structure_size == sizeof(*command));
        assert(command->api_version == UMI_WEB_WORKBENCH_API_VERSION);
        if (command->mutates_external_state) {
            external_count++;
            assert(command->requires_workspace_trust);
        }
        for (other = index + 1U; other < umi_web_workbench_command_count(); ++other) {
            assert(strcmp(command->command_id,
                umi_web_workbench_command_at(other)->command_id) != 0);
        }
    }
    assert(external_count > 0U);
    assert(umi_web_workbench_command_find("web.workbench.request.send") != NULL);
    assert(umi_web_workbench_command_find("missing") == NULL);
    return 0;
}
