/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_edit_commands.c
 *
 * PURPOSE:
 *   Implement the test editor edit commands behavior for
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
 * Umicom Framework professional editing command catalogue tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/edit_command.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorEditCommandRequest request;
    const UmiEditorEditCommandDescriptor *descriptor;
    size_t index;

    assert(umi_editor_edit_command_count() == 14U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_editor_edit_command_count(); ++index) {
        descriptor = umi_editor_edit_command_at(index);
        assert(descriptor != NULL);
        assert(descriptor->struct_size == (uint32_t)sizeof(*descriptor));
        assert(descriptor->api_version == UMI_EDITOR_EDIT_COMMAND_API_VERSION);
        assert(descriptor->id[0] != '\0');
        assert(umi_editor_edit_command_find(descriptor->id) == descriptor);
        assert(umi_editor_edit_command_for_kind(descriptor->kind) == descriptor);
    }
    descriptor = umi_editor_edit_command_find("editor.edit.toggle-line-comment");
    assert(descriptor != NULL);
    assert(strcmp(descriptor->default_key, "Ctrl+/") == 0);
    assert(descriptor->line_oriented);
    assert(umi_editor_edit_command_request_initialize(
               &request,
               UMI_EDITOR_EDIT_COMMAND_DELETE_LINE,
               17U) == UMI_STATUS_OK);
    assert(request.cursor_offset == 17U);
    assert(request.selection_start == 17U);
    assert(request.selection_end == 17U);
    assert(umi_editor_edit_command_find("editor.edit.missing") == NULL);
    return 0;
}
