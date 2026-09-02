/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_desktop_views.c
 *
 * PURPOSE:
 *   Implement the test desktop views behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Desktop semantic views | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>

#include "umicom/desktop/views.h"
#include "umicom/ui/command_view.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationContextHub *hub = NULL;
    UmiDesktopRuntime *runtime = NULL;
    UmiDesktopShellModel *shell = NULL;
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    UmiUiCommandViewAction action;
    assert(umi_application_context_hub_create(&hub) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(hub, &runtime) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(runtime) == UMI_STATUS_OK);
    assert(umi_desktop_shell_model_create(runtime, &shell) == UMI_STATUS_OK);
    assert(umi_desktop_overview_view_create(
        "desktop.overview", shell, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "layout-count", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 16);
    umi_ui_view_model_destroy(view);
    view = NULL;
    assert(umi_desktop_layout_designer_view_create(
        "desktop.designer", shell, &view) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(view, 0U, &action) == UMI_STATUS_OK);
    assert(strcmp(action.action_id, UMI_DESKTOP_ACTION_NEW_LAYOUT) == 0);
    assert(action.enabled);
    umi_ui_view_model_destroy(view);
    view = NULL;
    assert(umi_desktop_monitor_manager_view_create(
        "desktop.monitors", shell, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "primary-monitor-id", &value) == UMI_STATUS_OK);
    assert(strcmp(value.string_value, "primary") == 0);
    umi_ui_view_model_destroy(view);
    umi_desktop_shell_model_destroy(shell);
    umi_desktop_runtime_destroy(runtime);
    umi_application_context_hub_destroy(hub);
    return 0;
}
