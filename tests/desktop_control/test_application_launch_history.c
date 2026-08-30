/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_application_launch_history.c
 * PURPOSE: Validate the Framework-owned application launch history contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/application_launch_history.h"

int main(void)
{
    UmiDesktopApplicationLaunchHistorySnapshot value;
    UmiDesktopApplicationLaunchHistorySnapshot copy;
    umi_desktop_application_launch_history_init(&value, "application_launch_history.primary");
    assert(umi_desktop_application_launch_history_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_application_launch_history_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_application_launch_history_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_application_launch_history_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_application_launch_history_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_application_launch_history_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
