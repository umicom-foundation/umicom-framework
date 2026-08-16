/* Umicom Framework Tests | Federated desktop shell model | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>

#include "umicom/desktop/shell_model.h"

int main(void)
{
    UmiApplicationContextHub *hub = NULL;
    UmiDesktopRuntime *runtime = NULL;
    UmiDesktopShellModel *shell = NULL;
    UmiDesktopShellSnapshot snapshot;
    UmiDesktopShellTab tab;
    UmiDesktopCanvasWindow window;
    assert(umi_application_context_hub_create(&hub) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(hub, &runtime) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(runtime) == UMI_STATUS_OK);
    assert(umi_desktop_shell_model_create(runtime, &shell) == UMI_STATUS_OK);
    assert(umi_desktop_shell_model_snapshot(shell, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.tab_count == 16U && snapshot.monitor_count == 1U);
    assert(strcmp(snapshot.active_layout_id, "develop") == 0);
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
