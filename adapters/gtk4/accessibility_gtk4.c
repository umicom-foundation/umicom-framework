/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/accessibility_gtk4.c
 *
 * PURPOSE:
 *   Apply accessible application-shell metadata while keeping detailed node mappings
 *   inside the GTK4 adapter boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

/*
 * Provide the gtk4 refresh accessibility operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_accessibility(UmiGtk4Adapter *adapter,
                                         UmiUiWorkbench *workbench)
{
    UmiUiWorkbenchSnapshot snapshot;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->window == NULL || workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ui_workbench_snapshot(workbench, &snapshot) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    gtk_widget_set_tooltip_text(GTK_WIDGET(adapter->window), snapshot.workbench_id);
    return UMI_STATUS_OK;
}
