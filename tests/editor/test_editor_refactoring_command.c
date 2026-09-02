/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_refactoring_command.c
 *
 * PURPOSE:
 *   Implement the test editor refactoring command behavior for
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
 * File: tests/editor/test_editor_refactoring_command.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/refactoring_command.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    size_t index;
    size_t comparison;

    assert(umi_editor_refactoring_command_count() == 18U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_editor_refactoring_command_count(); ++index) {
        const UmiEditorRefactoringCommandDescriptor *command =
            umi_editor_refactoring_command_at(index);
        assert(command != NULL);
        assert(command->struct_size == (uint32_t)sizeof(*command));
        assert(command->api_version == UMI_EDITOR_REFACTORING_COMMAND_API_VERSION);
        assert(command->id[0] != '\0');
        assert(command->label[0] != '\0');
        assert(umi_editor_refactoring_command_find(command->id) == command);
        assert(umi_editor_refactoring_command_for_kind(command->kind) == command);
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = index + 1U;
             comparison < umi_editor_refactoring_command_count(); ++comparison) {
            assert(strcmp(command->id,
                          umi_editor_refactoring_command_at(comparison)->id) != 0);
        }
    }
    assert(umi_editor_refactoring_command_find("editor.refactor.rename") != NULL);
    assert(umi_editor_refactoring_command_at(
               umi_editor_refactoring_command_count()) == NULL);
    return 0;
}
