/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_intelligence_commands.c
 *
 * PURPOSE:
 *   Implement the test editor intelligence commands behavior for
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
 * File: tests/editor/test_editor_intelligence_commands.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/intelligence_command.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    size_t index;
    size_t comparison;
    const UmiEditorIntelligenceCommandDescriptor *command;

    assert(umi_editor_intelligence_command_count() == 12U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_editor_intelligence_command_count(); ++index) {
        command = umi_editor_intelligence_command_at(index);
        assert(command != NULL);
        assert(command->struct_size == (uint32_t)sizeof(*command));
        assert(command->api_version ==
               UMI_EDITOR_INTELLIGENCE_COMMAND_API_VERSION);
        assert(command->id[0] != '\0');
        assert(command->label[0] != '\0');
        assert(umi_editor_intelligence_command_find(command->id) == command);
        assert(umi_editor_intelligence_command_for_kind(command->kind) ==
               command);
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = index + 1U;
             comparison < umi_editor_intelligence_command_count();
             ++comparison) {
            assert(strcmp(command->id,
                          umi_editor_intelligence_command_at(comparison)->id) !=
                   0);
        }
    }
    command = umi_editor_intelligence_command_find(
        "editor.intelligence.rename");
    assert(command != NULL);
    assert(strcmp(command->default_key, "F2") == 0);
    assert(command->requires_symbol);
    assert(command->requires_editable_document);
    assert(umi_editor_intelligence_command_find("missing") == NULL);
    return 0;
}
