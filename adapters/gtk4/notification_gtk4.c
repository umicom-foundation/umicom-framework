/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/notification_gtk4.c
 *
 * PURPOSE:
 *   Render the newest active Framework notification in the GTK4 shell.
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
 * Provide the gtk4 refresh notifications operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_notifications(UmiGtk4Adapter *adapter,
                                         UmiUiWorkbench *workbench)
{
    UmiUiNotificationCentre *centre;
    size_t count;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    centre = umi_ui_workbench_notifications(workbench);
    count = umi_ui_notification_count(centre, 1);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > 0U) {
        UmiUiNotification notification;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_ui_notification_at(centre, count - 1U, &notification) == UMI_STATUS_OK && !notification.dismissed) {
            gtk_label_set_text(GTK_LABEL(adapter->notification_label), notification.message);
            gtk_widget_set_visible(adapter->notification_label, TRUE);
            return UMI_STATUS_OK;
        }
    }
    gtk_label_set_text(GTK_LABEL(adapter->notification_label), "");
    gtk_widget_set_visible(adapter->notification_label, FALSE);
    return UMI_STATUS_OK;
}
