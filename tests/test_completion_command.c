/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_completion_command.c
 *
 * PURPOSE:
 *   Verify the complete command catalogue, identity uniqueness and document-
 *   mutation/confirmation metadata for completion and inline suggestions.
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

#include "umicom/editor/completion_command.h"

int main(void)
{
    size_t position;
    size_t comparison;
    size_t document_mutations = 0U;
    size_t confirmation_commands = 0U;

    assert(umi_editor_completion_command_count() == 24U);
    for (position = 0U; position < umi_editor_completion_command_count();
         ++position) {
        const UmiEditorCompletionCommandDescriptor *descriptor =
            umi_editor_completion_command_at(position);
        assert(descriptor != NULL);
        assert(descriptor->struct_size == (uint32_t)sizeof(*descriptor));
        assert(descriptor->api_version ==
               UMI_EDITOR_COMPLETION_COMMAND_API_VERSION);
        assert(descriptor->id != NULL && descriptor->id[0] != '\0');
        assert(descriptor->label != NULL && descriptor->label[0] != '\0');
        assert(umi_editor_completion_command_find(descriptor->id) ==
               descriptor);
        assert(umi_editor_completion_command_for_kind(descriptor->kind) ==
               descriptor);
        if (descriptor->mutates_document) ++document_mutations;
        if (descriptor->requires_confirmation) ++confirmation_commands;
        for (comparison = position + 1U;
             comparison < umi_editor_completion_command_count();
             ++comparison) {
            const UmiEditorCompletionCommandDescriptor *other =
                umi_editor_completion_command_at(comparison);
            assert(other != NULL);
            assert(strcmp(descriptor->id, other->id) != 0);
            assert(descriptor->kind != other->kind);
        }
    }
    assert(document_mutations == 5U);
    assert(confirmation_commands == 1U);
    assert(umi_editor_completion_command_find("editor.completion.accept") !=
           NULL);
    assert(umi_editor_completion_command_find(
               "editor.inline-suggestion.accept-word") != NULL);
    assert(umi_editor_completion_command_find("missing") == NULL);
    assert(umi_editor_completion_command_at(24U) == NULL);
    return 0;
}
