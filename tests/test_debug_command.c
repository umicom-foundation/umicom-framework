/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_command.c
 *
 * PURPOSE:
 *   Implement the test debug command behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_command.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/debug/command.h"

int main(void)
{
    size_t index;
    size_t comparison;

    assert(umi_debug_command_count() == 50U);
    for (index = 0U; index < umi_debug_command_count(); ++index) {
        const UmiDebugCommandDescriptor *command = umi_debug_command_at(index);
        assert(command != NULL);
        assert(command->struct_size == (uint32_t)sizeof(*command));
        assert(command->api_version == UMI_DEBUG_COMMAND_API_VERSION);
        assert(command->id[0] != '\0');
        assert(command->label[0] != '\0');
        assert(umi_debug_command_find(command->id) == command);
        assert(umi_debug_command_for_kind(command->kind) == command);
        for (comparison = index + 1U;
             comparison < umi_debug_command_count(); ++comparison) {
            assert(strcmp(command->id,
                          umi_debug_command_at(comparison)->id) != 0);
        }
    }
    assert(umi_debug_command_find("debug.start") != NULL);
    assert(umi_debug_command_find("debug.breakpoint.add-data") != NULL);
    assert(umi_debug_command_find("debug.registers.open") != NULL);
    assert(umi_debug_command_find("debug.memory.write") != NULL);
    assert(umi_debug_command_find("debug.disassembly.refresh") != NULL);
    assert(umi_debug_command_find("debug.adapters.open") != NULL);
    assert(umi_debug_command_at(umi_debug_command_count()) == NULL);
    return 0;
}
