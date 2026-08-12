/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/pane_gtk4.c
 *
 * PURPOSE:
 *   Map toolkit-neutral pane metadata and registered view-model presentations
 *   into left, right and bottom GTK4 workbench containers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Centre documents are rendered by the document/editor adapter. Side and bottom
 * panes ask Framework's view-factory registry for a toolkit-neutral presentation.
 * If a product has not registered a factory yet, the old view-type label remains
 * as a safe fallback instead of making the whole workbench fail.
 */

#include "gtk4_internal.h"

static GtkWidget *pane_target(UmiGtk4Adapter *adapter, UmiUiPlacement placement)
{
    if (placement == UMI_UI_PLACEMENT_LEFT) return adapter->left_box;
    if (placement == UMI_UI_PLACEMENT_RIGHT) return adapter->right_box;
    if (placement == UMI_UI_PLACEMENT_BOTTOM ||
        placement == UMI_UI_PLACEMENT_TOP) {
        return adapter->bottom_box;
    }

    /*
     * UMI_UI_PLACEMENT_CENTRE belongs to the document/editor host. Previously
     * it fell through to the left sidebar, which could render the Editor as a
     * second sidebar pane.
     */
    return NULL;
}

UmiStatus umi_gtk4_refresh_panes(UmiGtk4Adapter *adapter,
                                 UmiUiWorkbench *workbench)
{
    UmiUiPaneModel *model;
    size_t index;

    if (adapter == NULL || workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_gtk4_clear_box(adapter->left_box);
    umi_gtk4_clear_box(adapter->right_box);
    umi_gtk4_clear_box(adapter->bottom_box);

    model = umi_ui_workbench_panes(workbench);
    for (index = 0U; index < umi_ui_pane_model_count(model); ++index) {
        UmiUiPaneSnapshot pane;
        UmiStatus status;

        status = umi_ui_pane_model_at(model, index, &pane);
        if (status != UMI_STATUS_OK) return status;
        if (!pane.visible) continue;

        {
            GtkWidget *target = pane_target(adapter, pane.placement);
            GtkWidget *frame;
            GtkWidget *content = NULL;

            if (target == NULL) continue;

            frame = gtk_frame_new(pane.title);
            status = umi_gtk4_build_view_widget(workbench, &pane, &content);
            if (status != UMI_STATUS_OK) return status;

            gtk_widget_set_hexpand(frame, TRUE);
            gtk_widget_set_vexpand(frame, TRUE);
            gtk_frame_set_child(GTK_FRAME(frame), content);
            gtk_box_append(GTK_BOX(target), frame);
        }
    }
    return UMI_STATUS_OK;
}
