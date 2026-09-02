/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_state.c
 *
 * PURPOSE:
 *   Verify reusable workbench focus, region and zen-mode state.
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

#include "umicom/application_shell/state.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationShellState state;

    umi_application_shell_state_init(&state);
    assert(state.primary_sidebar_visible == 1);
    assert(state.bottom_panel_visible == 1);

    assert(umi_application_shell_state_activate(
        &state, "explorer", "project-explorer") == UMI_STATUS_OK);
    assert(strcmp(state.active_activity_id, "explorer") == 0);

    assert(umi_application_shell_state_focus(
        &state, "editor.main") == UMI_STATUS_OK);
    assert(strcmp(state.focused_view_id, "editor.main") == 0);

    umi_application_shell_state_set_zen(&state, 1);
    assert(state.zen_mode == 1);
    assert(state.primary_sidebar_visible == 0);
    assert(state.bottom_panel_visible == 0);
    assert(state.status_bar_visible == 0);

    return 0;
}
