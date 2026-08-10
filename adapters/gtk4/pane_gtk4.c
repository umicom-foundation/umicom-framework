/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/pane_gtk4.c
 *
 * PURPOSE:
 *   Map toolkit-neutral pane metadata into left, right and bottom GTK4 containers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

static GtkWidget *pane_target(UmiGtk4Adapter *adapter, UmiUiPlacement placement)
{
    if (placement == UMI_UI_PLACEMENT_RIGHT) return adapter->right_box;
    if (placement == UMI_UI_PLACEMENT_BOTTOM || placement == UMI_UI_PLACEMENT_TOP) {
        return adapter->bottom_box;
    }
    return adapter->left_box;
}

UmiStatus umi_gtk4_refresh_panes(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench)
{
    UmiUiPaneModel *model;
    size_t index;
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_gtk4_clear_box(adapter->left_box);
    umi_gtk4_clear_box(adapter->right_box);
    umi_gtk4_clear_box(adapter->bottom_box);
    model = umi_ui_workbench_panes(workbench);
    for (index = 0U; index < umi_ui_pane_model_count(model); ++index) {
        UmiUiPaneSnapshot pane;
        if (umi_ui_pane_model_at(model, index, &pane) == UMI_STATUS_OK && pane.visible) {
            GtkWidget *frame = gtk_frame_new(pane.title);
            GtkWidget *label = gtk_label_new(pane.view_type);
            gtk_widget_set_hexpand(frame, TRUE);
            gtk_widget_set_vexpand(frame, TRUE);
            gtk_frame_set_child(GTK_FRAME(frame), label);
            gtk_box_append(GTK_BOX(pane_target(adapter, pane.placement)), frame);
        }
    }
    return UMI_STATUS_OK;
}
