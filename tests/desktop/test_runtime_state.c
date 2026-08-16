/* Umicom Framework Tests | Desktop runtime state | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/desktop/runtime.h"

int main(void)
{
    UmiApplicationContextHub *hub = NULL;
    UmiDesktopRuntime *runtime = NULL;
    UmiDesktopRuntimeState *state = NULL;
    const UmiDesktopWindow *window;
    int32_t original_x;
    assert(umi_application_context_hub_create(&hub) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(hub, &runtime) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(runtime) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_clone_layout(
        runtime, "develop", "runtime-state", "Runtime State", true) ==
        UMI_STATUS_OK);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime), "project-explorer");
    assert(window != NULL);
    original_x = window->bounds.x;
    state = (UmiDesktopRuntimeState *)calloc(1U, sizeof(*state));
    assert(state != NULL);
    assert(umi_desktop_runtime_capture_state(runtime, state) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_place_window(
        runtime, "project-explorer", "primary",
        (UmiDesktopRect){700, 90, 480, 760},
        UMI_DESKTOP_DOCK_FLOATING) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_restore_state(runtime, state) == UMI_STATUS_OK);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime), "project-explorer");
    assert(window != NULL && window->bounds.x == original_x);
    assert(strcmp(umi_desktop_runtime_layouts(runtime)->active_layout_id,
                  "runtime-state") == 0);
    free(state);
    umi_desktop_runtime_destroy(runtime);
    umi_application_context_hub_destroy(hub);
    return 0;
}
