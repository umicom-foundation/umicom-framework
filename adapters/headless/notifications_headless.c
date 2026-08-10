/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/notifications_headless.c
 *
 * PURPOSE:
 *   Render active notifications with severity for headless operational inspection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "headless_internal.h"

UmiStatus umi_headless_render_notifications(UmiUiHeadlessAdapter *adapter,
                                            UmiUiWorkbench *workbench)
{
    UmiUiNotificationCentre *centre = umi_ui_workbench_notifications(workbench);
    size_t index;
    UmiStatus status = umi_headless_append(adapter, "[notifications]\n");
    for (index = 0U; status == UMI_STATUS_OK && index < umi_ui_notification_count(centre, 1); ++index) {
        UmiUiNotification notification;
        status = umi_ui_notification_at(centre, index, &notification);
        if (status == UMI_STATUS_OK && !notification.dismissed) {
            status = umi_headless_append(adapter, "%llu|%s|%s|%s\n",
                (unsigned long long)notification.notification_id,
                umi_ui_severity_text(notification.severity),
                notification.title,
                notification.message);
            adapter->notification_count += 1U;
        }
    }
    return status;
}
