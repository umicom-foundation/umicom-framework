/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_search_command.c
 *
 * PURPOSE:
 *   Implement the test workspace search command behavior for
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
 * File: tests/test_workspace_search_command.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/workspace_search_command.h"

int main(void)
{
    const UmiEditorWorkspaceSearchCommandDescriptor *descriptor;
    size_t position;
    assert(umi_editor_workspace_search_command_count() == 22U);
    for (position = 0U;
         position < umi_editor_workspace_search_command_count();
         ++position) {
        size_t comparison;
        descriptor = umi_editor_workspace_search_command_at(position);
        assert(descriptor != NULL);
        assert(descriptor->struct_size == sizeof(*descriptor));
        assert(descriptor->api_version ==
               UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_API_VERSION);
        assert(descriptor->id != NULL && descriptor->id[0] != '\0');
        assert(descriptor->label != NULL && descriptor->label[0] != '\0');
        assert(umi_editor_workspace_search_command_find(descriptor->id) ==
               descriptor);
        assert(umi_editor_workspace_search_command_for_kind(
                   descriptor->kind) == descriptor);
        for (comparison = position + 1U;
             comparison < umi_editor_workspace_search_command_count();
             ++comparison) {
            const UmiEditorWorkspaceSearchCommandDescriptor *other =
                umi_editor_workspace_search_command_at(comparison);
            assert(other != NULL);
            assert(strcmp(descriptor->id, other->id) != 0);
        }
    }
    descriptor = umi_editor_workspace_search_command_find(
        "replace.workspace.apply");
    assert(descriptor != NULL);
    assert(descriptor->requires_confirmation);
    assert(descriptor->mutates_workspace);
    return 0;
}
