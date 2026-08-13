/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_distribution_notification.c
 *
 * PURPOSE:
 *   Verify actionable update notification storage and read state.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "distribution_fixture.h"

int main(void)
{
    UmiDistributionNotificationCentre *centre = NULL;
    UmiDistributionNotification notification;
    (void)memset(&notification, 0, sizeof(notification));
    (void)strcpy(notification.notification_id, "update.studio-0.17.0");
    (void)strcpy(notification.product_id, "org.umicom.studio");
    (void)strcpy(notification.title, "Studio update available");
    notification.kind = UMI_DISTRIBUTION_NOTIFICATION_UPDATE;
    notification.severity = UMI_DISTRIBUTION_NOTICE_ACTION;
    assert(umi_distribution_notification_centre_create(&centre) == UMI_STATUS_OK);
    assert(umi_distribution_notification_centre_upsert(centre, &notification) ==
           UMI_STATUS_OK);
    assert(umi_distribution_notification_centre_unread_count(centre) == 1U);
    assert(umi_distribution_notification_centre_mark_read(
        centre, notification.notification_id, 1) == UMI_STATUS_OK);
    assert(umi_distribution_notification_centre_unread_count(centre) == 0U);
    umi_distribution_notification_centre_destroy(centre);
    return 0;
}
