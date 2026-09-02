/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_assistance_commands.c
 *
 * PURPOSE:
 *   Implement the test editor assistance commands behavior for
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
 * File: tests/editor/test_editor_assistance_commands.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/assistance_command.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    size_t index;
    size_t comparison;

    assert(umi_editor_assistance_command_count() == 14U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_editor_assistance_command_count(); ++index) {
        const UmiEditorAssistanceCommandDescriptor *command =
            umi_editor_assistance_command_at(index);
        assert(command != NULL);
        assert(command->struct_size == (uint32_t)sizeof(*command));
        assert(command->api_version == UMI_EDITOR_ASSISTANCE_COMMAND_API_VERSION);
        assert(command->kind >= UMI_EDITOR_ASSISTANCE_COMMAND_SHOW_HOVER);
        assert(command->kind <= UMI_EDITOR_ASSISTANCE_COMMAND_REFRESH);
        assert(command->id[0] != '\0');
        assert(command->label[0] != '\0');
        assert(umi_editor_assistance_command_find(command->id) == command);
        assert(umi_editor_assistance_command_for_kind(command->kind) == command);
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = index + 1U;
             comparison < umi_editor_assistance_command_count();
             ++comparison) {
            assert(strcmp(command->id,
                          umi_editor_assistance_command_at(comparison)->id) != 0);
        }
    }
    assert(umi_editor_assistance_command_find(
               "editor.assistance.signature.trigger") != NULL);
    assert(umi_editor_assistance_command_find("missing") == NULL);
    assert(umi_editor_assistance_command_at(
               umi_editor_assistance_command_count()) == NULL);
    return 0;
}
