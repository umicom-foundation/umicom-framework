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
        }
    }

    status = umi_gtk4_refresh_menu(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_toolbar(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_activity_bar(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_view_container(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_breadcrumbs(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_panes(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_documents(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_status(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_notifications(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_apply_theme(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_accessibility(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_selection(adapter, workbench);
    if (status == UMI_STATUS_OK) status = umi_gtk4_process_dialog(adapter, workbench);
    return status;
}
