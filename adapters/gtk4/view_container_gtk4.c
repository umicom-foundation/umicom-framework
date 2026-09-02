/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/view_container_gtk4.c
 *
 * PURPOSE:
 *   Present the title and state of the active Framework view container.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Existing pane rendering remains responsible for view content; this presenter provides the reusable container-level navigation header.
 */

#include "gtk4_internal.h"

/*
 * Provide the gtk4 refresh view container operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_view_container(UmiGtk4Adapter *adapter,
                                         UmiUiWorkbench *workbench)
{
    UmiUiViewContainerModel *model;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    model = umi_ui_workbench_view_containers(workbench);
    gtk_label_set_text(GTK_LABEL(adapter->sidebar_header), "");

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_ui_view_container_model_count(model); ++index) {
        UmiUiViewContainerSnapshot container;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_ui_view_container_model_at(model, index, &container) == UMI_STATUS_OK &&
            container.active) {
            char title[UMI_UI_TEXT_CAPACITY + 48U];
            /* Apply this branch only when its contract condition is satisfied. */
            if (container.badge_count > 0U) {
                (void)g_snprintf(title, sizeof(title), "%s  (%u)",
                                 container.title,
                                 (unsigned)container.badge_count);
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                (void)g_snprintf(title, sizeof(title), "%s", container.title);
            }
            gtk_label_set_text(GTK_LABEL(adapter->sidebar_header), title);
            gtk_widget_set_tooltip_text(adapter->sidebar_header,
                                        container.description);
            break;
        }
    }
    return UMI_STATUS_OK;
}
