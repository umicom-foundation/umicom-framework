/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_ecosystem/test_command_catalogue.c
 * PURPOSE: Verify stable, unique package and extension workbench commands.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer/ecosystem/command_catalogue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    size_t index;
    assert(umi_ecosystem_command_count() == 40U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_ecosystem_command_count(); ++index) {
        const UmiEcosystemCommand *command = umi_ecosystem_command_at(index);
        size_t other;
        assert(command != NULL && command->command_id[0] != '\0');
        assert(umi_ecosystem_command_find(command->command_id) == command);
        /* Visit each bounded item once so every record receives the same rule. */
        for (other = index + 1U; other < umi_ecosystem_command_count(); ++other) {
            assert(strcmp(command->command_id,
                umi_ecosystem_command_at(other)->command_id) != 0);
        }
    }
    assert(umi_ecosystem_command_find("ecosystem.packages.install") != NULL);
    assert(umi_ecosystem_command_find("ecosystem.missing") == NULL);
    return 0;
}
