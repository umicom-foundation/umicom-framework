/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_workbench_state.c
 *
 * PURPOSE:
 *   Exercise one Batch 23 unified-workbench contract through its public C API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These tests are intentionally small. They document expected behaviour without starting a graphical application.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiUiWorkbenchState state;
    UmiUiWorkbenchState decoded;
    char encoded[UMI_UI_WORKBENCH_STATE_TEXT_CAPACITY];

    umi_ui_workbench_state_init(&state);
    (void)snprintf(state.active_activity, sizeof(state.active_activity),
                   "%s", "studio.activity.explorer");
    (void)snprintf(state.active_view_container,
                   sizeof(state.active_view_container),
                   "%s", "studio.container.explorer");
    (void)snprintf(state.active_workspace_profile,
                   sizeof(state.active_workspace_profile),
                   "%s", "develop");
    state.sidebar_size = 320;
    state.bottom_panel_visible = 0;
    state.editor_split_mode = UMI_UI_EDITOR_SPLIT_COLUMNS;
    state.editor_split_ratio = 4700;
    (void)snprintf(state.active_editor_group,
                   sizeof(state.active_editor_group), "%s",
                   UMI_UI_SECONDARY_EDITOR_GROUP_ID);

    assert(umi_ui_workbench_state_encode(&state, encoded,
                                         sizeof(encoded)) == UMI_STATUS_OK);
    assert(umi_ui_workbench_state_decode(encoded, &decoded) == UMI_STATUS_OK);
    assert(strcmp(decoded.active_activity,
                  "studio.activity.explorer") == 0);
    assert(decoded.sidebar_size == 320);
    assert(decoded.bottom_panel_visible == 0);
    assert(strcmp(decoded.active_workspace_profile, "develop") == 0);
    assert(decoded.editor_split_mode == UMI_UI_EDITOR_SPLIT_COLUMNS);
    assert(decoded.editor_split_ratio == 4700);
    assert(strcmp(decoded.active_editor_group,
                  UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0);

    /* Version 1 sessions remain readable after named profiles are introduced. */
    assert(umi_ui_workbench_state_decode(
               "v1|||||1|0|1|280|360|240|7", &decoded) == UMI_STATUS_OK);
    assert(decoded.active_workspace_profile[0] == '\0');
    assert(decoded.sidebar_size == 280);
    assert(decoded.editor_split_mode == UMI_UI_EDITOR_SPLIT_SINGLE);
    assert(decoded.editor_split_ratio == UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT);
    assert(umi_ui_workbench_state_decode(
               "v3||||||1|1|1|280|360|240|editor.secondary|1|999|7",
               &decoded) == UMI_STATUS_PARSE_ERROR);
    return 0;
}
