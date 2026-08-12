/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_workbench_state.c
 *
 * PURPOSE:
 *   Exercise one Batch 23 unified-workbench contract through its public C API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
    state.sidebar_size = 320;
    state.bottom_panel_visible = 0;

    assert(umi_ui_workbench_state_encode(&state, encoded,
                                         sizeof(encoded)) == UMI_STATUS_OK);
    assert(umi_ui_workbench_state_decode(encoded, &decoded) == UMI_STATUS_OK);
    assert(strcmp(decoded.active_activity,
                  "studio.activity.explorer") == 0);
    assert(decoded.sidebar_size == 320);
    assert(decoded.bottom_panel_visible == 0);
    return 0;
}
