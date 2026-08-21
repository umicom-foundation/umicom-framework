/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_command_context.c
 *
 * PURPOSE:
 *   Verify command enablement reflects selection, lock, history, save state and
 *   current designer mode.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerClipboard clipboard;
    UmiWorkbenchDesignerCommandHistory history;
    UmiWorkbenchDesignerLockState lock_state;
    UmiWorkbenchDesignerCommandContext context;
    const UmiWorkbenchDesignerCommandState *state;
    umi_workbench_designer_selection_init(&selection);
    umi_workbench_designer_clipboard_init(&clipboard);
    umi_workbench_designer_command_history_init(&history);
    (void)memset(&lock_state, 0, sizeof(lock_state));
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "editor", true));
    TEST_REQUIRE_STATUS(umi_workbench_designer_command_context_build(
        &context, UMI_WORKBENCH_DESIGNER_MODE_DESIGN, &selection,
        &clipboard, &history, &lock_state,
        UMI_WORKBENCH_DESIGNER_SAVE_DIRTY));
    state = umi_workbench_designer_command_context_find(
        &context, UMI_WORKBENCH_DESIGNER_COMMAND_REMOVE_NODE);
    TEST_REQUIRE(state != NULL && state->enabled);
    lock_state.locked = true;
    TEST_REQUIRE_STATUS(umi_workbench_designer_command_context_build(
        &context, UMI_WORKBENCH_DESIGNER_MODE_DESIGN, &selection,
        &clipboard, &history, &lock_state,
        UMI_WORKBENCH_DESIGNER_SAVE_DIRTY));
    state = umi_workbench_designer_command_context_find(
        &context, UMI_WORKBENCH_DESIGNER_COMMAND_REMOVE_NODE);
    TEST_REQUIRE(state != NULL && !state->enabled);
    return 0;
}
