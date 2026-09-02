/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_application_update_state.c
 * PURPOSE: Validate the Framework-owned application update state contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/application_update_state.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopApplicationUpdateStateSnapshot value;
    UmiDesktopApplicationUpdateStateSnapshot copy;
    umi_desktop_application_update_state_init(&value, "application_update_state.primary");
    assert(umi_desktop_application_update_state_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_application_update_state_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_application_update_state_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_application_update_state_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_application_update_state_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_application_update_state_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
