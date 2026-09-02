/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_workbench_profile.c
 * PURPOSE: Validate the Framework-owned workbench profile contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/workbench_profile.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopWorkbenchProfileSnapshot value;
    UmiDesktopWorkbenchProfileSnapshot copy;
    umi_desktop_workbench_profile_init(&value, "workbench_profile.primary");
    assert(umi_desktop_workbench_profile_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_workbench_profile_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_workbench_profile_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_workbench_profile_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_workbench_profile_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_workbench_profile_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
