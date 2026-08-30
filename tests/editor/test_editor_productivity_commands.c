/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_productivity_commands.c
 *
 * PURPOSE:
 *   Implement the test editor productivity commands behavior for
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
 * File: tests/editor/test_editor_productivity_commands.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/productivity_command.h"

int main(void)
{
    size_t index;
    size_t comparison;

    assert(umi_editor_productivity_command_count() == 16U);
    for (index = 0U; index < umi_editor_productivity_command_count(); ++index) {
        const UmiEditorProductivityCommandDescriptor *command =
            umi_editor_productivity_command_at(index);
        assert(command != NULL);
        assert(command->struct_size == (uint32_t)sizeof(*command));
        assert(command->api_version ==
               UMI_EDITOR_PRODUCTIVITY_COMMAND_API_VERSION);
        assert(command->id[0] != '\0');
        assert(command->label[0] != '\0');
        assert(umi_editor_productivity_command_find(command->id) == command);
        assert(umi_editor_productivity_command_for_kind(command->kind) ==
               command);
        for (comparison = index + 1U;
             comparison < umi_editor_productivity_command_count();
             ++comparison) {
            assert(strcmp(command->id,
                          umi_editor_productivity_command_at(comparison)->id) !=
                   0);
        }
    }
    assert(umi_editor_productivity_command_at(
               umi_editor_productivity_command_count()) == NULL);
    assert(umi_editor_productivity_command_find("missing") == NULL);
    return 0;
}
