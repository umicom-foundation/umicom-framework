/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench_canvas.c
 *
 * PURPOSE:
 *   Verify the portable Workbench Canvas coordinator routes layout operations
 *   through the existing workspace customisation model and keeps detachable
 *   monitor state in step with the active layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workbench_canvas.h"

#include <assert.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/customisation.h"

/* Register one reusable surface before a host tries to open it. */
static void register_explorer(UmiUiWorkspaceCustomisation *customisation)
{
    UmiUiWindowDescriptor descriptor = {
        .tool_id = "explorer",
        .title = "Explorer",
        .description = "Browse the active workspace.",
        .icon_name = "folder",
        .category = UMI_UI_WINDOW_CATEGORY_NAVIGATION,
        .supports_multiple = true,
        .default_width = 0.25,
        .default_height = 0.70
    };

    assert(umi_ui_window_catalogue_register(
               &customisation->catalogue,
               &descriptor) == UMI_STATUS_OK);
}

/* Confirm every published application experience can provide a usable layout
 * and a registered Workbench Host without creating product-specific canvas
 * bootstrapping code. */
static void verify_application_layouts(void)
{
    size_t index;

    for (index = 0U;
         index < umi_application_experience_catalogue_count();
         ++index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(index);
        UmiUiWorkspaceCustomisation customisation;
        UmiUiWorkbenchCanvas canvas;
        const UmiUiWorkbenchCanvasHost *host;

        assert(experience != NULL);
        assert(experience->layout_count > 0U);
        umi_ui_workbench_canvas_init(&canvas);
        assert(umi_ui_workbench_canvas_add_application_host(
                   &canvas,
                   "catalogue-host",
                   experience->application_id,
                   "monitor-1",
                   &customisation) == UMI_STATUS_OK);
        host = umi_ui_workbench_canvas_host_const(
            &canvas,
            "catalogue-host");
        assert(host != NULL);
        assert(host->customisation == &customisation);
        assert(customisation.layout_count == experience->layout_count);
        assert(umi_ui_workspace_customisation_active_const(
                   &customisation) != NULL);
    }
}

/* Exercise the complete host, layout, surface and monitor lifecycle. */
int main(void)
{
    UmiUiWorkspaceCustomisation customisation;
    UmiUiWorkbenchCanvas canvas;
    UmiUiWorkbenchCanvasSnapshot snapshot;
    const UmiUiWorkbenchCanvasHost *host;
    const UmiUiWorkbenchCanvasSurfaceState *surface;
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char oversized_monitor[UMI_UI_WORKBENCH_CANVAS_MONITOR_ID_CAPACITY + 1U];

    umi_ui_workspace_customisation_init(&customisation);
    register_explorer(&customisation);
    umi_ui_workbench_canvas_init(&canvas);

    assert(umi_ui_workbench_canvas_add_host(
               &canvas,
               "studio-host-1",
               "org.umicom.studio",
               "monitor-1",
               &customisation) == UMI_STATUS_OK);
    assert(umi_ui_workbench_canvas_create_blank_layout(
               &canvas,
               "studio-host-1",
               "blank-layout",
               "Blank Layout") == UMI_STATUS_OK);
    assert(umi_ui_workbench_canvas_open_surface(
               &canvas,
               "studio-host-1",
               "explorer",
               "left",
               true,
               100U,
               window_id,
               sizeof(window_id)) == UMI_STATUS_OK);
    assert(umi_ui_workbench_canvas_move_surface(
               &canvas,
               "studio-host-1",
               window_id,
               0.20,
               0.10) == UMI_STATUS_OK);
    {
        const UmiUiWorkspaceLayout *layout =
            umi_ui_workspace_customisation_active_const(&customisation);
        const UmiUiWorkspaceWindow *window =
            umi_ui_workspace_layout_find_window(layout, window_id);

        /* Moving inside the canvas must not silently create a detached native
         * window; the renderer uses this placement token for internal panels. */
        assert(window != NULL);
        assert(!window->floating);
        assert(strcmp(window->placement_id,
                      UMI_UI_WORKSPACE_CANVAS_PLACEMENT) == 0);
    }
    assert(umi_ui_workbench_canvas_resize_surface(
               &canvas,
               "studio-host-1",
               window_id,
               0.40,
               0.50) == UMI_STATUS_OK);
    assert(umi_ui_workbench_canvas_snap_surface(
               &canvas,
               "studio-host-1",
               window_id,
               0.10) == UMI_STATUS_OK);
    /* Reject an oversized monitor identity before the detach transaction
     * starts, so failed persistence cannot leave a half-detached surface. */
    (void)memset(oversized_monitor,
                 'x',
                 sizeof(oversized_monitor) - 1U);
    oversized_monitor[sizeof(oversized_monitor) - 1U] = '\0';
    assert(umi_ui_workbench_canvas_detach_surface(
               &canvas,
               "studio-host-1",
               window_id,
               oversized_monitor) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_ui_workbench_canvas_detach_surface(
               &canvas,
               "studio-host-1",
               window_id,
               "monitor-2") == UMI_STATUS_OK);

    host = umi_ui_workbench_canvas_host_const(&canvas, "studio-host-1");
    assert(host != NULL);
    surface = umi_ui_workbench_canvas_surface_state(host, window_id);
    assert(surface != NULL);
    assert(surface->detached);
    assert(strcmp(surface->monitor_id, "monitor-2") == 0);

    assert(umi_ui_workbench_canvas_attach_surface(
               &canvas,
               "studio-host-1",
               window_id,
               "left",
               "navigation") == UMI_STATUS_OK);
    host = umi_ui_workbench_canvas_host_const(&canvas, "studio-host-1");
    assert(host != NULL);
    surface = umi_ui_workbench_canvas_surface_state(host, window_id);
    assert(surface != NULL);
    assert(!surface->detached);
    assert(strcmp(surface->monitor_id, "monitor-1") == 0);

    assert(umi_ui_workbench_canvas_snapshot(&canvas, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.host_count == 1U);
    assert(snapshot.hosts[0].surface_count == 1U);

    assert(umi_ui_workbench_canvas_set_layout_locked(
               &canvas,
               "studio-host-1",
               true) == UMI_STATUS_OK);
    assert(umi_ui_workbench_canvas_clear_current_canvas(
               &canvas,
               "studio-host-1") == UMI_STATUS_OK);
    assert(umi_ui_workbench_canvas_snapshot(&canvas, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.hosts[0].surface_count == 0U);

    /* Closing one native host must remove only that host and select the
     * remaining application window instead of leaving a stale route. */
    {
        UmiUiWorkspaceCustomisation second_customisation;
        UmiUiWorkbenchCanvasSnapshot after_remove;

        umi_ui_workspace_customisation_init(&second_customisation);
        register_explorer(&second_customisation);
        assert(umi_ui_workbench_canvas_add_host(
                   &canvas,
                   "trader-host-1",
                   "org.umicom.trader",
                   "monitor-2",
                   &second_customisation) == UMI_STATUS_OK);
        assert(umi_ui_workbench_canvas_set_active_host(
                   &canvas, "trader-host-1") == UMI_STATUS_OK);
        assert(umi_ui_workbench_canvas_remove_host(
                   &canvas, "studio-host-1") == UMI_STATUS_OK);
        assert(umi_ui_workbench_canvas_snapshot(
                   &canvas, &after_remove) == UMI_STATUS_OK);
        assert(after_remove.host_count == 1U);
        assert(after_remove.active_host_index == 0U);
        assert(strcmp(after_remove.hosts[0].host_id,
                      "trader-host-1") == 0);
        assert(after_remove.hosts[0].active);
        {
            const uint64_t revision_before_unknown = canvas.revision;

            /* An unknown close notification must not look like a real canvas
             * edit to observers watching revision numbers. */
            assert(umi_ui_workbench_canvas_remove_host(
                       &canvas, "missing-host") == UMI_STATUS_NOT_FOUND);
            assert(canvas.revision == revision_before_unknown);
        }
        assert(umi_ui_workbench_canvas_remove_host(
                   &canvas, "trader-host-1") == UMI_STATUS_OK);
        assert(canvas.host_count == 0U);
    }

    verify_application_layouts();
    return 0;
}
