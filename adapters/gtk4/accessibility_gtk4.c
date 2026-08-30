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

UmiStatus umi_gtk4_refresh_accessibility(UmiGtk4Adapter *adapter,
                                         UmiUiWorkbench *workbench)
{
    UmiUiWorkbenchSnapshot snapshot;
    if (adapter == NULL || adapter->window == NULL || workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_ui_workbench_snapshot(workbench, &snapshot) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    gtk_widget_set_tooltip_text(GTK_WIDGET(adapter->window), snapshot.workbench_id);
    return UMI_STATUS_OK;
}
