/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_notification_centre.c
 * PURPOSE: Validate the Framework-owned notification centre contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/notification_centre.h"

int main(void)
{
    UmiDesktopNotificationCentreSnapshot value;
    UmiDesktopNotificationCentreSnapshot copy;
    umi_desktop_notification_centre_init(&value, "notification_centre.primary");
    assert(umi_desktop_notification_centre_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_notification_centre_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_notification_centre_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_notification_centre_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_notification_centre_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_notification_centre_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_notification_centre_visible(UMI_DESKTOP_CONTROL_SEVERITY_ERROR, UMI_DESKTOP_CONTROL_SEVERITY_WARNING, false));
    return 0;
}
