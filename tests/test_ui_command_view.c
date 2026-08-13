/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_command_view.c
 *
 * PURPOSE:
 *   Verify that command surfaces remain toolkit-neutral and round-trip action
 *   identity, labels, tooltips and enablement through a view model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/contracts.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiUiViewModel *view = NULL;
    UmiUiCommandViewAction action = {0};
    UmiUiCommandViewAction actual;
    (void)strcpy(action.action_id, "test.action.build");
    (void)strcpy(action.label, "Build");
    (void)strcpy(action.tooltip, "Build the active target");
    action.enabled = 1;
    assert(umi_ui_view_model_create("test.view", "test.command-view",
                                    UMI_UI_ROLE_PANE, &view) == UMI_STATUS_OK);
    assert(umi_ui_command_view_set_action(view, 0U, &action) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(view, 0U, &actual) == UMI_STATUS_OK);
    assert(strcmp(actual.action_id, action.action_id) == 0);
    assert(strcmp(actual.label, action.label) == 0);
    assert(strcmp(actual.tooltip, action.tooltip) == 0);
    assert(actual.enabled);
    assert(umi_ui_command_view_property_is_reserved(
        UMI_UI_COMMAND_VIEW_ACTION_COUNT_KEY));
    assert(!umi_ui_command_view_property_is_reserved("title"));
    umi_ui_view_model_destroy(view);
    return 0;
}
