/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_gtk4.c
 *
 * PURPOSE:
 *   Coordinate GTK4 refresh operations from immutable Framework workbench models.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Batch 23 chrome refreshes first, followed by existing panes, documents, status and dialogs.
 */

#include "gtk4_internal.h"

#define UMI_GTK4_ACTIVITY_RAIL_WIDTH 48
#define UMI_GTK4_APPROXIMATE_CHROME_HEIGHT 96

static int clamp_size(int value, int minimum, int maximum)
{
    if (maximum < minimum) return minimum;
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

static void apply_layout_state(UmiGtk4Adapter *adapter,
                               const UmiUiWorkbenchState *state)
{
    int default_width = 1440;
    int default_height = 900;
    int left_position;
    int centre_available;
    int centre_position;
    int content_available;
    int content_position;
    int editor_available;
    int editor_position;
    GtkOrientation editor_orientation;

    gtk_window_get_default_size(adapter->window,
                                &default_width,
                                &default_height);
    if (default_width <= 0) default_width = 1440;
    if (default_height <= 0) default_height = 900;

    left_position = UMI_GTK4_ACTIVITY_RAIL_WIDTH +
        clamp_size(state->sidebar_size, 180, default_width / 2);
    centre_available = default_width - left_position;
    centre_position = centre_available -
        clamp_size(state->auxiliary_sidebar_size, 200, centre_available / 2);
    content_available = default_height - UMI_GTK4_APPROXIMATE_CHROME_HEIGHT;
    content_position = content_available -
        clamp_size(state->bottom_panel_size, 160, content_available / 2);
    editor_orientation = state->editor_split_mode == UMI_UI_EDITOR_SPLIT_ROWS
        ? GTK_ORIENTATION_VERTICAL
        : GTK_ORIENTATION_HORIZONTAL;
    editor_available = editor_orientation == GTK_ORIENTATION_HORIZONTAL
        ? gtk_widget_get_width(adapter->editor_paned)
        : gtk_widget_get_height(adapter->editor_paned);
    if (editor_available <= 0) {
        editor_available = editor_orientation == GTK_ORIENTATION_HORIZONTAL
            ? centre_available
            : content_available;
    }
    editor_position = (int)(((int64_t)editor_available *
                             state->editor_split_ratio) / 10000);

    adapter->applying_layout_state = 1;
    gtk_orientable_set_orientation(GTK_ORIENTABLE(adapter->editor_paned),
                                   editor_orientation);
    gtk_widget_set_visible(
        adapter->secondary_document_notebook,
        state->editor_split_mode != UMI_UI_EDITOR_SPLIT_SINGLE);
    gtk_paned_set_position(GTK_PANED(adapter->middle_paned), left_position);
    gtk_paned_set_position(GTK_PANED(adapter->centre_paned), centre_position);
    gtk_paned_set_position(GTK_PANED(adapter->content_paned), content_position);
    if (state->editor_split_mode != UMI_UI_EDITOR_SPLIT_SINGLE) {
        gtk_paned_set_position(GTK_PANED(adapter->editor_paned),
                               editor_position);
    }
    adapter->applying_layout_state = 0;
}

UmiStatus umi_gtk4_refresh_workbench(UmiGtk4Adapter *adapter)
{
    UmiUiWorkbench *workbench;
    UmiStatus status;
    if (adapter == NULL || adapter->shell == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    workbench = umi_ui_application_shell_workbench(adapter->shell);

    {
        UmiUiWorkbenchState state;
        if (umi_ui_workbench_state_snapshot(workbench, &state) == UMI_STATUS_OK) {
            gtk_widget_set_visible(adapter->sidebar_box, state.sidebar_visible != 0);
            gtk_widget_set_visible(adapter->right_box,
                                   state.auxiliary_sidebar_visible != 0);
            gtk_widget_set_visible(adapter->bottom_box,
                                   state.bottom_panel_visible != 0);
            apply_layout_state(adapter, &state);
        }
    }

    status = umi_gtk4_refresh_menu(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_toolbar(adapter, workbench);
    if (status == UMI_STATUS_OK) {
        status = umi_gtk4_refresh_workspace_profiles(adapter, workbench);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_gtk4_refresh_appearance(adapter, workbench);
    }
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_activity_bar(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_view_container(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_breadcrumbs(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_panes(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_documents(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_desktop_shell(adapter);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_status(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_notifications(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_apply_theme(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_accessibility(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_selection(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_process_dialog(adapter, workbench);
    if (status == UMI_STATUS_OK) {
        umi_gtk4_refresh_quick_access_request(adapter, workbench);
    }
    return status;
}
