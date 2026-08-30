/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_shell_model.c
 *
 * PURPOSE:
 *   Verify the federated desktop shell, application taskbar, layout tabs and
 *   visual layout designer state remain toolkit-neutral and deterministic.
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

#include "umicom/desktop/shell_model.h"

int main(void)
{
    UmiApplicationContextHub *hub = NULL;
    UmiDesktopRuntime *runtime = NULL;
    UmiDesktopShellModel *shell = NULL;
    UmiDesktopShellSnapshot snapshot;
    UmiDesktopTaskbarItem application;
    UmiDesktopShellTab tab;
    UmiDesktopCanvasWindow window;
    assert(umi_application_context_hub_create(&hub) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(hub, &runtime) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(runtime) == UMI_STATUS_OK);
    assert(umi_desktop_shell_model_create(runtime, &shell) == UMI_STATUS_OK);
    assert(umi_desktop_shell_model_snapshot(shell, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.application_count == 1U);
    assert(snapshot.pinned_application_count == 1U);
    assert(snapshot.running_application_count == 1U);
    assert(strcmp(snapshot.active_application_id, "org.umicom.studio") == 0);
    assert(snapshot.tab_count == 16U && snapshot.monitor_count == 1U);
    assert(strcmp(snapshot.active_layout_id, "develop") == 0);

    assert(umi_desktop_shell_model_application_at(
        shell, 0U, &application) == UMI_STATUS_OK);
    assert(strcmp(application.application_id, "org.umicom.studio") == 0);
    assert(strcmp(application.icon_name,
                  "applications-development-symbolic") == 0);
    assert(application.visible && application.running && application.active);

    /* A newly installed module becomes a visible taskbar application without
     * scanning or executing arbitrary files from applications/. */
    assert(umi_desktop_shell_model_set_application_presence(
        shell, "org.umicom.trader", true, true, true) == UMI_STATUS_OK);
    assert(umi_desktop_shell_model_pin_application(
        shell, "org.umicom.trader", true) == UMI_STATUS_OK);
    assert(umi_desktop_shell_model_activate_application(
        shell, "org.umicom.trader") == UMI_STATUS_OK);
    assert(umi_desktop_shell_model_find_application(
        shell, "org.umicom.trader", &application) == UMI_STATUS_OK);
    assert(application.visible && application.running && application.active);
    assert(strcmp(application.default_layout_id, "trading") == 0);
    assert(umi_desktop_shell_model_snapshot(shell, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.application_count == 2U);
    assert(snapshot.pinned_application_count == 2U);
    assert(snapshot.running_application_count == 2U);
    assert(strcmp(snapshot.active_application_id, "org.umicom.trader") == 0);

    assert(umi_desktop_shell_model_set_application_state(
        shell, "org.umicom.trader", UMI_DESKTOP_APPLICATION_ATTENTION) ==
        UMI_STATUS_OK);
    assert(umi_desktop_shell_model_find_application(
        shell, "org.umicom.trader", &application) == UMI_STATUS_OK);
    assert(application.attention && application.running);
    assert(strcmp(umi_desktop_application_state_text(application.state),
                  "attention") == 0);

    assert(umi_desktop_shell_model_tab_at(shell, 0U, &tab) == UMI_STATUS_OK);
    assert(strcmp(tab.layout_id, "develop") == 0 && tab.active && tab.pinned);
    assert(umi_desktop_shell_model_activate_layout(shell, "trading") ==
           UMI_STATUS_OK);
    assert(umi_desktop_shell_model_set_viewport(shell, 960.0, 520.0) ==
           UMI_STATUS_OK);
    assert(umi_desktop_shell_model_begin_design(
        shell, "trading-shell-b79", "Trading Shell") == UMI_STATUS_OK);
    assert(umi_desktop_shell_model_window_at(shell, 1U, &window) ==
           UMI_STATUS_OK);
    assert(umi_desktop_shell_model_select_at(
        shell, window.bounds.x + window.bounds.width / 2.0,
        window.bounds.y + window.bounds.height / 2.0) == UMI_STATUS_OK);
    assert(umi_desktop_shell_model_move_selected_canvas(shell, 12.0, 8.0) ==
           UMI_STATUS_OK);
    assert(umi_desktop_shell_model_end_design(shell, true) == UMI_STATUS_OK);
    assert(umi_desktop_shell_model_snapshot(shell, &snapshot) == UMI_STATUS_OK);
    assert(!snapshot.designer_active);
    assert(strcmp(snapshot.active_layout_id, "trading-shell-b79") == 0);
    assert(snapshot.tab_count == 17U);
    umi_desktop_shell_model_destroy(shell);
    umi_desktop_runtime_destroy(runtime);
    umi_application_context_hub_destroy(hub);
    return 0;
}
