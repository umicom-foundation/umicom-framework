/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/notifications_headless.c
 *
 * PURPOSE:
 *   Render active notifications with severity for headless operational inspection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "headless_internal.h"

/*
 * Provide the headless render notifications operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_notifications(UmiUiHeadlessAdapter *adapter,
                                            UmiUiWorkbench *workbench)
{
    UmiUiNotificationCentre *centre = umi_ui_workbench_notifications(workbench);
    size_t index;
    UmiStatus status = umi_headless_append(adapter, "[notifications]\n");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < umi_ui_notification_count(centre, 1); ++index) {
        UmiUiNotification notification;
        status = umi_ui_notification_at(centre, index, &notification);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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
