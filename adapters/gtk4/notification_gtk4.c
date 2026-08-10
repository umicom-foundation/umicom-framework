/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/notification_gtk4.c
 *
 * PURPOSE:
 *   Render the newest active Framework notification in the GTK4 shell.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

UmiStatus umi_gtk4_refresh_notifications(UmiGtk4Adapter *adapter,
                                         UmiUiWorkbench *workbench)
{
    UmiUiNotificationCentre *centre;
    size_t count;
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    centre = umi_ui_workbench_notifications(workbench);
    count = umi_ui_notification_count(centre, 1);
    if (count > 0U) {
        UmiUiNotification notification;
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
