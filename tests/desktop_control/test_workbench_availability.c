/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_workbench_availability.c
 * PURPOSE: Validate the Framework-owned workbench availability contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/workbench_availability.h"

int main(void)
{
    UmiDesktopWorkbenchAvailabilitySnapshot value;
    UmiDesktopWorkbenchAvailabilitySnapshot copy;
    umi_desktop_workbench_availability_init(&value, "workbench_availability.primary");
    assert(umi_desktop_workbench_availability_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_workbench_availability_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_workbench_availability_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_workbench_availability_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_workbench_availability_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_workbench_availability_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_workbench_availability_selectable(UMI_DESKTOP_CONTROL_AVAILABILITY_DEGRADED));
    return 0;
}
