/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_command.c
 *
 * PURPOSE:
 *   Verify the canonical diagnostics command catalogue exposes unique, stable
 *   Framework command identities and lookup semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/diagnostics/command.h"

int main(void)
{
    size_t position;
    assert(umi_diagnostic_command_count() == 26U);
    for (position = 0U; position < umi_diagnostic_command_count(); ++position) {
        const UmiDiagnosticCommandDescriptor *command =
            umi_diagnostic_command_at(position);
        size_t comparison;
        assert(command != NULL);
        assert(command->struct_size == (uint32_t)sizeof(*command));
        assert(command->api_version == UMI_DIAGNOSTIC_COMMAND_API_VERSION);
        assert(command->id != NULL && command->id[0] != '\0');
        assert(command->label != NULL && command->label[0] != '\0');
        assert(umi_diagnostic_command_find(command->id) == command);
        assert(umi_diagnostic_command_for_kind(command->kind) == command);
        for (comparison = position + 1U;
             comparison < umi_diagnostic_command_count(); ++comparison) {
            assert(strcmp(command->id,
                          umi_diagnostic_command_at(comparison)->id) != 0);
        }
    }
    assert(umi_diagnostic_command_find("diagnostics.sarif.export") != NULL);
    assert(umi_diagnostic_command_find("diagnostics.suppressions.manage") != NULL);
    assert(umi_diagnostic_command_at(umi_diagnostic_command_count()) == NULL);
    return 0;
}
