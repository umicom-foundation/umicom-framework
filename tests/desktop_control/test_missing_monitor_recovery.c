/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_missing_monitor_recovery.c
 * PURPOSE: Validate the Framework-owned missing monitor recovery contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/missing_monitor_recovery.h"

int main(void)
{
    UmiDesktopMissingMonitorRecoverySnapshot value;
    UmiDesktopMissingMonitorRecoverySnapshot copy;
    umi_desktop_missing_monitor_recovery_init(&value, "missing_monitor_recovery.primary");
    assert(umi_desktop_missing_monitor_recovery_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_missing_monitor_recovery_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_missing_monitor_recovery_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_missing_monitor_recovery_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_missing_monitor_recovery_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_missing_monitor_recovery_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    UmiDesktopControlRect bounds = {0,0,100,100}; UmiDesktopControlRect geometry = {500,500,20,20};
    assert(umi_desktop_missing_monitor_recovery_recover(&bounds, &geometry) == UMI_STATUS_OK); assert(geometry.x == 80 && geometry.y == 80);
    return 0;
}
