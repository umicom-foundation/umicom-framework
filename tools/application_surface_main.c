/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/application_surface_main.c
 *
 * PURPOSE:
 *   Start any Framework-owned workspace recipe in the headless presentation
 *   host and print a small diagnostic snapshot for developers and automation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>

#include "umicom/application/presentation/presentation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(int argc, char **argv)
{
    const char *recipe_id = argc > 1
        ? argv[1]
        : "org.umicom.workspace.studio.standard";
    UmiApplicationPresentationSurfaceRuntime runtime;
    UmiApplicationPresentationHeadlessSurfaceHost host;
    UmiApplicationPresentationSurfaceSnapshot snapshot;
    UmiStatus status;
    status = umi_application_presentation_surface_runtime_init(
        recipe_id, &runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Unable to load recipe '%s': %d\n",
                      recipe_id, (int)status);
        return 1;
    }
    umi_application_presentation_headless_surface_host_init(&host);
    status = umi_application_presentation_surface_runtime_bind_host(
        &runtime,
        umi_application_presentation_headless_surface_host_interface(&host));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_application_presentation_surface_runtime_start(&runtime);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_application_presentation_surface_runtime_snapshot(
            &runtime, &snapshot);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Unable to start recipe '%s': %d\n",
                      recipe_id, (int)status);
        (void)umi_application_presentation_surface_runtime_stop(&runtime);
        return 1;
    }
    (void)printf("Recipe: %s\nWindow: %s\nPanels: %zu (%zu visible)\n"
                 "Ready: %zu\nNeeds attention: %zu\nFocused: %s\n",
                 snapshot.recipe_id, snapshot.window_id, snapshot.panel_count,
                 snapshot.visible_count, snapshot.ready_count,
                 snapshot.attention_count,
                 snapshot.focused_component_id != NULL
                     ? snapshot.focused_component_id
                     : "none");
    return umi_application_presentation_surface_runtime_stop(&runtime) ==
                   UMI_STATUS_OK
        ? 0
        : 1;
}
