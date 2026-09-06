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
#include <stdlib.h>
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
               &customisation->windows,
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
        /* The customisation model owns several complete layouts, so heap
         * storage keeps this catalogue sweep independent of the native stack
         * limit on Windows and other hosts. */
        UmiUiWorkspaceCustomisation *customisation =
            (UmiUiWorkspaceCustomisation *)calloc(1U, sizeof(*customisation));
        UmiUiWorkbenchCanvas canvas;
        const UmiUiWorkbenchCanvasHost *host;

        assert(experience != NULL);
        assert(customisation != NULL);
        assert(experience->layout_count > 0U);
        umi_ui_workspace_customisation_init(customisation);
        umi_ui_workbench_canvas_init(&canvas);
        assert(umi_ui_workbench_canvas_add_application_host(
                   &canvas,
                   "catalogue-host",
                   experience->application_id,
                   "monitor-1",
                   customisation) == UMI_STATUS_OK);
        host = umi_ui_workbench_canvas_host_const(
            &canvas,
            "catalogue-host");
        assert(host != NULL);
        assert(host->customisation == customisation);
        assert(customisation->layout_count == experience->layout_count);
        assert(umi_ui_workspace_customisation_active_const(
                   customisation) != NULL);
        free(customisation);
    }
}

/* Exercise the complete host, layout, surface and monitor lifecycle. */
int main(void)
{
    /* This model contains bounded catalogues and layout snapshots; allocating
     * it on the heap avoids stack exhaustion that otherwise appears as a
     * Windows 0xc0000409 fast-fail before the test can report a check. */
    UmiUiWorkspaceCustomisation *customisation =
        (UmiUiWorkspaceCustomisation *)calloc(1U, sizeof(*customisation));
    UmiUiWorkbenchCanvas canvas;
    UmiUiWorkbenchCanvasSnapshot snapshot;
    const UmiUiWorkbenchCanvasHost *host;
    const UmiUiWorkbenchCanvasSurfaceState *surface;
    UmiUiWorkbenchCanvasSurfaceState surface_snapshot[2];
    size_t surface_snapshot_count;
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char second_window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char oversized_monitor[UMI_UI_WORKBENCH_CANVAS_MONITOR_ID_CAPACITY + 1U];

    assert(customisation != NULL);
    umi_ui_workspace_customisation_init(customisation);
    register_explorer(customisation);
    umi_ui_workbench_canvas_init(&canvas);

    assert(umi_ui_workbench_canvas_add_host(
               &canvas,
               "studio-host-1",
               "org.umicom.studio",
               "monitor-1",
               customisation) == UMI_STATUS_OK);
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
            umi_ui_workspace_customisation_active_const(customisation);
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

    /* A single canvas edit often changes several panels together. Verify the
     * batch contract rolls back the whole gesture when one request is bad. */
    assert(umi_ui_workbench_canvas_open_surface(
               &canvas,
               "studio-host-1",
               "explorer",
               "right",
               false,
               200U,
               second_window_id,
               sizeof(second_window_id)) == UMI_STATUS_OK);
    {
        UmiUiWorkspacePanelSettings batch[2];
        UmiUiWorkspaceLayout before_batch =
            *umi_ui_workspace_customisation_active_const(customisation);

        batch[0] = umi_ui_workspace_panel_settings_default(window_id);
        batch[0].floating = true;
        batch[0].x = 0.05;
        batch[0].y = 0.05;
        batch[0].width = 0.40;
        batch[0].height = 0.40;
        batch[1] = umi_ui_workspace_panel_settings_default("missing-window");
        assert(umi_ui_workbench_canvas_apply_panel_batch(
                   &canvas, "studio-host-1", batch, 2U) ==
               UMI_STATUS_NOT_FOUND);
        assert(memcmp(&before_batch,
                      umi_ui_workspace_customisation_active_const(
                          customisation),
                      sizeof(before_batch)) == 0);

        batch[1] = umi_ui_workspace_panel_settings_default(second_window_id);
        batch[1].placement_id = "right";
        batch[1].stack_id = "quotes";
        assert(umi_ui_workbench_canvas_apply_panel_batch(
                   &canvas, "studio-host-1", batch, 2U) == UMI_STATUS_OK);
        {
            const UmiUiWorkspaceLayout *layout =
                umi_ui_workspace_customisation_active_const(customisation);
            const UmiUiWorkspaceWindow *first =
                umi_ui_workspace_layout_find_window(layout, window_id);
            const UmiUiWorkspaceWindow *second =
                umi_ui_workspace_layout_find_window(layout, second_window_id);

            assert(first != NULL && second != NULL);
            assert(first->floating);
            assert(strcmp(first->placement_id, "floating") == 0);
            assert(!second->floating);
            assert(strcmp(second->placement_id, "right") == 0);
            assert(strcmp(second->stack_id, "quotes") == 0);
        }
        assert(umi_ui_workbench_canvas_apply_panel_batch(
                   &canvas, "studio-host-1", batch, 0U) ==
               UMI_STATUS_INVALID_ARGUMENT);
    }

    /* Frontends receive a copied surface list instead of walking mutable host
     * storage. A short destination is rejected so no panel silently vanishes. */
    host = umi_ui_workbench_canvas_host_const(&canvas, "studio-host-1");
    assert(host != NULL);
    assert(umi_ui_workbench_canvas_surface_snapshot(
               host,
               surface_snapshot,
               1U,
               &surface_snapshot_count) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(surface_snapshot_count == 0U);
    assert(umi_ui_workbench_canvas_surface_snapshot(
               host,
               surface_snapshot,
               2U,
               &surface_snapshot_count) == UMI_STATUS_OK);
    assert(surface_snapshot_count == 2U);
    assert(umi_ui_workbench_canvas_surface_snapshot(
               host,
               NULL,
               0U,
               &surface_snapshot_count) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(surface_snapshot_count == 0U);

    assert(umi_ui_workbench_canvas_snapshot(&canvas, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.host_count == 1U);
    assert(snapshot.hosts[0].surface_count == 2U);

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
        UmiUiWorkspaceCustomisation *second_customisation =
            (UmiUiWorkspaceCustomisation *)calloc(
                1U, sizeof(*second_customisation));
        UmiUiWorkbenchCanvasSnapshot after_remove;

        assert(second_customisation != NULL);
        umi_ui_workspace_customisation_init(second_customisation);
        register_explorer(second_customisation);
        assert(umi_ui_workbench_canvas_add_host(
                   &canvas,
                   "trader-host-1",
                   "org.umicom.trader",
                   "monitor-2",
                   second_customisation) == UMI_STATUS_OK);
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
        free(second_customisation);
    }

    verify_application_layouts();
    free(customisation);
    return 0;
}
