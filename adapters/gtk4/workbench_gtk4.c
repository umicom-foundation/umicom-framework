/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_gtk4.c
 *
 * PURPOSE:
 *   Coordinate GTK4 refresh operations from immutable Framework workbench models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Batch 23 chrome refreshes first, followed by existing panes, documents, status and dialogs.
 */

#include "gtk4_internal.h"

#define UMI_GTK4_ACTIVITY_RAIL_WIDTH 48
#define UMI_GTK4_APPROXIMATE_CHROME_HEIGHT 96

/* Return the number of records represented by clamp without changing their state. */
static int clamp_size(int value, int minimum, int maximum)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (maximum < minimum) return minimum;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value < minimum) return minimum;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value > maximum) return maximum;
    return value;
}

/*
 * Provide the apply layout state operation used by this module and its client
 * applications.
 */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (default_width <= 0) default_width = 1440;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (state->editor_split_mode != UMI_UI_EDITOR_SPLIT_SINGLE) {
        gtk_paned_set_position(GTK_PANED(adapter->editor_paned),
                               editor_position);
    }
    adapter->applying_layout_state = 0;
}

/* Provide the chrome visible operation used by this module and its client applications. */
static int chrome_visible(
    const UmiGtk4Adapter *adapter,
    UmiGtk4ChromeFlags flag)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!adapter->chrome_visibility_configured) return 1;
    return (adapter->chrome_visibility & flag) != 0U;
}

/*
 * Provide the apply chrome visibility operation used by this module and its client
 * applications.
 */
static void apply_chrome_visibility(UmiGtk4Adapter *adapter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL) return;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->menu_bar != NULL) {
        gtk_widget_set_visible(
            adapter->menu_bar,
            chrome_visible(adapter, UMI_GTK4_CHROME_MENU));
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->toolbar_box != NULL) {
        gtk_widget_set_visible(
            adapter->toolbar_box,
            chrome_visible(adapter, UMI_GTK4_CHROME_TOOLBAR));
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->activity_box != NULL) {
        gtk_widget_set_visible(
            adapter->activity_box,
            chrome_visible(adapter, UMI_GTK4_CHROME_ACTIVITY));
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->sidebar_box != NULL) {
        gtk_widget_set_visible(
            adapter->sidebar_box,
            chrome_visible(adapter, UMI_GTK4_CHROME_PRIMARY_SIDEBAR));
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->right_box != NULL) {
        gtk_widget_set_visible(
            adapter->right_box,
            chrome_visible(adapter, UMI_GTK4_CHROME_SECONDARY_SIDEBAR));
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->bottom_box != NULL) {
        gtk_widget_set_visible(
            adapter->bottom_box,
            chrome_visible(adapter, UMI_GTK4_CHROME_BOTTOM_PANEL));
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->status_box != NULL) {
        gtk_widget_set_visible(
            adapter->status_box,
            chrome_visible(adapter, UMI_GTK4_CHROME_STATUS));
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter->desktop_layout_bar != NULL) {
        gtk_widget_set_visible(
            adapter->desktop_layout_bar,
            chrome_visible(adapter, UMI_GTK4_CHROME_DESKTOP_LAYOUT));
    }
}

/*
 * Provide the gtk4 refresh workbench operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_workbench(UmiGtk4Adapter *adapter)
{
    UmiUiWorkbench *workbench;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->shell == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    workbench = umi_ui_application_shell_workbench(adapter->shell);

    {
        UmiUiWorkbenchState state;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_toolbar(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_gtk4_refresh_workspace_profiles(adapter, workbench);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_gtk4_refresh_appearance(adapter, workbench);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_activity_bar(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_view_container(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_breadcrumbs(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_panes(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_documents(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_desktop_shell(adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_status(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_notifications(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_apply_theme(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_accessibility(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_refresh_selection(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_gtk4_process_dialog(adapter, workbench);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        umi_gtk4_refresh_quick_access_request(adapter, workbench);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        /*
         * Individual refreshers preserve their historical model-driven
         * visibility.  A configured embedding host receives final authority
         * only over native chrome presentation, never over the models.
         */
        apply_chrome_visibility(adapter);
    }
    return status;
}

/*
 * Provide the gtk4 adapter set chrome visibility operation used by this module and its
 * client applications.
 */
UmiStatus umi_gtk4_adapter_set_chrome_visibility(
    UmiGtk4Adapter *adapter,
    UmiGtk4ChromeFlags visible_chrome)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL ||
        (visible_chrome & ~((UmiGtk4ChromeFlags)UMI_GTK4_CHROME_ALL)) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    adapter->chrome_visibility = visible_chrome;
    adapter->chrome_visibility_configured = 1;
    apply_chrome_visibility(adapter);
    return UMI_STATUS_OK;
}

/*
 * Provide the gtk4 adapter chrome visibility operation used by this module and its client
 * applications.
 */
UmiGtk4ChromeFlags umi_gtk4_adapter_chrome_visibility(
    const UmiGtk4Adapter *adapter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL) return 0U;
    return adapter->chrome_visibility_configured
        ? adapter->chrome_visibility
        : (UmiGtk4ChromeFlags)UMI_GTK4_CHROME_ALL;
}
